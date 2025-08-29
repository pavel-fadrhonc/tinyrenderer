#pragma once

#include <windows.h>
#include <GL/gl.h>

namespace sor
{
	// Window and GL contexts
	inline HDC hDC; // Handle to device context (Windows GDI graphic context)
	inline HGLRC hRC; // Handle to OpenGL rendering context
	inline HWND hWnd; // Handle to window

	// WinAPI window procedure
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Setup OpenGL context
	void InitOpenGLContext(HWND hwnd);

	// Main rendering loop
	void RunLoop();

}
