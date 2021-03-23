#include "block.h"

void block::update(int add_to_x, int add_to_y)
{
	x += add_to_x;
	y += add_to_y;
}

bool blockWindow::is_class_registered(HINSTANCE hInst, LPCWSTR cName)
{
	WNDCLASSEXW wcx;
	return GetClassInfoExW(hInst, cName, &wcx);
}

void blockWindow::register_class(HINSTANCE hInstance, LPCWSTR cName)
{
	WNDCLASSEX wcx;

	wcx.cbSize = sizeof(WNDCLASSEX);

	wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcx.lpfnWndProc = window_proc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hInstance;
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)(COLOR_ACTIVEBORDER);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = cName;
	wcx.hIconSm = NULL;

	RegisterClassEx(&wcx);
}

blockWindow::blockWindow(HINSTANCE hInst, HWND p_hWnd, block* b, const std::wstring& title) :
	m_hWnd{ nullptr }, blockStruct(b)
{
	LPCWSTR className = L"Block Window Class";


	if (!is_class_registered(hInst, className))
		register_class(hInst, className);

	CreateWindow(className, title.c_str(),
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		blockStruct->GetX(), blockStruct->GetY(), block::BLOCK_WIDTH, block::BLOCK_HEIGHT,
		p_hWnd, nullptr, hInst, reinterpret_cast <LPVOID>(this));

	//m_hWnd will be set on WM_NCCREATE
}

blockWindow::~blockWindow()
{
	if (m_hWnd)
		DestroyWindow(m_hWnd);
	if (blockStruct)
		delete blockStruct;
}

LRESULT blockWindow::window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	blockWindow* w = nullptr;
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg)
	{
	case WM_NCCREATE:
	{
		auto pcs = reinterpret_cast <LPCREATESTRUCTW>(lParam);
		w = reinterpret_cast <blockWindow*>(pcs->lpCreateParams);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast <LONG_PTR>(w));
		w->m_hWnd = hWnd;
		ShowWindow(hWnd, SW_SHOW);
		break;
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
	
		blockWindow* bw = (blockWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		char s[2];
		s[0] = bw->blockStruct->GetLetter();
		s[1] = '\0';

		RECT rc;
		GetClientRect(hWnd, &rc);

		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));
		DrawText(hdc, (LPCWSTR)s, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		//InvalidateRect(hWnd, NULL, TRUE);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		DestroyWindow(hWnd);
		return 0;
	}
	if (msg != WM_NCCREATE)
		w = reinterpret_cast <blockWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

	if (w) {
		auto r = w->window_proc(msg, wParam, lParam);
		if (msg == WM_NCDESTROY) {
			w->m_hWnd = nullptr;
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
		}
		return r;
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT blockWindow::window_proc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(m_hWnd);
		return 0;
	case WM_DESTROY:
		DestroyWindow(m_hWnd);
		return 0;
	}
	return DefWindowProcW(m_hWnd, msg, wParam, lParam);
}

blockWindow& blockWindow::operator=(blockWindow&& other) noexcept
{
	if (this != &other)
	{
		this->m_hWnd = other.m_hWnd;
	}
	return *this;
}
