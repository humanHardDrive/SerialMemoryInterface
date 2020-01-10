#define WIN32_LEAN_AND_MEAN

#include "SerialMemoryFrame.h"
#include "SerialMemoryApp.h"

SerialMemoryFrame::SerialMemoryFrame(const wxString & title) :
	MemoryView(NULL, wxID_ANY, title)
{
	for (int i = 0; i < m_GridMemory->GetNumberCols(); i++)
	{
		wxString str = "0x0";
		str += decToHex(i);
		m_GridMemory->SetColLabelValue(i, str);
		m_GridMemory->AutoSizeColLabelSize(i);
	}

	for (int i = 0; i < m_GridMemory->GetNumberRows(); i++)
	{
		wxString str = "0x";
		int tempRow = i;

		do
		{
			str += decToHex(tempRow);
			tempRow /= 16;
		} while (tempRow > 0);
		str += '0';

		m_GridMemory->SetRowLabelValue(i, str);
		m_GridMemory->AutoSizeRowLabelSize(i);
	}
}

void SerialMemoryFrame::OnBtnLoadFile(wxCommandEvent & event)
{
	wxFileDialog fileDlg(this, _("Open File"), "", "", "HEX files (*.hex)|*.hex|S Record (*.S)|.*S", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (fileDlg.ShowModal() == wxID_CANCEL)
		return;
}

void SerialMemoryFrame::OnWordSizeLoseFocus(wxFocusEvent & event)
{
}

char SerialMemoryFrame::decToHex(uint8_t n)
{
	n = n & 0x0F;

	if (n < 10)
		return (n + '0');
	else
		return (n - 10 + 'A');
}
