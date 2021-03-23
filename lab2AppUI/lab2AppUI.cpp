// lab2AppUI.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "lab2AppUI.h"
#include "block.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
static std::list <blockWindow*> blockWindowsList;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDC_LAB2APPUI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2APPUI));

	MSG msg;


	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


COLORREF crLabelTextColour;
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2APPUI));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_INACTIVECAPTION);
	crLabelTextColour = COLOR_INACTIVECAPTION;
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB2APPUI);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}


int missedKeys = 0;
int wrongKeys = 0;

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	RECT rc;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
	int centerX = (rc.left + rc.right + 1) / 2;
	int centerY = (rc.top + rc.bottom + 1) / 2;

	TCHAR s[256];
	_stprintf_s(s, 256, _T("KeyboardMaster_WinAPI2021. Missed : %d    Wrong: %d"), missedKeys, wrongKeys);
	HWND hWnd = CreateWindowEx(WS_EX_TOPMOST, szWindowClass, s, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU ,
		CW_USEDEFAULT, 0, (rc.right - rc.left) / 2, (rc.bottom - rc.top) / 2, nullptr, nullptr, hInstance, nullptr);


	// current size of the window
	GetWindowRect(hWnd, &rc);
	int currentSizeX = rc.right - rc.left;
	int currentSizeY = rc.bottom - rc.top;

	MoveWindow(hWnd, centerX - currentSizeX / 2, centerY - currentSizeY / 2, currentSizeX, currentSizeY, TRUE);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

int refreshRate = 30;
const int MAX_BLOCKS = 100;
bool pause = false;
bool focus = false;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_RBUTTONDOWN:
	{
		POINT cursor;
		GetCursorPos(&cursor);
		HMENU hPopupMenu = CreatePopupMenu();
		InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_COLOR, L"Color");
		InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_COLOR_DEFAULT, L"Default");
		TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, hWnd, NULL);
	}
	break;
	case WM_ACTIVATE:
		/*SendMessage(hWnd, WM_COMMAND, IDM_PAUSE, NULL);*/
	{
		if (focus)
		{
			focus = false;
			KillTimer(hWnd, 7);
			KillTimer(hWnd, 9);
		}
		else if (!focus)
		{
			focus = true;
			if (pause == false)
			{
				SetTimer(hWnd, 7, 1000, NULL);
				SetTimer(hWnd, 9, 100, NULL);
			}
		}
	}
		break;
	case WM_CREATE:
	{
		srand(time(NULL));
		int time = rand() % (1000 - 300) + 300;
		SetTimer(hWnd, 7, time, NULL);
		SetTimer(hWnd, 9, refreshRate, NULL);
	}
	break;
	case WM_TIMER:
	{
		if (wParam == 7) // create new window
		{
			int time = rand() % (1000 - 300) + 300;
			if (blockWindowsList.size() > MAX_BLOCKS)
			{
				SetTimer(hWnd, 7, time, NULL);
				break;
			}

			RECT rc;
			GetWindowRect(hWnd, &rc);
			int currentSizeX = rc.right - rc.left;
			int x = rand() % (currentSizeX - 2 * block::BLOCK_WIDTH);
			int y = 0;
			double velocity = (double)rand() / RAND_MAX * 3 + 1;
			char letter = rand() % ('z' - 'a' + 1) + 'a';

			block* new_block = new block(x, y, velocity, letter);
			blockWindow* new_block_window = new blockWindow(hInst, hWnd, new_block, L"Block Window");

			blockWindowsList.push_back(new_block_window);

			SetTimer(hWnd, 7, time, NULL);
			SetTimer(hWnd, 9, refreshRate, NULL);
		}
		if (wParam == 9) // move windows
		{

			RECT rc;
			GetWindowRect(hWnd, &rc);
			int currentSizeY = rc.bottom - rc.top;
			int currentSizeX = rc.right - rc.left;
			std::list<std::list<blockWindow*>::iterator> l;
			for (auto it = blockWindowsList.begin(); it != blockWindowsList.end(); ++it)
			{
				block* currBlock = (*it)->blockStruct;
				currBlock->update(0, currBlock->GetVelocity());


				if (currBlock->GetY() > (currentSizeY - block::BLOCK_HEIGHT))
				{
					DestroyWindow((*it)->getHWND());
					l.push_back(it);
					missedKeys++;
					TCHAR s[256];
					_stprintf_s(s, 256, _T("KeyboardMaster_WinAPI2021. Missed : %d    Wrong: %d"), missedKeys, wrongKeys);
					SetWindowText(hWnd, s);
					continue;
				}

				MoveWindow((*it)->getHWND(), currBlock->GetX(), currBlock->GetY(),
					block::BLOCK_WIDTH, block::BLOCK_HEIGHT, TRUE);

				InvalidateRect((*it)->getHWND(), NULL, TRUE);
			}



			// delete blocks, which are outside of the main window
			for (auto it = l.begin(); it != l.end(); ++it)
			{
				blockWindow* deletedWindow = *(*it);
				HWND h = deletedWindow->getHWND();
				DestroyWindow(deletedWindow->getHWND());
				blockWindowsList.erase(*it);
				delete deletedWindow;


				SetTimer(hWnd, 9, refreshRate, NULL);
			}
		}
		break;
	case WM_PAINT:
	{

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		RECT rc;
		GetClientRect(hWnd, &rc);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_MENU:
		{
			RECT rc;
			GetWindowRect(hWnd, &rc);
			HMENU pSysMenu = (HMENU)GetSystemMenu(hWnd, FALSE);
			if (pSysMenu != NULL)
			{
				int flag = TrackPopupMenu(pSysMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, rc.left + 50, rc.top, NULL, hWnd, 0);
				if (flag > 0)
					SendMessage((HWND)hWnd, WM_SYSCOMMAND, flag, 0);
			}
		}
		break;
		case IDM_NEW:
		{
			for (auto it = blockWindowsList.begin(); it != blockWindowsList.end(); ++it)
			{
				blockWindow* deletedWindow = (*it);
				delete deletedWindow;
			}
			blockWindowsList.clear();
			SetTimer(hWnd, 7, 1000, NULL);
		}
		break;
		case IDM_PAUSE:
		{
			if (!focus)
				break;

			if (!pause)
			{
				pause = true;
				KillTimer(hWnd, 7);
				KillTimer(hWnd, 9);
			}
			else if (pause)
			{
				pause = false;
				SetTimer(hWnd, 7, 1000, NULL);
				SetTimer(hWnd, 9, 100, NULL);
			}
		}
		break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_COLOR:
		{
			static COLORREF acrCustClr[16]; // array of custom colors
			CHOOSECOLOR ccColour = { 0 };
			ccColour.lStructSize = sizeof(ccColour);
			ccColour.hwndOwner = hWnd;
			ccColour.hInstance = hWnd;
			ccColour.lpCustColors = (LPDWORD)acrCustClr;
			ccColour.rgbResult = crLabelTextColour;
			ccColour.Flags = CC_FULLOPEN | CC_RGBINIT;
			if (ChooseColor(&ccColour) == TRUE)
			{
				crLabelTextColour = ccColour.rgbResult;
			}

			HBRUSH brush = CreateSolidBrush(crLabelTextColour);
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		break;
		case ID_COLOR_DEFAULT:
		{
			HBRUSH brush = (HBRUSH)(COLOR_INACTIVECAPTION);
			crLabelTextColour = (COLORREF)(COLOR_INACTIVECAPTION);
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_CHAR:
	{
		char letter = wParam;
		if (pause == true)
			break;

		blockWindow* deletedWindow = nullptr;
		std::list<blockWindow*>::iterator deletedIt;
		bool found = false;

		for (auto it = blockWindowsList.begin(); it != blockWindowsList.end(); ++it)
		{
			if ((*it)->blockStruct->GetLetter() == letter)
			{
				if (deletedWindow == nullptr)
				{
					deletedWindow = *it;
					deletedIt = it;
					found = true;
				}
				else if ((*it)->blockStruct->GetY() > deletedWindow->blockStruct->GetY())
				{
					deletedWindow = *it;
					deletedIt = it;
				}
			}
		}

		if (found)
		{
			DestroyWindow(deletedWindow->getHWND());
			blockWindowsList.erase(deletedIt);
		}
		else
		{
			wrongKeys++;
			TCHAR s[256];
			_stprintf_s(s, 256, _T("KeyboardMaster_WinAPI2021. Missed : %d    Wrong: %d"), missedKeys, wrongKeys);
			SetWindowText(hWnd, s);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
	}
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}