#include "doubleEye.h"
#include<stdlib.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>


int CDoubleEye::m_isFirst = 0;

CDoubleEye::CDoubleEye(int connect)
{
	m_iconnet_accept = connect;
}


bool CDoubleEye::doSend()
{
	
    printf("%s\n",__TIME__);
	char sendData [MAXLINE]={0};
    if(m_isFirst == 0)//防止重复起动
    {
        //FILE* fp=NULL;
        //fp=fopen("/home/log.txt","w+");
		int fd,r;
		struct sockaddr_in addr={0};
		fd=socket(AF_INET,SOCK_STREAM,0);
		//2。链接到指定的地址
		addr.sin_family = AF_INET;
		addr.sin_port=htons(10700);
		addr.sin_addr.s_addr=inet_addr("192.168.1.110");
		r=connect(fd,(struct sockaddr*)&addr,sizeof(addr));
		int i=0;
        //30秒以内或链接成功才算成功
        for(;r!=0&&i<30;i++)
        {
		    r=connect(fd,(struct sockaddr*)&addr,sizeof(addr));            
            printf("r!:%d\n",r);
            sleep(1);
        }
            
        if(i==30)
        {
            this->sendFail();
            
        } 
        sleep(2); 
		//3.发送数据
        char tmp[64];
        memset(tmp, 0, 64);
        tmp[0] = 0xFF;
        strcpy(&tmp[1] , "stopbinocular");
        tmp[strlen(tmp)] = 0xFE;	
        for(i=0;i<3;i++)
        {
		    r=write(fd,tmp,strlen(tmp));
            usleep(100*1000);
        }
        
        if(r!=15)//发送的字符长度
        {
            this->sendFail();
            close(fd);
            return true;
        }  
        //sleep(2);
		//3.发送数据
	
        memset(tmp, 0, 64);
        tmp[0] = 0xFF;
        strcpy(&tmp[1] , "orginal");
        tmp[strlen(tmp)] = 0xFE;	
        for(i=0;i<1;i++)
        {
		    r=write(fd,tmp,strlen(tmp));  
            usleep(100*1000);
        }		
        if(r!=9)//
        {
            this->sendFail();
            close(fd);
            return true;
        }  
		close(fd); 
        sleep(2);
    
		sendData[0]=2;
		sendData[1]=CMD_DOUBLE_EYE;
		
		sendData[2]=1;
		send(m_iconnet_accept, sendData, 3,0);
		m_isFirst = 1;
		return true;
    }

    sendData[0]=2;
    sendData[1]=CMD_DOUBLE_EYE;
    
	sendData[2]=1;
	send(m_iconnet_accept, sendData, 3,0);
	return true;
}

void CDoubleEye::sendFail()
{
    char sendData[5]={0};

    sendData[0]=2;
    sendData[0]=CMD_DOUBLE_EYE;
    sendData[0]=0;
    send(m_iconnet_accept,sendData,5,0);
    return;
}
