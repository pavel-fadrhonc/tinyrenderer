#include "window_output.h"

#include "triangle_drawing_test.h"
#include "input.h"

namespace sor
{
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//auto processKeyInput = [wParam]()

		switch (msg)
		{
		case WM_KEYDOWN:
			if (wParam == 'S') { g_DeviceInput.WriteKeyInput(EKeyCodeFlags::S, EInputType::DOWN); }
			else if (wParam == 'F') {g_DeviceInput.WriteKeyInput(EKeyCodeFlags::F, EInputType::DOWN); }
			if (wParam == VK_ESCAPE) PostQuitMessage(0); // exit app
			break;
		case WM_MOUSEMOVE:
		{
			int mx = LOWORD(lParam);
			int my = HIWORD(lParam);
			g_DeviceInput.mouseDelta.x += mx;
			g_DeviceInput.mouseDelta.y += my;
		}
			break;
		case WM_LBUTTONDOWN:
			g_DeviceInput.WriteMouseInput(EMouseInputFlags::LEFT_BUTTON, EInputType::DOWN);
			break;
		case WM_RBUTTONDOWN:
			g_DeviceInput.WriteMouseInput(EMouseInputFlags::RIGHT_BUTTON, EInputType::DOWN);
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	void InitOpenGLContext(HWND hwnd)
	{
		PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1,
		  PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
		  24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
		};
		hDC = GetDC(hwnd);
		int pf = ChoosePixelFormat(hDC, &pfd);
		SetPixelFormat(hDC, pf, &pfd);
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
	}

	void RunLoop()
	{
		GLuint tex = 0;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // No filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Allocate blank texture (don't supply data yet)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		// unsigned char* pixels = new unsigned char[IMAGE_SIZE_DEFAULT_X * IMAGE_SIZE_DEFAULT_Y * 3];

		MSG msg = {};
		float lastFrameTime = GetTimeSinceStartupSeconds();
		while (true)
		{
			// Pump messages
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT) goto done;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			// Update your pixel buffer
			//fill_pixels(pixels, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y);


			RotateModel();

			g_DrawContext.screenTexture.Clear();
			DrawModel(&g_DrawContext);

			g_DeviceInput.Clear();

			// Upload pixel data to texture using glTexSubImage2D
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, GL_BGR_EXT, GL_UNSIGNED_BYTE, g_DrawContext.screenTexture.GetBuffer());

			// Render textured quad
			glViewport(0, 0, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0, IMAGE_SIZE_DEFAULT_X, 0, IMAGE_SIZE_DEFAULT_Y, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2f(0, 0);
			glTexCoord2f(1, 0); glVertex2f(IMAGE_SIZE_DEFAULT_X, 0);
			glTexCoord2f(1, 1); glVertex2f(IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y);
			glTexCoord2f(0, 1); glVertex2f(0, IMAGE_SIZE_DEFAULT_Y);
			glEnd();
			glDisable(GL_TEXTURE_2D);

			SwapBuffers(hDC);


			float currentTime = GetTimeSinceStartupSeconds();
			if (currentTime < lastFrameTime + FRAME_DURATION)
			{
				Sleep((FRAME_DURATION - (currentTime - lastFrameTime)) * 1000.f);
			}

			float deltaTime = currentTime - lastFrameTime;
			g_Time.m_fDeltaTime = deltaTime;

#if SHOW_FPS_COUNTER
			{
				if (fmod(currentTime, 1.0f / FPS_COUNTER_REFRESH_FREQUENCY) < FRAME_DURATION)
				{
					printf("FPS: %f\n", 1.0f / deltaTime);
				}
			}
#endif
			
			lastFrameTime = GetTimeSinceStartupSeconds();
		}
	done:
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRC);
		ReleaseDC(hWnd, hDC);
	}

}

