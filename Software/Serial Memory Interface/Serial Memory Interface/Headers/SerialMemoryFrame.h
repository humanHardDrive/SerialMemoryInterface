#pragma once

#include <wx/wx.h>
#include <wx/grid.h>

#include <string>

#include "MemoryViewBase.h"

class SerialMemoryFrame : public MemoryView
{
public:
	SerialMemoryFrame(const wxString& title);

protected:
	void OnBtnLoadFile(wxCommandEvent& event);
	void OnWordSizeLoseFocus(wxFocusEvent& event);
};