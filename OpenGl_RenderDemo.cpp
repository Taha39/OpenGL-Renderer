// OpenGl_RenderDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>

//#include "CaptureBitBLT.h"
#include "render.h"
// Window dimensions

int WIDTH = 0, HEIGHT = 0;

//RECT screen_rect;
//CaptureBitBLt* cap;
std::vector<UINT8> v;
HWND hOGLWnd = NULL;
HGLRC hOGLRenderContext = NULL;

bool ignore = true;
render* my_render;

void display(void)
{	
	//cap->capture_frame(v.data());
	my_render->present(v.data(), WIDTH, HEIGHT);
}


LRESULT CALLBACK WindowProcedure(HWND hWnd, unsigned int msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_ERASEBKGND:
		if (ignore)
			break;
		display();
		break;
	}
	return DefWindowProc(hWnd, msg, wparam, lparam);
}

HWND createWindow(int width, int height)
{
	// Get module handle
	HMODULE hModule = ::GetModuleHandle(0);
	if (!hModule)
		return NULL;

	// Create window calss
	WNDCLASSEX wndClassData;
	memset(&wndClassData, 0, sizeof(WNDCLASSEX));
	wndClassData.cbSize = sizeof(WNDCLASSEX);
	wndClassData.style = CS_DBLCLKS;
	wndClassData.lpfnWndProc = WindowProcedure;
	wndClassData.cbClsExtra = 0;
	wndClassData.cbWndExtra = 0;
	wndClassData.hInstance = hModule;
	wndClassData.hIcon = ::LoadIcon(0, IDI_APPLICATION);
	wndClassData.hCursor = ::LoadCursor(0, IDC_ARROW);
	wndClassData.hbrBackground = ::CreateSolidBrush(COLOR_WINDOW + 1);
	wndClassData.lpszMenuName = 0;
	wndClassData.lpszClassName = (LPCWSTR)L"my_wnd_class";
	wndClassData.hIconSm = 0;
	if (!::RegisterClassEx(&wndClassData))
		return false;

	// Creaate Window
	hOGLWnd = ::CreateWindow((LPCWSTR)L"my_wnd_class", NULL, WS_OVERLAPPEDWINDOW, 0, 0, width, height, NULL, NULL, hModule, NULL);
	if (hOGLWnd == NULL)
		return NULL;

	// Get device context
	HDC hDC = ::GetDC(hOGLWnd);

	// Create OpenGL context
	DWORD pixelFormatFlags = PFD_SUPPORT_OPENGL | PFD_SUPPORT_COMPOSITION | PFD_GENERIC_ACCELERATED | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		pixelFormatFlags,         //Flags
		PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
		32,                       //Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		24,                       //Number of bits for the depthbuffer
		8,                        //Number of bits for the stencilbuffer
		0,                        //Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0, 0, 0, 0
	};
	int pixelFormat = ::ChoosePixelFormat(hDC, &pfd);
	::SetPixelFormat(hDC, pixelFormat, &pfd);
	hOGLRenderContext = ::wglCreateContext(hDC);

	// make OpenGL context the current context
	::wglMakeCurrent(hDC, hOGLRenderContext);

	// show the window
	::ShowWindow(hOGLWnd, SW_SHOWDEFAULT);

	// release device context
	::ReleaseDC(hOGLWnd, hDC);
	return hOGLWnd;
}

void mainloop(void)
{
	MSG msg;
	while (::GetMessage(&msg, 0, 0, 0))
		::DispatchMessage(&msg);
}

void destroyWindow(void)
{
	if (HDC currentDC = ::wglGetCurrentDC())
		::wglMakeCurrent(currentDC, NULL);
	::DestroyWindow(hOGLWnd);
	::wglDeleteContext(hOGLRenderContext);

	HMODULE hModule = ::GetModuleHandle(0);
	if (!hModule)
		return;
	::UnregisterClass((LPCWSTR)L"my_wnd_class", hModule);
}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Passing image width and height
	WIDTH = 1920;
	HEIGHT = 1080;
	FILE* fp;	// Open image and image into vector.
	fopen_s(&fp, "Image1920x1080.rgb", "rb");
	v.resize(WIDTH * HEIGHT * 4);
	fread(v.data(), 1, WIDTH * HEIGHT * 4, fp);
	fclose(fp);

	HWND window = createWindow(WIDTH - 200, HEIGHT - 150);
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		return EXIT_FAILURE;
	}
	my_render = new render(::GetDC(hOGLWnd), WIDTH, HEIGHT);

	if (!my_render->init())
	{
		std::cout << "Render init failed..." << std::endl;
		return EXIT_FAILURE;
	}
	
	ignore = false; // Till render initialised ignore the display call.
	mainloop();

	delete my_render;
	return EXIT_SUCCESS;
}

