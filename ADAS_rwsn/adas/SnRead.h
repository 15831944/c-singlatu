#pragma once
#include "Reply.h"
#include <stdio.h>

class CSnRead :public CReply
{
public:
    CSnRead(int connect);

    bool doSend();
  
}; 

