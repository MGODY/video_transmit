
#ifndef TCPSERVER_H
#define TCPSERVER_H
/*---------------------------------------------------------------------------------------------
//	版权所有： ISST
//	功能： TCP多线程服务器
//	@author 冯坤 ， any question email me ： mgody.fengkun@gmail.com
//	@finish time ： 
//  版本号： v1.2
------------------------------------------------------------------------------------------------*/
#pragma GCC diagnostic error "-std=c++11"  //添加c++11 支持

#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>

#include <iostream>
#include <sstream>
#include <list>
#include <string>
#include <vector>

#include <QThread>
#include <QtGui>
#include <QMutex>
#include <QString>
#include <QTimer>
#include <algorithm>
#include <def.h>
#include <mesegtype.h>
#include <iterator>
#define DEFAULT_PORT    4417    //默认端口
#define BUFF_SIZE       1024    //buffer大小
#define SELECT_TIMEOUT  5       //select的timeout seconds
class Client;
class Camera;

//const unsigned int MESEG_BUFF_SIZE = 100;
using namespace std;
//对于每个机器人端都设定一个标志其状态以及用于连接的类对象。
class Camera : public QThread
{
	Q_OBJECT
public://构造与析构函数
	Camera(int sock,int id_d,int h,int w,int ratec,int on_d,int connect_d,int error_d, char *ip):
		socket(sock),ID(id_d),on(on_d),connect_i(connect_d),error(error_d),height(h),wide(w),
		rate_control(ratec),ip_address(ip){	cout<<"a new robot device created"<<endl;run_flag = true ;}
	
	//方便起见、定义委托构造函数。
	Camera(int sock,int id_d,int h,int w, int ratec,char *ip):
		Camera(sock,id_d,h,w,ratec,0xAA,0x55,0xAA,ip){}
	Camera(int sock,int id_d,char *ip):
		Camera(sock,id_d,0,0,0,0xAA,0x55,0xAA,ip){}

	~Camera();


protected:
	void run();
public:
	int ison();
	int  isconnect();
	Mtype::StateRobot getstate();
	//由于其他对象要是访问类的成员变量的话的话很麻烦，尤其是在不同线程的时候，多个线程如果同访问对象中的变量很容易出问题，所以干脆直接设置一个
	//函数，使用函数返回对象所需要的类的成员变量，使用锁保障这个函数的线程安全就OK了，这样才是正确的处理方式。
	
private://状态信息
	int socket;	
	int ID;//ID是事先固定好的，我们希望知道是哪个机器人传来的视频数据。
	int on;//开关状态.
	int connect_i;//判断连接状态。0x55代表连接正常. 0xAA代表断开连接。
	int error ; //故障状态。暂定0x55代表有故障。0xAA代表正常工作。（后期可以加上别的定义）
	int height;//分辨率高度.
	int wide;//分辨率宽度.
	int rate_control;//码率控制.
	string ip_address;//IP地址。
private://运行信息以及函数。
	bool run_flag;
	QMutex mutex_socket; //防止两个线程中同时收发socket的时候会起冲突。
	QMutex mutex_robot;//设置一个锁用来在getstate函数中锁住数据访问，一个目的是希望能保证函数的线程安全
//另一个目的是为了保证函数访问与线程中数据访问不会冲突。

	int sendmeseg(const Mtype::SMeseg *const );
	int update(const Mtype::StateRobot *const);//note：使用const的意义并不是为了说真的怕自己的程序误改了对象的指针或者值，
												//应该说一定程度上是为了读程序方便。表示这段程序只是需要对象的值，并不需要改变对象。
												//而使用指针不使用引用的话，可以将对象继续传递下去。
												//毕竟引用只能传递一次。（引用必须绑定在对象上）
	int deal_rmesegs(const Mtype::SMeseg *const,Mtype::StateRobot *);
	int dealmesegin(const Mtype::SMeseg *const ,Mtype::StateRobot *); 
//Mtype::SMeseg connect;
//	int update;//更新状态。1代表已更新，0代表待更新。
//	list<int> client_list;
signals:
	void toclient(const QString);

private slots:
	//note：QT的信号槽机制带给使用者许多的便利，但是使用在多线程中的时候尤其要注意槽函数执行在哪个线程，需不需要加锁保证数据的线程安全。
	//多线程中使用信号槽的时候。根据信号发送者在哪个线程，以及信号连接方式的不同，槽函数执行的线程都是不同的。
	//如果是直连方式，槽函数会直接在信号发送者所在的线程执行，如果是队列连接，那么槽函数在槽函数所依附的线程执行。
	//而对于QThread对象来说，run()函数是线程的入口。也就是说QThread的槽函数并不是在QThread中执行的，而是在创建QThread对象的
	//的线程中执行的，所以QThread的槽函数中如果要访问或者更改QThread对象的成员变量必须加线程锁。
	//void update();//用于保持连接。
	void dealmeseg(const QString);
};

class CTcpserv : public QThread
{
	Q_OBJECT
public:
	static QMutex  mutex_robots;
	static QMutex  mutex_clients;
	static vector<Camera*> robot_device;
	static vector<Client*> client_device;
	//static list<order_client2ser> camera_messeg;//用来保存未执行的命令。

	CTcpserv ();
	~CTcpserv();
protected:
	void run();
	
private:
	int clientID;
	
	int creatNewDevice(int ,Mtype::SMeseg *,char *);
	char buffer[BUFF_SIZE];
	void update();
private://用于TCP连接的函数
	//int HandleMesseg(int ,char*,int);//socket buffer size
	//int send_client; // 0 不发送 1 发送  
	//int send_camera; //
	int updateMaxfd(fd_set , int);
	void setSockNonBlock(int);
	int send_robot_states(vector<Client*>::iterator *);
private://用于TCP连接的变量
	sockaddr_in bind_addr;
	timeval timeout;
	sockaddr_in client_addr;

	unsigned short int port;
	int sock;
	int yes;

	//创建并初始化select需要的参数(这里仅监视read)，并把sock添加到fd_set中
	fd_set readfds;
	fd_set readfds_bak; //backup for readfds(由于每次select之后会更新readfds，因此需要backup)
	int maxfd;

	//循环接受client请求所需变量
	int new_sock;
	socklen_t client_addr_len;
	char client_ip_str[INET_ADDRSTRLEN];///****
	int res;
	int i;
	int recv_size;
};

class Client : public QThread
{
	Q_OBJECT
public:
	friend class CTcpserv;
	Client(int sock,int id_d,int on_d,int connect_d,char *ip,int flag):
		flag_send_rstates(flag),socket(sock),ID(id_d),on(on_d),
		connect_i(connect_d),
		ip_address(ip)
		{ cout<<"a new client device created"<<endl;run_flag = true ;}
	
	Client(int sock,int id_d,int on_d,char *ip):
		Client(sock,id_d,on_d,0x55,ip,0){}
	Client(int sock,int id_d,char *ip):
		Client(sock,id_d,0xAA,0x55,ip,0){}
	
	~Client();

	
protected:
	void run();
public:
	int flag_send_rstates;
	Mtype::StateClient getstate();
	int ison();
	int isconnect();
	string get_robotlist();
	//bool issend_rstates();
	int  send_rstates(const Mtype::SMeseg*const);
private://function

	bool run_flag;
	int deal_cmesegs(const Mtype::SMeseg *const ,Mtype::StateClient *);
	
	int dealmesegin(const Mtype::SMeseg *const ,Mtype::StateClient *);
	int sendmeseg(const Mtype::SMeseg *const );
	int update(const Mtype::StateClient *const);//note：使用const的意义并不是为了说真的怕自己的程序误改了对象的指针或者值，
	int change_getlist(const Mtype::SMeseg *const ,Mtype::StateClient *);
	
private:
//	Mtype::SMeseg meseg;
	int socket;
	int ID;//临时添加的ID 
	int on;//决定是否接受
	int connect_i;//连接状态判定。
	int error;
	string ip_address;
	QMutex flag_lock;
	
	
	vector<int> Getlist;

	
	QMutex mutex_socket; //防止两个线程中同时收发socket的时候会起冲突。
	QMutex mutex_client;//因为状态变量并不会频繁的被更改和访问，所以这里对所有的camera设一个锁。
	
	//list<order_client2ser> cam_order_list;
signals:
	void to_robot(const QString);
private slots:
	//void update();//用于保持连接
	 void dealmeseg(const QString);
};







#endif
