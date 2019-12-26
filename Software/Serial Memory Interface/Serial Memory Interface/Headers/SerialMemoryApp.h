#pragma once

#include <wx/wx.h>
#include <memory>

#include "SerialInterface.h"

class SerialMemoryApp : public wxApp
{
public:
	virtual bool OnInit();

private:
	std::shared_ptr<SerialInterface> m_SerialInterface;
};

DECLARE_APP(SerialMemoryApp)