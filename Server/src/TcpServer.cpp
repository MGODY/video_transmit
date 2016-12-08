#include "tcpserver.h"

QMutex  CTcpserv::mutex_robots;
QMutex  CTcpserv::mutex_clients;
vector<Camera*> CTcpserv::robot_device;
vector<Client*> CTcpserv::client_device;
/*---------------------------------------------------------------------------------------------
*	Camera  类的实现部分
*-	@author  mgody.fengkun@gmail.com
*-	@time 2016年11月29日 星期二 20时08分23秒 
------------------------------------------------------------------------------------------------*/
Camera::~Camera()
{
	cout<<"Robot device "<<ID<<" destroied"<<endl;
	close(socket);
}
int  Camera::ison()
{


	int ison(on);
	if(ison==0x55)
		ison = 1;
	else if(ison==0x77)
		ison = 0;
	else 
		ison = -1;
	return ison;
}
int Camera::isconnect()
{
	int isconnect(connect_i);
	if(isconnect==0x55)
		isconnect = 1;
	else if(isconnect==0x77)
		isconnect = 0;
	else 
		isconnect = -1;
	return isconnect;
}
void Camera::run()
{
	int count = 0;
	int recvsize;
	Mtype::SMeseg meseg;
	Mtype::StateRobot state;
	string ssend;
	while(run_flag)
	{
		memset((void*)&state,0,sizeof(Mtype::StateRobot));
		memset( (void*)&meseg, 0, sizeof(Mtype::SMeseg));
		mutex_socket.lock();
		if((recvsize=recv(socket, (void*)&meseg, sizeof(Mtype::SMeseg), 0)) == -1)
		{
			mutex_socket.unlock();
			(errno == EINTR)||(errno == EWOULDBLOCK)||(errno == EAGAIN)||(connect_i  = 0xAA);
			
			
		}else if(recvsize == 0)
		{
			mutex_socket.unlock();
			run_flag = 0;
			cout<<"Robot"<<ID<<" disconected"<<endl;
			connect_i  = 0xAA;
			//%%%connect error process
			//perror("recv failed");
			//exit(EXIT_FAILURE);
		}else 
		{
			mutex_socket.unlock();
			dealmesegin(&meseg,&state);
			//ssend = (char*)connect;
			//emit toclient(QString QString::fromStdString(ssend));
		}
		count++;
		//%%%update
		msleep(100);
	}

}


Mtype::StateRobot Camera::getstate()
{
	mutex_robot.lock();
	
	Mtype::StateRobot state;
	memset((void*)&state,0,sizeof(Mtype::StateRobot));
	state.ID = ID;
	state.on = on;
	state.connect_i = connect_i;
	state.height = height;
	state.wide = wide;
	state.error = error;
	state.rate_control = rate_control;
	memcpy(state.ip_address,ip_address.c_str(),ip_address.size());

	mutex_robot.unlock();
	return state;
}
int Camera::update(const Mtype::StateRobot * const state)
{
	
	if(state->ID!=ID)
	{
		cout<<"robot"<<ID<<"'s update messeg is wrong"<<endl;
		
	}else
	{
		mutex_robot.lock();
		if(state->on != on)
		{
			on = state->on;
			cout<<"robot"<<ID<<"'s on is changed,please check the robot"<<endl;
		}
		if(state->connect_i != connect_i)
		{
			connect_i = state->connect_i;
			cout<<"robot"<<ID<<"'s connect is changed,please check the robot"<<endl;
		}
		if(state->height != height)
		{
			height = state->height;
			cout<<"robot"<<ID<<"'s height is changed,please check the robot"<<endl;
		}
		if(state->wide != wide)
		{
			wide = state->wide;
			cout<<"robot"<<ID<<"'s wide is changed,please check the robot"<<endl;
		}
		if(state->rate_control != rate_control)
		{
			rate_control = state->rate_control;
			cout<<"robot"<<ID<<"'s rate_control is changed,please check the robot"<<endl;
		}
		switch(state->error)
		{
			case 0xAA: 
				cout<<"no error"<<endl;
				break;
			default :
				//cout<<""
				break;
			//%%%error
		}
		mutex_robot.unlock();
	}
	return 0;
}
int Camera::deal_rmesegs(const Mtype::SMeseg *const meseg,Mtype::StateRobot *state)
{
	
	switch( meseg->type)
	{
		case 0x01:	
			memcpy((void*)state, (void*)(meseg->buffer),sizeof(Mtype::StateRobot));
			update(state);
			//由于连接的意义在于维持TCP连接，所以不处理数据也是可以的。
			//%%%timer 使用一个计时器来定时更新，后期再考虑。
			break;
		case 0xAA:  
			mutex_robot.lock();
			connect_i = 0xAA;
			mutex_robot.unlock();
			break;
		case 0x55:	
			cout<<"recevive a wrong connect from robot "<<ID<<endl;
			//%%%log todo 
			break;
		case 0x88:
			{

			string sout{(char*)meseg};//C风格字符串用来初始化string的时候会被空字符隔断，所以不用担心string会跟buffer一样大。
										//从这点来说有利有弊吧，所以想要跟C字符串一样大可以这样做：
										//sout.assign(buf,sizeof(buffer)/sizeof(char));  
										//当然这里不需要这么做。
			sout[1] = sout[0];
			sout[0] = 0x20;
			sout[2] = 0x77;
			
			sout = sout+" i'm a AI by MGODY ,i have heard you!! haha";
			Mtype::SMeseg mesegs;
			memset((void*)&mesegs,0,sizeof(Mtype::SMeseg));
			memcpy((char*)&mesegs,sout.c_str(),sout.size());
			sendmeseg(&mesegs);
			//string sout(meseg->buffer);
			cout<<"test meseg from robot"<<ID<<": "<<mesegs.buffer<<endl;
			break;
			}
		case 0x77:	
			switch(meseg->buffer[0])
			{
				case 0x01:	
					if(meseg->buffer[1]==0x55)
					{
						memcpy((void*)state, (void*)(&((meseg->buffer)[2])),
							sizeof(Mtype::StateRobot));
						update(state);
						break;
					}
					break;
				//%%%case
				default: break;
			}
			break;
		default : 
				//%%%log todo
				break;
	}
	return 0;
}
int Camera::dealmesegin(const Mtype::SMeseg *const meseg,Mtype::StateRobot *state)
{	
	string ssend;
	
	//QString qs_send;
	if(((int)(meseg->source_id&0x0f)==ID)&&((meseg->source_id&0xf0)==0x10))
	{
		switch(meseg->to_id&0xf0)
		{
			case 0x20:
				{   
					deal_rmesegs(meseg,state);
					break;
	            }
			case 0x30:
				{
					ssend = (char*)meseg;
					QString qs_send = QString::fromStdString(ssend);
					emit toclient(qs_send );
				}
			case 0x50:
				{
					deal_rmesegs(meseg,state);
					ssend = (char*)meseg;
					QString qs_send = QString::fromStdString(ssend);
					emit toclient(qs_send);
				}
			default:
				//%%%log todo
				cout<<"get a wrong to_id"<<(((meseg->to_id)&0xf0)>>4)<<((meseg->to_id)&0x0f)<<endl;
				break;
		}
		
	}else
	{
		//%%%log todo
		cout<<"get a wrong souce_id"<<(((meseg->source_id)&0xf0)>>4)<<
			((meseg->source_id)&0x0f)<<endl;
	}
	
	return 0;

}
int Camera::sendmeseg(const Mtype::SMeseg *const meseg)
{
	mutex_socket.lock();
	string ssend((char*)meseg);
	
	if ( send(socket, ssend.c_str(), ssend.size(), 0) == -1 )
	{
		cout<<"send failed"<<endl;
		exit(EXIT_FAILURE);
	}
	//%%%connect error process
	mutex_socket.unlock();
	return 0;
}
void Camera::dealmeseg(const QString qs_meseg)
{
	
	Mtype::SMeseg meseg;
	//Mtype::StateRobot state;
	string s_meseg;
	s_meseg = qs_meseg.toStdString();
	memcpy((void*)&meseg,(void*)s_meseg.c_str(),s_meseg.size());
	
	switch(meseg.source_id&0xf0)
	{
		case 0x20:
			if (((meseg.to_id&0xf0)==0x10)&&((meseg.to_id&0x0f)==(unsigned char)(ID)))
			{
				sendmeseg(&meseg);
			}
			else if((meseg.to_id == 0x4f)||(meseg.to_id==0x1f))
			{
				meseg.to_id = (0x10|(unsigned char)(ID));
				sendmeseg(&meseg);
			}
			else 
			{
				cout<<"meseg's error (to_id) from server "<<endl;
			}
			break;
		case 0x30:
			if (((meseg.to_id&0xf0)==0x10)&&((meseg.to_id&0x0f)==(unsigned char)(ID)))
			{
				sendmeseg(&meseg);
			}
			else if((meseg.to_id == 0x4f)||(meseg.to_id==0x1f))
			{
				meseg.to_id = (0x10|(unsigned char)(ID));
				sendmeseg(&meseg);
			}
			else 
			{	
				//%%%log todo
				cout<<"meseg's illegal (to_id) from server running in camera "<<ID<<endl;
			}
			break;
		default:
			//%%%log todo
			cout<<"meseg's illegal (source_id) from server running in camera "<<ID<<endl;
			break;
	}
	
	
}
/*---------------------------------------------------------------------------------------------
*	Client  类的实现部分
*-	@author  mgody.fengkun@gmail.com
*-	@time 2016-11-23
------------------------------------------------------------------------------------------------*/

Client::~Client()
{
	cout<<"Client device "<<ID<<" destroied"<<endl;
	close(socket);
}

void Client::run()
{

	int count = 0;
	int recvsize;
	Mtype::SMeseg meseg;
	Mtype::StateClient state;
	string ssend;
	
	while(run_flag)
	{
		memset(	(void*)&state,0,sizeof(Mtype::StateClient));
		memset( (void*)&meseg, 0, sizeof(Mtype::SMeseg));
		mutex_socket.lock();
		if((recvsize=recv(socket, (void*)&meseg, sizeof(Mtype::SMeseg), 0)) == -1)
		{
			mutex_socket.unlock();
			(errno == EINTR)||(errno == EWOULDBLOCK)||(errno == EAGAIN)||(connect_i  = 0xAA);
			
			
		}else if(recvsize == 0)
		{
			mutex_socket.unlock();
			run_flag = 0;
			cout<<"Client"<<ID<<" disconected"<<endl;
			connect_i  = 0xAA;
			//%%%connect error process
			//perror("recv failed");
			//exit(EXIT_FAILURE);
		}else 
		{
			mutex_socket.unlock();
			///cout<<"received meseg test"<<endl;
			dealmesegin(&meseg,&state);
			//ssend = (char*)connect;
			//emit toclient(QString QString::fromStdString(ssend));
		}
		count++;
		cout<<"client count test"<<count<<endl;
		//%%%update
		msleep(100);
	}

}
Mtype::StateClient Client::getstate()
{
	Mtype::StateClient state;
	mutex_client.lock();
	
	state.ID = ID;
	state.on = on;
	state.nrobot =Getlist.size();
	state.connect_i = connect_i;
	state.error = error;
	
	mutex_client.unlock();
	return state;
}

string Client::get_robotlist()
{
	
	mutex_client.lock();
	string roblist(Getlist.size(),' ');
	for(int i=0;i<Getlist.size();i++)
	{
		roblist[i] = (unsigned char) (Getlist[i]);
	}
	mutex_client.unlock();
	return roblist;
}

int Client::update(const Mtype::StateClient *const state)
{
	if(state->ID!=ID)
	{
		cout<<"client "<<ID<<"'s update messeg is wrong"<<endl;
		
	}else
	{
		mutex_client.lock();
		if(state->on != on)
		{
			on = state->on;
			cout<<"client"<<ID<<"'s on is changed,please check the client"<<endl;
		}
		if(state->connect_i != connect_i)
		{
			connect_i = state->connect_i;
			cout<<"client"<<ID<<"'s connect is changed,please check the client"<<endl;
		}
		if(state->nrobot != Getlist.size())
		{
			//height = state->height;
			cout<<"client"<<ID<<"'s nrobot is changed,please check the client"<<endl;
		}

		switch(state->error)
		{
			case 0xAA: 
				cout<<"no error"<<endl;
				break;
			default :
				//cout<<""
				break;
			//%%%error
		}
		mutex_client.unlock();
	}
	return 0;
}


int Client::change_getlist(const Mtype::SMeseg *const meseg ,Mtype::StateClient *state)
{

	memcpy((void*)state,(void*)meseg,sizeof(Mtype::StateClient));
	
	mutex_client.lock();
	if(state->nrobot<0)
	{
		cout<<"receive a wrong getlist from client "<<ID<<endl;
	}else if(state->nrobot==0)
	{
		Getlist.clear();
	}else
	{
		//%%%error process todo
		Getlist.clear();
		while((state->nrobot)--)
		{
			Getlist.push_back((int)(state->addmeseg)[state->nrobot]);
		}
		cout<<"client "<<ID<<"'s getlist updated"<<endl;
	}
	mutex_client.unlock();
	return 0;
}
int Client::ison()
{
	int ison(on);
	if(ison==0x55)
		ison = 1;
	else if(ison==0x77)
		ison = 0;
	else 
		ison = -1;
	return ison;
}
int Client::isconnect()
{
	int isconnect(connect_i);
	if(isconnect==0x55)
		isconnect = 1;
	else if(isconnect==0x77)
		isconnect = 0;
	else 
		isconnect = -1;
	return isconnect;
}
int Client::deal_cmesegs(const Mtype::SMeseg *const meseg,Mtype::StateClient *state)
{
	//static Mtype::SMeseg mesegs;
	switch(meseg->type)
	{
		case 0x01:
			memcpy((void*)state,(void*)meseg->buffer,sizeof(Mtype::StateClient));
			update(state);
			break;
		case 0x02:
			flag_send_rstates = 1;
			break;
		case 0x03:
			change_getlist(meseg,state);
			break;
		case 0x55:
			cout<<"error connect from client "<<ID<<endl;
			//%%%log todo
			break;
		case 0xAA:
			connect_i = 0xAA;
			break;
		case 0x77:
			switch(meseg->buffer[0])
			{
				case 0x01:	
					if(meseg->buffer[1]==0x55)
					{
						memcpy((void*)state, (void*)(&((meseg->buffer)[2])),
							sizeof(Mtype::StateRobot));
						update(state);
						break;
					}
					break;
				//%%%case
				default: 
					cout<<"ignore a 0x77 meseg from Client "<<ID<<endl; 
					break;
			}
			break;
		case 0x88:
			{
				//cout<<"received meseg test2"<<endl;
				string sout{(char*)meseg};
				sout[1] = sout[0];
				sout[0] = 0x20;
				sout[2] = 0x77;
				sout = sout+"\n i'm a AI by MGODY ,i have heard you!! haha";
				Mtype::SMeseg mesegs;
				memset((void*)&mesegs,0,sizeof(Mtype::SMeseg));
				memcpy((void*)&mesegs,sout.c_str(),sout.size());
				//cout<<"received meseg test3"<<endl;
				sendmeseg(&mesegs);
				//string sout(meseg->buffer);
				cout<<"test meseg from Client "<<ID<<": "<<mesegs.buffer<<endl;
				break;
			}
		default:
			break;
	}
	return 0;
	
}
int Client::dealmesegin(const Mtype::SMeseg *const meseg,Mtype::StateClient *state)
{
	string ssend;
	QString qs_send;
	if(((meseg->source_id&0xf0)==0x30)&&((meseg->source_id&0x0f)==(unsigned char)ID))
	{
		switch(meseg->to_id&0xf0)
		{
			case 0x20:
				//cout<<"received meseg test1"<<endl;
				deal_cmesegs(meseg,state);
				break;
			case 0x10:
				ssend = (char*)meseg;
				qs_send = QString::fromStdString(ssend);
				emit to_robot(qs_send);
				break;
			case 0x40:
				deal_cmesegs(meseg,state);
				ssend = (char*)meseg;
				qs_send = QString::fromStdString(ssend);
				emit to_robot(qs_send);
				break;
			default:
				//%%%log todo 
				cout<<"receive a wrong to_id from client "<<ID<<endl;
				break;	
		}
	}else
	{
		//%%%log todo 
		cout<<"receive a wrong source_id from client please check the client"<<ID<<endl;
	}
	return 0;

}
int Client::sendmeseg(const Mtype::SMeseg *const meseg)
{
	mutex_socket.lock();
	string ssend((char*)meseg);
	if ( send(socket, ssend.c_str(), ssend.size(), 0) == -1 )
	{
		cout<<"client "<<ID<<" send failed"<<endl;
		exit(EXIT_FAILURE);
	}
	//%%%connect error process
	mutex_socket.unlock();
	return 0;
}
void Client::dealmeseg(QString qs_meseg)
{
	Mtype::SMeseg meseg;
	//Mtype::StateClient state;
	string s_meseg;
	s_meseg = qs_meseg.toStdString();
	memcpy((void*)&meseg,(void*)s_meseg.c_str(),s_meseg.size());
	switch((meseg.source_id)&0xf0)
	{
		
		case 0x10:
			if(((meseg.to_id&0x0f)==(unsigned char)(ID))&&((meseg.to_id&0xf0)==0x30))
				sendmeseg(&meseg);
			else if(meseg.to_id == 0x5f)
			{
				meseg.to_id = (0x30&((unsigned char)(ID)));
				sendmeseg(&meseg);
			}else
			{
				//%%%log todo
				cout<<"get ilegal to_id in client"<<ID<<endl;
			}
			break;
		case 0x20:
			if(((meseg.to_id&0x0f)==(unsigned char)(ID))&&((meseg.to_id&0xf0)==0x30))
				sendmeseg(&meseg);
			else if(meseg.to_id == 0x5f)
			{
				meseg.to_id = (0x30&((unsigned char)(ID)));
				sendmeseg(&meseg);
			}else
			{
				//%%%log todo
				cout<<"get ilegal to_id in client"<<ID<<endl;
			}
			break;
		default:
			//%%%log todo
			cout<<"get ilegal source_id in client"<<ID<<endl;
			break;
		
	
	}
	
	
}




/*---------------------------------------------------------------------------------------------
*	CTcpserv  类的实现部分
*-	@author  mgody.fengkun@gmail.com
*-	@time 2016-11-15
------------------------------------------------------------------------------------------------*/

CTcpserv::CTcpserv()
{
	clientID = 1;
	port = DEFAULT_PORT;

	//创建socket
	
	if ( (sock = socket(PF_INET, SOCK_STREAM, 0)) == -1 ) {
		perror("socket failed, ");
		exit(EXIT_FAILURE);
	}
	printf("socket done\n");

	//in case of 'address already in use' error message
	 yes = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}

	//设置sock为non-blocking
	setSockNonBlock(sock);

	//创建要bind的socket address
	
	memset(&bind_addr, 0, sizeof(bind_addr));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //设置接受任意地址
	bind_addr.sin_port = htons(port);               //将host byte order转换为network byte order

	//bind sock到创建的socket address上
	if ( bind(sock, (struct sockaddr *) &bind_addr, sizeof(bind_addr)) == -1 ) {
		perror("bind failed, ");
		exit(EXIT_FAILURE);
	}
	printf("bind done\n");

	//listen
	if ( listen(sock, 5) == -1) {
		perror("listen failed.");
		exit(EXIT_FAILURE);
	}
	printf("listen done\n");

	//创建并初始化select需要的参数(这里仅监视read)，并把sock添加到fd_set中

	maxfd = sock;
	FD_ZERO(&readfds);
	FD_ZERO(&readfds_bak);
	FD_SET(sock, &readfds_bak);


}
CTcpserv::~CTcpserv()
{

	close(sock);
	cout<<"server shut down!"<<endl;
}



void CTcpserv::run()
{
	Mtype::SMeseg meseg;

	while (1) 
	{

		//注意select之后readfds和timeout的值都会被修改，因此每次都进行重置
		readfds = readfds_bak;
		//maxfd = updateMaxfd(readfds, maxfd);        //更新maxfd
		timeout.tv_sec = SELECT_TIMEOUT;
		timeout.tv_usec = 0;
		//printf("selecting maxfd=%d\n", maxfd);

		//select(这里没有设置writefds和errorfds，如有需要可以设置)
		res = select(maxfd + 1, &readfds, NULL, NULL, &timeout);
		if (res == -1) 
		{
			perror("select failed");
			exit(EXIT_FAILURE);
		} else if (res == 0) 
		{
			update();
			//cout<<"tcpserver updated "<<endl;
			QCoreApplication::processEvents();//处理事件
			//fprintf(stderr, "no socket ready for read within %d secs\n", SELECT_TIMEOUT);
			continue;
		}

		//检查每个socket，并进行读(如果是sock则accept)
		//可读的socket
		if ( FD_ISSET(maxfd, &readfds)) 
		{
			//当前是server的socket，不进行读写而是accept新连接
			client_addr_len = sizeof(client_addr);
			new_sock = accept(sock, (struct sockaddr *) &client_addr, &client_addr_len);
			if (new_sock == -1) 
			{
				perror("accept failed");
				exit(EXIT_FAILURE);
			}
			if (!inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip_str, sizeof(client_ip_str))) 
			{
				perror("inet_ntop failed");
				exit(EXIT_FAILURE);
			}
			printf("accept a client from: %s\n", client_ip_str);
			memset( (void*)&meseg, 0, sizeof(Mtype::SMeseg));
			
			if ( (recv_size = recv(new_sock, (void*)&meseg, sizeof(Mtype::SMeseg), 0)) == -1 ) 
			{
				perror("recv failed");
				exit(EXIT_FAILURE);
			}
			
			creatNewDevice(new_sock,&meseg,client_ip_str);
			setSockNonBlock(new_sock);
			//设置new_sock为non-blocking
			//setSockNonBlock(new_sock);
			//把new_sock添加到select的侦听中
			
		}
		update();
		//cout<<"tcpserver updated "<<endl;
		QCoreApplication::processEvents();//处理事件 
		

	}

}
int CTcpserv::send_robot_states(vector<Client*>::iterator *p_iter_client = NULL)
{

	Mtype::SMeseg meseg;
	memset((void*)&meseg,0,sizeof(Mtype::SMeseg));
	Mtype::StateRobot state;

	meseg.source_id = 0x20;
	meseg.to_id = 0x30|((unsigned char)((**p_iter_client)->ID));
	meseg.type = 0x77;
	meseg.buffer[0] = 0x02;
	meseg.buffer[1] = 0x55;
	
	mutex_robots.lock();
	auto inter_robot  = robot_device.begin();
	
	char *cpointer = &meseg.buffer[2];
	while(inter_robot!=robot_device.end())
	{
		memset((void*)&state,0,sizeof(Mtype::StateRobot));
		state = (*inter_robot)->getstate();
		memcpy((void*)cpointer,(void*)&state,sizeof(Mtype::StateRobot));
		cpointer += sizeof(Mtype::StateRobot);
		inter_robot++;
	//%%todo 防指针越界。
	}
	mutex_robots.unlock();
	//string ssend((char*)&meseg);
	(**p_iter_client)->sendmeseg(&meseg);
	return 0;
	
}
void CTcpserv::update ()//更新Camera与client两个类的信息。
{
	mutex_robots.lock();
	auto inter_robot  = robot_device.begin();
	
	while(inter_robot!=robot_device.end())
	{
		if(((*inter_robot)->isconnect())<=0)
		{
			delete *inter_robot;
			inter_robot = robot_device.erase(inter_robot);
		}else
		{	
			inter_robot++;
		}
	}
	mutex_robots.unlock();
	
	mutex_clients.lock();
	auto inter_client  = client_device.begin();
	while(inter_client!=client_device.end())
	{
		if( ((*inter_client)->isconnect())<=0)
		{
			delete (*inter_client);
			inter_client = client_device.erase(inter_client);
		}else 
		{
			if ((*inter_client)->flag_send_rstates)
			{
				send_robot_states(&inter_client);
			}
			inter_client++;
		}
	}
	mutex_clients.unlock();
	
	
}

int CTcpserv::creatNewDevice(int sock,Mtype::SMeseg *meseg,char *deviceip)
{

	int deviceid;
//	int devicesock;
	ostringstream sout;
	string sendmeseg;
	if (((meseg->source_id&0xf0)==0x10)&&(meseg->type==0x55))
	{
		
		deviceid = (meseg->source_id&0x0f);
		printf("accept a new robot, id is :%d \n", deviceid);
		Camera *robot;
		robot = new Camera(sock,deviceid,deviceip);
		robot_device.push_back(robot);
		//robot->start();
		
		memset( meseg->buffer, 0, MESEG_BUFF_SIZE);
		
		meseg->to_id =meseg->source_id ;
		meseg->source_id = 0x20;
		meseg->type = 0x55;
		meseg->buffer[0] = 0x01;
		
		
		//sout<<0x20<<(meseg->source_id)<<0x55<<0x01<<endl;
		sendmeseg = (char*)meseg;
		if ( send(sock, sendmeseg.c_str(), sendmeseg.size(), 0) == -1 ) 
		{
			perror("send failed");
			exit(EXIT_FAILURE);
			return -1;
		}
		robot->start();
	}else if(((meseg->source_id&0xf0)==0x30)&&(meseg->type==0x55))
	{
		printf("accept a new client, id is :%d \n", clientID);
		Client *client;
		client = new Client(sock,clientID,deviceip);
		//%%%connect
		
		for(auto robot:robot_device)
		{
			connect(robot,SIGNAL(toclient(const QString)),client,SLOT(dealmeseg(const QString)));
			connect(client,SIGNAL(to_robot(const QString)),robot,SLOT(dealmeseg(const QString)));
		}
		//client->start();
		client_device.push_back(client);
		
		memset( (void*)meseg, 0, sizeof(Mtype::SMeseg));
		meseg->source_id = 0x20;
		meseg->to_id = ((unsigned char)clientID|0x30);
		meseg->type = 0x55;
		meseg->buffer[0] = 0x01;
		//sout<<0x20<<((char)(client->ID)|0x30)<<0x55<<0x01<<endl;
		sendmeseg = (char*)meseg;
		if ( send(sock, sendmeseg.c_str(), sendmeseg.size(), 0) == -1 ) 
		{
			perror("send failed");
			exit(EXIT_FAILURE);
			return -1;
		}
		client->start();
		clientID++;
	}else
	{
		printf("accept a wrong device, ip is :%s \n", deviceip);
		
		sout<<0x20<<(meseg->source_id)<<0x55<<0x00<<endl;
		sendmeseg = sout.str();
		if ( send(i, sendmeseg.c_str(), sendmeseg.size(), 0) == -1 ) 
		{
			perror("send failed");
			exit(EXIT_FAILURE);
			return -1;
		}
		return 1;
	}
	
	return 0;

}



//函数：设置sock为non-blocking mode
void CTcpserv::setSockNonBlock(int sock) 
{
	int flags;
	flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) 
	{
		perror("fcntl(F_GETFL) failed");
		exit(EXIT_FAILURE);
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) 
	{
		perror("fcntl(F_SETFL) failed");
		exit(EXIT_FAILURE);
	}
}

//函数：更新maxfd
int CTcpserv::updateMaxfd(fd_set fds, int maxfd) 
{
	int i;
	int new_maxfd = 0;
	for (i = 0; i <= maxfd; i++) 
	{
		if (FD_ISSET(i, &fds) && i > new_maxfd) 
		{
			new_maxfd = i;
		}
	}
	return new_maxfd;
}
