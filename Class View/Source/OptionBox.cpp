
#include "..\include\main.h"


OptionBox::OptionBox(int x, int y)
{
	// Create the display checkboxes
	hWndCheck[0] = CreateCheckBox(x + 10, y + 10, IDB_TABLE);
	hWndCheck[1] = CreateCheckBox(x + 10, y + 30, IDB_CLASS);
	hWndCheck[2] = CreateCheckBox(x + 10, y + 50, IDB_STATE);
	hWndCheck[3] = CreateCheckBox(x + 10, y + 70, IDB_FUNCTION);
	hWndCheck[4] = CreateCheckBox(x + 118, y + 10, IDB_IMPORT);
	hWndCheck[5] = CreateCheckBox(x + 118, y + 30, IDB_VARIABLE);
	hWndCheck[6] = CreateCheckBox(x + 118, y + 50, IDB_OBJECT);
	hWndCheck[7] = CreateCheckBox(x + 118, y + 70, IDB_INTERFACE);

	// Create the frame
	CreateWindowEx(0,
		"STATIC",
		NULL,
		WS_VISIBLE | WS_CHILD | SS_ETCHEDFRAME,
		x, y,
		168, 100,
		hWndMain,
		NULL,
		hInstMain,
		NULL);

	// Create the menu ruler
	CreateWindowEx(0,
		"STATIC",
		NULL,
		WS_VISIBLE | WS_CHILD | SS_ETCHEDFRAME,
		5, 0,
		582, 2,
		hWndMain,
		NULL,
		hInstMain,
		NULL);
}


HWND OptionBox::CreateCheckBox(int x, int y, int image)
{
	HWND	hWnd;
	HBITMAP	hBmp;

	// Create the checkbox window
	hWnd = CreateWindowEx(0,
		"BUTTON",
		NULL,
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_BITMAP | BS_AUTOCHECKBOX,
		x, y,
		36, 20,
		hWndMain,
		NULL,
		hInstMain,
		NULL);

	// Load the bitmap
	hBmp = LoadBitmap(hInstMain, MAKEINTRESOURCE(image));

	// Set the image
	SendMessage(hWnd, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmp);

	// Check the box by default
	SendMessage(hWnd, BM_SETCHECK, (WPARAM)TRUE, 0);

	return (hWnd);
}


int OptionBox::Update(HWND hWnd)
{
	#define	CHECK(x, y)	if (SendMessage(hWndCheck[x], BM_GETCHECK, 0, 0)) g_Display |= y;

	// Valid window?
	if (hWnd == hWndCheck[0] ||
		hWnd == hWndCheck[1] ||
		hWnd == hWndCheck[2] ||
		hWnd == hWndCheck[3] ||
		hWnd == hWndCheck[4] ||
		hWnd == hWndCheck[5] ||
		hWnd == hWndCheck[6] ||
		hWnd == hWndCheck[7])
	{
		// Nothing to display
		g_Display = 0;

		// Set the display parameters
		CHECK(0, DISPLAY_TABLE);
		CHECK(1, DISPLAY_CLASS);
		CHECK(2, DISPLAY_STATE);
		CHECK(3, DISPLAY_FUNCTION);
		CHECK(4, DISPLAY_IMPORT);
		CHECK(5, DISPLAY_VARIABLE);
		CHECK(6, DISPLAY_OBJECT);
		CHECK(7, DISPLAY_INTERFACE);

		return (1);
	}

	return (0);
}