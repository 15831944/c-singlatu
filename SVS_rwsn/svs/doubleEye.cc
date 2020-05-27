#include "doubleEye.h"



FILE* CDoubleEye::m_fstream = NULL;

CDoubleEye::CDoubleEye(int connect)
{
	m_iconnet_accept = connect;
}


bool CDoubleEye::doSend()
{
	char cmd[MAXLINE] = "/home/root/start-camera.sh.1";
	char sendData[MAXLINE]={0};
	sendData[0]=2;
	sendData[1]=CMD_DOUBLE_EYE;
    if(m_fstream != NULL)//防止重复起动
    {
        sendData[2]=1;
	    send(m_iconnet_accept, sendData, 3,0);
        return true;
    }
	m_fstream = popen(cmd,"r");
	if(m_fstream==NULL)
	{
	    sendData[2]=0;
	    send(m_iconnet_accept, sendData, MAXLINE,0);
		pclose(m_fstream);
	    return true;
	}
	//int res = pclose(fstream);
        
    sendData[2]=1;
	send(m_iconnet_accept, sendData, 3,0);

	return true;
}


