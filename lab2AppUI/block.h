#pragma once

#include "framework.h"
#include <utility>
#include <string>


class block
{
	int x;
	int y;
	double velocity;
	char letter;

public:
	static const int BLOCK_HEIGHT = 25;
	static const int BLOCK_WIDTH = 25;
	int GetX() { return x; }
	int GetY() { return y; }
	int GetVelocity() { return velocity; }
	char GetLetter() { return letter; }
	void update(int add_to_x, int add_to_y);

	block(int x, int y, double velocity, char letter) : x(x), y(y), velocity(velocity), letter(letter) {}
};

class blockWindow
{
	// class frame taken from the lecture

	static bool is_class_registered(HINSTANCE, LPCWSTR);
	static void register_class(HINSTANCE, LPCWSTR);
	HWND m_hWnd;
public:
	block* blockStruct;
	static LRESULT window_proc(HWND, UINT, WPARAM, LPARAM);
	virtual LRESULT window_proc(UINT, WPARAM, LPARAM);

	blockWindow() : m_hWnd( nullptr ), blockStruct (nullptr) { }
	blockWindow(const blockWindow&) = delete;
	blockWindow(blockWindow&& other) noexcept : m_hWnd(nullptr) { *this = std::move(other); }
	blockWindow(HINSTANCE, HWND, block*, const std::wstring&);
	blockWindow& operator=(const blockWindow&) = delete;
	blockWindow& operator=(blockWindow&& other) noexcept;
	HWND getHWND() const { return m_hWnd; }

	virtual ~blockWindow();
};


