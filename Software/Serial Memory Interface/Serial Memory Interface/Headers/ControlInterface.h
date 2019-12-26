#pragma once

#include "ThreadBase.h"

#include <map>
#include <functional>

struct MEM_CHANGE_REQ
{
	uint64_t addr;
	uint64_t val;
};

class ControlInterface : public ThreadBase
{
public:
	ControlInterface();
	~ControlInterface();

	enum class COMMAND
	{
		START,
		STOP,
		STEP,
		READ_MEM_ADDR,
		WRITE_MEM_ADDR,
		SET_BREAKPOINT,
		CLEAR_BREAKPOINT
	};

	void setCommandHandler(COMMAND c, std::function<void(void*)> fn) { m_CommandHandlerFn[c] = fn; }
	void clearCommandHandler(COMMAND c) { m_CommandHandlerFn[c] = std::function<void(void*)>(); }

protected:
	virtual void processing();

private:
	std::map<COMMAND, std::function<void(void*)>> m_CommandHandlerFn;
};
