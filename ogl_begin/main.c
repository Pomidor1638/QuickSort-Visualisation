#include <windows.h>
#include <gl/gl.h>

#pragma comment(lib, "opengl32.lib")

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

#define MAX_COUNT 1000

int num = MAX_COUNT;
int Array[MAX_COUNT];

HDC hDC;

void swap(int* a, int* b) {
	int c = *a;
	*a = *b;
	*b = c;
}

void Shuffle() {
	int randnum, i;

	/*for (int i = 0; i < num; i++)
		Array[i] = rand() % num + 1;*/

	for (int i = 0; i < num; i++)
		Array[i] = i + 1;


	for (int i = 0; i < num; i++) {
		swap(&Array[i], &Array[i + rand() % (num - i)]);
	}
}

#define Width  (800.0/MAX_COUNT);
#define Height (800.0/MAX_COUNT);

void DrawArray(int i, int j) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();

	for (int k = 0; k < num; k++) {
		glBegin(GL_QUADS); {

			if (k == i || k == j) {

				if (k == i)
					glColor3f(0, 1, 0);
				else
					glColor3f(1, 0, 0);
				glVertex2f(k, 0);
				glVertex2f(k + 1, 0);
				glVertex2f(k + 1, Array[k]);
				glVertex2f(k, Array[k]);
			}
			else {
				glColor3f(1, 1, 1);

				/*glVertex2f(k, 0);
				glVertex2f(k + 1, 0);
				glVertex2f(k + 1, Array[k]);
				glVertex2f(k, Array[k]);*/

				glVertex2f(k, Array[k]);
				glVertex2f(k + 1, Array[k]);
				glVertex2f(k + 1, Array[k] + 1);
				glVertex2f(k, Array[k] + 1);
			}
		}
		glEnd();
	}

	glPopMatrix();

	SwapBuffers(hDC);
}

void QuickSort(int left, int right) {
	if (left >= right)
		return;
	int pivot = Array[(left + right) / 2];
	int i = left;
	int j = right;
	while (i <= j) {

		while (Array[i] < pivot) {
			i++;
		}

		while (Array[j] > pivot) {
			j--;
		}

		if (i <= j) {


			swap(&Array[i], &Array[j]);
			DrawArray(i, j);
			i++;
			j--;
		}
	}

	QuickSort(left, j);
	QuickSort(i, right);
}


void BubbleSort() {
	int i, j;
	for (i = 0; i < num; i++) {
		for (j = 0; j < num - 1 - i; j++) {
			if (Array[j] > Array[j + 1]) {
				swap(&Array[j], &Array[j + 1]);
			}
			DrawArray(j, j + 1);
		}
	}
}



int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hwnd;

	HGLRC hRC;
	MSG msg;
	BOOL bQuit = FALSE;
	float theta = 0.0f;

	/* register window class */
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "GLSample";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


	if (!RegisterClassEx(&wcex))
		return 0;

	/* create main window */
	hwnd = CreateWindowEx(0,
		"GLSample",
		"OpenGL Sample",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1000,
		800,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, nCmdShow);

	/* enable OpenGL for the window */
	EnableOpenGL(hwnd, &hDC, &hRC);

	glOrtho(0, MAX_COUNT, 0, MAX_COUNT, -1, 1);

	/* program main loop */
	while (!bQuit)
	{
		/* check for messages */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* handle or dispatch messages */
			if (msg.message == WM_QUIT)
			{
				bQuit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if (GetKeyState('V') < 0)
		{
			/* OpenGL animation code goes here */

			Shuffle();
			QuickSort(0, num - 1);
			//BubbleSort();


		}

	}

	/* shutdown OpenGL */
	DisableOpenGL(hwnd, hDC, hRC);

	/* destroy the window explicitly */
	DestroyWindow(hwnd);

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	}
	break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC(hwnd);

	/* set the pixel format for the DC */
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}
