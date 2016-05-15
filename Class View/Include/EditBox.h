
#ifndef	_INCLUDED_EDITBOX_H
#define	_INCLUDED_EDITBOX_H


class EditBox
{
public:
	// Constructor/Destructor
	EditBox(HWND hWndParent, int x, int y, int w, int h);
	~EditBox(void);

	// Clear the text from the editbox
	void	Clear(void);

	// Print a formatted string to the editbox
	void	Printf(char *format, ...);

	// Set the current font
	void	SetFont(char *name);

private:
	// Callback for enumerating fonts
	static int CALLBACK	EnumFontsProc(const LOGFONT *lplf, const TEXTMETRIC *lptm, DWORD dwType, LPARAM lpData);

	// Handle to the window
	HWND	hWnd;

	// 64k buffer for the text
	char	m_Buffer[65535];

	// Cursor position within the buffer
	int		m_BufferPos;

	// List of enumerated fonts
	TArray<LOGFONT *>	m_LogicalFonts;

	// Handle to set font
	HFONT	hFont;
};


#endif	/* _INCLUDED_EDITBOX_H */