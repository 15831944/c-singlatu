#pragma once
#include <sys/socket.h>  
#include <string.h>  //strcpy


#define MAXLINE 50 //发送字节个数
#define CMD_WRITE_SN_I  22  //写sn的标志
#define CMD_READ_SN_I  21  //读sn的标志
#define CMD_DOUBLE_EYE  104  //双目摄像头测试
#define CMD_AGING 105  //老化测试

class CReply
{
    
protected:

    int m_iconnet_accept;//发送的对象
    char m_chAccept[MAXLINE];//接收的字符

    //virtual bool doSend()=0;  

};
