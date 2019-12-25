#ifndef __COMMANDER_H__
#define __COMMANDER_H__

#include "msgs.h"

typedef void (*MsgHandler)(void);

class Commander
{
	public:
	Commander();
	~Commander();
	
	void init();
	void background();
	
	private:
	MsgHandler m_MsgHandlerFnc[ALL_MSGS];
};

#endif