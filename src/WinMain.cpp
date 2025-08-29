#include <Windows.h>

#include "triangle_drawing_test.h"
#include "window_output.h"

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int)
{
    // Register window class
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = sor::WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"SORENDER";
    RegisterClass(&wc);

    // also add console
	AllocConsole(); // create a new console
    freopen("CONOUT$", "w", stdout); // redirect stdout
    freopen("CONOUT$", "w", stderr); // redirect stderr

    // Create window
    sor::hWnd = CreateWindowW(wc.lpszClassName, L"Software renderer that goes places.", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, sor::IMAGE_SIZE_DEFAULT_X, sor::IMAGE_SIZE_DEFAULT_Y, 0, 0, hInst, 0);

    ShowWindow(sor::hWnd, SW_SHOW);

    sor::InitOpenGLContext(sor::hWnd);
    sor::PrepareForDrawModel();

    sor::RunLoop();

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
        if (wParam == 'A') { /* handle 'A' key */ }
        if (wParam == VK_ESCAPE) PostQuitMessage(0); // exit app
        break;
    case WM_MOUSEMOVE:
        // int mx = LOWORD(lParam);
        // int my = HIWORD(lParam);
        // use mx, my
        break;
    case WM_LBUTTONDOWN:
        // handle left click
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
