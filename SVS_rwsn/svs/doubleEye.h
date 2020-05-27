#pragma once
#include "Reply.h"
#include <stdio.h>


class CDoubleEye:public CReply
{
public:
	CDoubleEye(int connect);

	bool doSend();
private:

    static FILE* m_fstream;


};
