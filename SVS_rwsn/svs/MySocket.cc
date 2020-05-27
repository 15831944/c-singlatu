#include "MySocket.h"

int CMySocket::doSocket()
{
    int    socket_fd;  
    struct sockaddr_in     servaddr,clientAddr;  
    //初始化Socket  
    if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {  
        return 0;  
    }  
    //初始化  
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。  
    servaddr.sin_port = htons(6178);//设置的端口为DEFAULT_PORT  
  
    //将本地地址绑定到所创建的套接字上  
    if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {  
        return 0;  
    }  
    //开始监听是否有客户端连接  
    if( listen(socket_fd, 5) == -1)
    {  
        return 0;  
    }  
    

    unsigned int len = sizeof(clientAddr);
    while(1)
    {  
    //阻塞直到有客户端连接，不然多浪费CPU资源。  
        m_connect_accept = accept(socket_fd, (struct sockaddr*)&clientAddr,&len);
        if(m_connect_accept == -1)
            continue;
    //接受客户端传过来的数据 
        while(1)
        {
            if(!acceptDate())
                break;
        }
        close(m_connect_accept);
    }
    close(socket_fd);  
    return 0;
}

bool CMySocket::acceptDate()
{
    char    buff[MAXLINE];  
    int     n;  

    memset(buff,0,MAXLINE);
    n = recv(m_connect_accept, buff, MAXLINE, 0); 
    if(n==-1)//错误
        return true;
    if(n == 0)//断开链接
        return false;
    buff[n] = '\0'; 
    //如果是写操作                并且总长18位         数据16位，这样是正确的，要写到一个文件里
    if(buff[1] == CMD_WRITE_SN_I)
    {
        CSnWrite snWrite(m_connect_accept,buff);
        snWrite.doSend();
    }
    //如果是读操作        
    else if(buff[1] == CMD_READ_SN_I&&buff[0] == 1)
    {
        CSnRead snRead(m_connect_accept);
        snRead.doSend();  
    }
    //如果是老化测试        
    else if(buff[1] == CMD_AGING&&buff[0] == 1)
    {
        CAging aging(m_connect_accept);
        aging.doSend();  
    }
    //如果是双目摄像头测试        
    else if(buff[1] == CMD_DOUBLE_EYE&&buff[0] == 1)
    {
        CDoubleEye doubleEye(m_connect_accept);
        doubleEye.doSend();  
    }
    else
    {
        char sendData[MAXLINE];
        sendData[0]=2;
        sendData[1]=CMD_WRITE_SN_I;
        sendData[2]=0;
        send(m_connect_accept, sendData, MAXLINE,0); 
    }   
    return true;

}

