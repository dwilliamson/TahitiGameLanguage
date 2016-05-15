
#include "..\include\main.h"


#define APP_TITLE	"ClassView"


// Main application handles
HINSTANCE	hInstMain;
HWND		hWndMain;

// Application treeview
TreeView	*g_Tree;

// Application editbox
EditBox		*g_Edit;

// Application optionbox
OptionBox	*g_Option;

// List of units loaded in
TArray<Unit *>	g_UnitList;

// What to display
int			g_Display;


void LoadUnit(void);


long WINAPI MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case (WM_CREATE):
			break;

		case (WM_COMMAND):

			// Check for program exit
			if (LOWORD(wParam) == ID_FILE_EXIT)
				SendMessage(hWnd, WM_CLOSE, 0, 0);

			// Check for adding a new unit
			if (LOWORD(wParam) == ID_FILE_ADDUNIT)
				LoadUnit();

			// Button has been clicked?
			if (HIWORD(wParam) == BN_CLICKED)
			{
				// Check to see if the display parameters need updating
				if (g_Option->Update((HWND)lParam))
				{
					int		x;

					// Update each units treeview
					for (x = 0; x < g_UnitList.GetPosition(); x++)
					{
						g_UnitList(x)->RemoveFromTreeView();
						g_Tree->ResetPosition();
						g_UnitList(x)->AddToTreeView();
					}
				}
			}

			break;

		case (WM_NOTIFY):

			// Has a treeview selection changed?
			if ( ((NMHDR *)lParam)->code == TVN_SELCHANGED)
			{
				// Clear the editbox
				g_Edit->Clear();

				((TreeViewEntry *) ((LPNMTREEVIEW)lParam)->itemNew.lParam ) -> PrintInfo();
			}

			break;

		case (WM_CLOSE):
			DestroyWindow(hWnd);
			break;

		case (WM_DESTROY):
			PostQuitMessage(0);
			break;

		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}

	return (0L);
}


BOOL CreateApplicationWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASS	wc;

	// Setup the window class
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hbrBackground	= (HBRUSH)COLOR_WINDOW;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hInstance		= hInstance;
	wc.lpfnWndProc		= MainWndProc;
	wc.lpszClassName	= APP_TITLE;
	wc.lpszMenuName		= MAKEINTRESOURCE(IDR_MENU1);
	wc.style			= CS_HREDRAW | CS_VREDRAW;

	// Try and register the class with windows
	if (!RegisterClass(&wc))
		return (FALSE);

	// Create the window
	hWndMain = CreateWindowEx(0,
		APP_TITLE,
		APP_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		600,
		400,
		NULL,
		NULL,
		hInstMain = hInstance,
		NULL);

	// Check for errors
	if (hWndMain == NULL)
		return (FALSE);

	// Show the window
	UpdateWindow(hWndMain);
	ShowWindow(hWndMain, nCmdShow);

	return (TRUE);
}


BOOL PollMessageQueue(void)
{
	MSG		msg;

	// Loop until all messages are out
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		// Get the message and check for WM_QUIT
		if (!GetMessageA(&msg, NULL, 0, 0))
			return (FALSE);

		// Translate any keycodes and send to message handler
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (TRUE);
}


void NewUnit(char *filename)
{
	Unit	*unit;

	// Process the new unit
	unit = new Unit(filename);

	// Add it to the tree view
	g_Tree->ResetPosition();
	unit->AddToTreeView();

	// Add it to the global list
	g_UnitList.Add(unit);
}


void DeleteUnit(Unit *unit)
{
	// Remove and delete
	delete unit;
}


void DeleteAllUnits(void)
{
	int		x;

	// Just delete everything
	for (x = 0; x < g_UnitList.GetPosition(); x++)
		delete g_UnitList(x);
}


void LoadUnit(void)
{
	OPENFILENAME	ofn;
	char			szFile[260] = { 0 };

	// Initialise file structure
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWndMain;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = 260;
	ofn.lpstrFilter = "VM Units\0*.VMU\0All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the open dialogue box to get the file
	if (GetOpenFileName(&ofn) == FALSE)
		return;

	// Make a new unit!
	NewUnit(szFile);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	// Ensure the treeview class is loaded
	InitCommonControls();

	// Create the main window
	if (CreateApplicationWindow(hInstance, nCmdShow) == FALSE)
		return (0);

	// Create the treeview
	g_Tree = new TreeView(hWndMain);

	// Create the information editbox
	g_Edit = new EditBox(hWndMain, 400, 10, 185, 220);
	g_Edit->SetFont("Tahoma");

	// Create the display options box
	g_Option = new OptionBox(410, 240);

	// Initial display settings
	g_Display = DISPLAY_ALL;

	// LOOP!
	while (PollMessageQueue() == TRUE)
	{
	}

	// Delete stuff
	delete g_Edit;
	delete g_Tree;

	DeleteAllUnits();

	return (1);
}