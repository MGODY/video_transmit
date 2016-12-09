#ifndef STATE_H
#define STATE_H
#include "def.h"
#include "mesegtype"
#include <arpa/inet.h>
#include <string>
#include <QMutex>
using namespace std;


class CROBOT
{

public:
	CROBOT(int i = H264_PT_ID,int ht = pic_height,int wd = pic_width,string s_ip = Destip):
		ID(i),ServerIP(ip),on(0),bit_rate(bit_rates_define),height(ht),wide(wd){ }
	~CROBOT();
	int changeState(Mtype::StateRobot);
	Mtype::StateRobot getstate();
	const int ID;
	const string ServerIP;//服务器IP
	//int getID();
	//int getwide();
	//int getheight();
	//int 

private:
//	const int ID;
//	const string ServerIP;//服务器IP
	int on;
	int bit_rate;
	int height;
	int wide;
	QMutex mutex_state;
};






#endif
