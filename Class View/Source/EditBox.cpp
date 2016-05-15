
#include "..\include\main.h"


EditBox::EditBox(HWND hWndParent, int x, int y, int w, int h)
{
	// Clear the editbox
	ZeroMemory(m_Buffer, 65535);
	m_BufferPos = 0;
	hFont = NULL;

	// Create the window
	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,
		"EDIT",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
		x, y, w, h,
		hWndParent,
		(HMENU)0x300,
		hInstMain,
		NULL);

	// Set the editbox text
	SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)m_Buffer);
}


EditBox::~EditBox(void)
{
	int		x;

	// Delete the allocated fonts
	for (x = 0; x < m_LogicalFonts.GetPosition(); x++)
		delete (void *)(m_LogicalFonts(x));

	// Release the font
	if (hFont)
		DeleteObject(hFont);
}


void EditBox::Clear(void)
{
	// Clear the buffer
	ZeroMemory(m_Buffer, 65535);
	m_BufferPos = 0;

	// Set the editbox text
	SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)m_Buffer);
}


void EditBox::Printf(char *format, ...)
{
	va_list	valist;
	char	string[256], final[256];
	int		x, y = 0;

	// Format the string
	va_start(valist, format);
	vsprintf(string, format, valist);
	va_end(valist);

	// Replace all carriage returns
	for (x = 0; string[x]; x++)
	{
		if (string[x] == '\n')
		{
			final[y] = '\r';
			final[y + 1] = '\n';
			y += 2;
		}
		else
		{
			final[y] = string[x];
			y++;
		}
	}
	final[y] = 0;

	// Test for a buffer overflow
	if (m_BufferPos + strlen(string) + 1 > 65535)
	{
		// Clear it and tell the user
		Clear();
		Printf("*** BUFFER RESET ***\n");
		Printf(final);
	}
	else
	{
		// Just copy the string
		strcpy(&m_Buffer[m_BufferPos], final);
		m_BufferPos += strlen(final);

		// Set the editbox text
		SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)m_Buffer);
	}
}


int CALLBACK EditBox::EnumFontsProc(const LOGFONT *lplf, const TEXTMETRIC *lptm, DWORD dwType, LPARAM lpData)
{
	LOGFONT	*logfont;

	// Allocate the font holder
	logfont = new LOGFONT;

	// Copy the memory over
	memcpy(logfont, lplf, sizeof(LOGFONT));

	// Add the logical font pointer
	((EditBox *)lpData)->m_LogicalFonts.Add(logfont);

	return (TRUE);
}


void EditBox::SetFont(char *name)
{
	int		x;

	// Get the editbox device context
	HDC hdc = GetDC(hWnd);

	// Enumerate all the fonts
	EnumFonts(hdc, NULL, EnumFontsProc, (LPARAM)this);

	// For every enumerated font
	for (x = 0; x < m_LogicalFonts.GetPosition(); x++)
	{
		// Delete the font if it's allocated
		if (hFont)
			DeleteObject(hFont);

		// Does the face name match?
		if (!strcmp(name, (char *)m_LogicalFonts(x)->lfFaceName))
		{
			HFONT	hfont;

			m_LogicalFonts(x)->lfWidth = 0;
			m_LogicalFonts(x)->lfHeight = 12;
			m_LogicalFonts(x)->lfWeight = FW_THIN;

			// Create the font
			hfont = CreateFontIndirect(m_LogicalFonts(x));

			// Change it
			SendMessage(hWnd, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(1, 0));
		}
	}

	// Release the DC
	ReleaseDC(hWnd, hdc);
}