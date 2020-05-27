#pragma once

#include <stdlib.h>  
#include <errno.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>
#include <unistd.h>
///////////////
#include "SnWrite.h"
#include "SnRead.h"
#include "aging.h"
#include "doubleEye.h"


/////////////////////////////////////////////////////////
class CMySocket
{
public:
    int doSocket();//main函数中执行
    
private:   
    int m_connect_accept;//发送的socket
 
    bool acceptDate();
};
