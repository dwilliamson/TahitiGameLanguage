
#ifndef	_INCLUDED_OPTIONBOX_H
#define	_INCLUDED_OPTIONBOX_H


class OptionBox
{
public:
	// Constructor
	OptionBox(int x, int y);

	// Check to see if the display parameters need updating
	int		Update(HWND hWnd);

private:
	// Create a check box
	HWND	CreateCheckBox(int x, int y, int image);

	HWND	hWndCheck[8];
};


#endif	/* _INCLUDED_OPTIONBOX_H */