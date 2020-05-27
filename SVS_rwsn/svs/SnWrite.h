#pragma once
#include "Reply.h"
#include <stdio.h>  //处理文件

class CSnWrite :public CReply
{
public:
    CSnWrite(int connect,char *accept);

    bool doSend();
  
}; 
