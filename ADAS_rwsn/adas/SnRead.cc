#include "SnRead.h"


CSnRead::CSnRead(int connect)
{
    m_iconnet_accept = connect;
}

bool CSnRead::doSend()
{
    char cmd[MAXLINE] = "fw_printenv sn";
    char sendData[MAXLINE]={0};
    FILE* fstream = NULL;
    fstream = popen(cmd,"r");
    if(fstream == NULL)
    {
        sendData[0]=2;
        sendData[1]=CMD_READ_SN_I;
        sendData[2]=0;
        send(m_iconnet_accept, sendData, MAXLINE,0);
        return true;
    }
    while(NULL!=fgets(m_chAccept, sizeof(m_chAccept), fstream))      
    {      
        //printf("res:%s\n",m_chAccept);     
    }      

    fclose(fstream);
    

    int len = strlen(&m_chAccept[3]);
    sendData[0]=len+3;
    sendData[1]=CMD_READ_SN_I;
    sendData[2]=1;
    sendData[3]=len;
    memcpy(&sendData[4],&m_chAccept[3],sizeof(sendData)-5);
    //printf("%s",sendData);
    send(m_iconnet_accept, sendData, len+4,0);  
    
    return true;
}
