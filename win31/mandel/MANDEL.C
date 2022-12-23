/****************************************************************************

Bare-bones Mandelbrot viewer for Windows 3.1.

Dmitry Brant, 2022

****************************************************************************/

#include <windows.h>
#include "mandel.h"

HANDLE hInst;
HCURSOR cursorArrow, cursorWait, cursorMove;

DWORD bmpSizeAllocated = 0;
HANDLE hBmpInfo;
BITMAPINFO huge* bmpInfo;
DWORD bmpWidth, bmpHeight;
HANDLE bmpMem = NULL;
BYTE huge* bmpBits = NULL;

int numIterations = 32;
int colorPaletteSize = 256;
RGBQUAD huge* colorPalette;

double xCenter = -0.5;
double yCenter = 0.0;
double xExtent = 3.0;

BOOL mouseDown = FALSE;
int prevX, prevY;


int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	if (!hPrevInstance)
	if (!InitApplication(hInstance))
		return (FALSE);

	if (!InitInstance(hInstance, nCmdShow))
		return (FALSE);

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


    GlobalUnlock(hBmpInfo);
    GlobalFree(hBmpInfo);

    if (bmpMem != NULL) {
    	GlobalUnlock(bmpMem);
    	GlobalFree(bmpMem);
    }
	return (msg.wParam);
}


BOOL InitApplication(HANDLE hInstance)
{
	WNDCLASS  wc;

	cursorArrow = LoadCursor(NULL, IDC_ARROW);
	cursorWait = LoadCursor(NULL, IDC_WAIT);
	cursorMove = LoadCursor(NULL, IDC_SIZE);

	wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = cursorArrow;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName =  "MandelMenu";
	wc.lpszClassName = "MandelWClass";

	return (RegisterClass(&wc));
}


BOOL InitInstance(HANDLE hInstance, int nCmdShow)
{
	HWND hwnd;
	int i, colorLevel;

	hInst = hInstance;

	hwnd = CreateWindow(
		"MandelWClass",
		"Mandelbrot Viewer",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		400,
		300,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hwnd)
		return (FALSE);

    // Create and initialize our color palette,
    // for use with an 8-bit (256-color) indexed bitmap.
    hBmpInfo = GlobalAlloc(GPTR, sizeof(BITMAPINFO) + colorPaletteSize * sizeof(RGBQUAD));
    bmpInfo = (BITMAPINFO huge*)GlobalLock(hBmpInfo);
    colorPalette = bmpInfo->bmiColors;
    for (i = 0; i < 64; i++) {
    	colorLevel = i * 4;
        colorPalette[i].rgbRed = (BYTE)colorLevel;
        colorPalette[i].rgbGreen = 0;
        colorPalette[i].rgbBlue = (BYTE)(255 - colorLevel);
        colorPalette[i + 64].rgbRed = (BYTE)(255 - colorLevel);
        colorPalette[i + 64].rgbGreen = (BYTE)colorLevel;
        colorPalette[i + 64].rgbBlue = 0;
        colorPalette[i + 128].rgbRed = 0;
        colorPalette[i + 128].rgbGreen = 255;
        colorPalette[i + 128].rgbBlue = (BYTE)colorLevel;
        colorPalette[i + 192].rgbRed = 0;
        colorPalette[i + 192].rgbGreen = (BYTE)(255 - colorLevel);
        colorPalette[i + 192].rgbBlue = 255;
    }
    // Set the last color index to black explicitly.
    colorPalette[colorPaletteSize-1].rgbRed =
        colorPalette[colorPaletteSize-1].rgbGreen =
            colorPalette[colorPaletteSize-1].rgbBlue = 0;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return (TRUE);

}

long FAR PASCAL MainWndProc(HWND hWnd, unsigned message, WORD wParam, LONG lParam)
{
	FARPROC lpProcAbout;
	RECT rect;
	HDC hDC;
	int x, y;
	double aspect;

	switch (message)
	{
		case WM_CREATE:
			break;

        case WM_SETCURSOR:
            if (mouseDown) {
	            SetCursor(cursorMove);
            } else {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;

		case WM_LBUTTONDOWN:
		    mouseDown = TRUE;
		    prevX = LOWORD(lParam);
		    prevY = HIWORD(lParam);
			break;

		case WM_MOUSEMOVE:
			break;

		case WM_LBUTTONUP:
			x = LOWORD(lParam);
	        y = HIWORD(lParam);
		    if (mouseDown && ((abs(x - prevX) > 4) || (abs(y - prevY) > 4))) {
	            xCenter += (xExtent / (double)bmpWidth * (double)(prevX - x));
	            aspect = (double)bmpWidth / (double)bmpHeight;
	            yCenter -= (xExtent / aspect / (double)bmpHeight * (double)(prevY - y));
	            prevX = x;
	            prevY = y;

                FullRedraw(hWnd);
	        }
	        mouseDown = FALSE;
			break;

		case WM_LBUTTONDBLCLK:
			mouseDown = FALSE;
			x = LOWORD(lParam);
	        y = HIWORD(lParam);

		    ZoomBy(0.66666, x, y);
	        FullRedraw(hWnd);
			break;

		case WM_RBUTTONUP:
		    mouseDown = FALSE;
		    x = LOWORD(lParam);
	        y = HIWORD(lParam);

	        ZoomBy(1.5, x, y);
	        FullRedraw(hWnd);
			break;

		case WM_ERASEBKGND:

            hDC = GetDC(hWnd);

            if (bmpBits != NULL) {
              // write the bitmap to the screen in the fastest way possible.
              SetDIBitsToDevice(hDC, 0, 0, (WORD)bmpWidth, (WORD)bmpHeight, 0, 0, 0,
                (WORD)bmpHeight, (LPSTR)bmpBits, bmpInfo, DIB_RGB_COLORS);
            }

            ReleaseDC(hWnd, hDC);

			return TRUE;

        case WM_SIZE:

            FullRedraw(hWnd);
            return TRUE;

		case WM_COMMAND:
			switch (wParam)
			{
				case IDM_ABOUT:

				    MessageBox(hWnd, "Mandelbrot Viewer, version 0.1.\nDmitry Brant, 2022.", "About...", MB_OK | MB_ICONINFORMATION);

					break;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (NULL);
}


void FullRedraw(HWND hWnd) {
    RECT rect;

    SetCursor(cursorWait);
    RecreateBitmap(hWnd);
    DrawMandelbrot();

    GetClientRect(hWnd, &rect);
    InvalidateRect(hWnd, &rect, TRUE);
    SetCursor(cursorArrow);
}

void RecreateBitmap(HWND hWnd) {
    DWORD bmpSizeRequired;
    RECT rect;

    // Resize our bitmap to fit the size of the given window.
    GetClientRect(hWnd, &rect);
    bmpWidth = rect.right;
    if (bmpWidth % 2 > 0) {
      bmpWidth++;
    }

    bmpHeight = rect.bottom;

    // do we need to reallocate our buffer?
    bmpSizeRequired = bmpWidth * bmpHeight * sizeof(BYTE);
    if (bmpSizeRequired > bmpSizeAllocated) {
        if (bmpMem != NULL) {
            GlobalUnlock(bmpMem);
            GlobalFree(bmpMem);
        }
        // allocate twice the amount of requested bytes, to reduce reallocations upon resize.
        bmpSizeAllocated = 2 * bmpSizeRequired;
        bmpMem = GlobalAlloc(GMEM_FIXED, bmpSizeAllocated);

        if (bmpMem == NULL) {
          MessageBox(hWnd, "Allocation failed!", "Error", MB_ICONEXCLAMATION);
        } else {
          bmpBits = (BYTE huge*)GlobalLock(bmpMem);
        }
    }

    // update our BITMAPINFO object, for painting the bitmap onto the window
    bmpInfo->bmiHeader.biXPelsPerMeter = 0;
    bmpInfo->bmiHeader.biYPelsPerMeter = 0;
    bmpInfo->bmiHeader.biClrUsed = colorPaletteSize;
    bmpInfo->bmiHeader.biClrImportant = colorPaletteSize;
    bmpInfo->bmiHeader.biBitCount = 8;
    bmpInfo->bmiHeader.biWidth = bmpWidth;
    bmpInfo->bmiHeader.biHeight = bmpHeight;
    bmpInfo->bmiHeader.biPlanes = 1;
    bmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo->bmiHeader.biSizeImage = bmpWidth * bmpHeight;
    bmpInfo->bmiHeader.biCompression = BI_RGB;
}


int abs(int n) {
	return n < 0 ? -n : n;
}

void ZoomBy(double factor, int x, int y) {
    double aspect = (double)bmpWidth / (double)bmpHeight;
    double xmin = xCenter - xExtent / 2.0, xmax = xmin + xExtent;
    double xpos = xmin + ((double)x * xExtent / (double)bmpWidth);
    double ymin = yCenter - xExtent / aspect / 2.0, ymax = ymin + xExtent / aspect;
    double ypos = ymin + ((double)(bmpHeight - y) * xExtent / aspect / (double)bmpHeight);

    xmin = xpos - (xpos - xmin) * factor;
    xmax = xpos + (xmax - xpos) * factor;
    ymin = ypos - (ypos - ymin) * factor;
    ymax = ypos + (ymax - ypos) * factor;

    xExtent = xmax - xmin;
    xCenter = xmin + xExtent / 2.0;
    yCenter = ymin + xExtent / aspect / 2.0;
}

void DrawMandelbrot(void) {
    DWORD viewWidth = bmpWidth;
    DWORD viewHeight = bmpHeight;
    DWORD startX = 0, maxX = startX + bmpWidth;
    DWORD startY, maxY;

    double xmin = xCenter - (xExtent / 2.0);
    double xmax = xmin + xExtent;
    double aspect = (double)viewWidth / (double)viewHeight;
    double ymin = yCenter - (xExtent / aspect / 2.0);
    double ymax = ymin + (xExtent / aspect);

    double x, y, x0, y0, x2, y2, xinc;
    double xscale = (xmax - xmin) / viewWidth;
    double yscale = (ymax - ymin) / viewHeight;
    int iteration;
    int iterScale = 1;
    DWORD px, py, yptr;

    if (bmpBits == NULL) {
        return;
    }

    startY = 0;
    maxY = (int)bmpHeight;
    xinc = xscale * (double)(maxX - startX) / (double)bmpWidth;

    if (numIterations < colorPaletteSize) {
        iterScale = colorPaletteSize / numIterations;
    }

    for (py = startY; py < maxY; py++) {
        y0 = ymin + (double)py * yscale;
        x0 = xmin + (double)startX * xscale;
        yptr = py * viewWidth;

        for (px = startX; px < maxX; px++) {

            x = y = x2 = y2 = 0.0;
            iteration = 0;
            x0 += xinc;
            while (x2 + y2 < 4.0 && iteration++ < numIterations) {
                y = 2.0 * x * y + y0;
                x = x2 - y2 + x0;
                x2 = x * x;
                y2 = y * y;
            }

            bmpBits[yptr + px] = (BYTE)(iteration >= numIterations ?
            	colorPaletteSize-1 : (iteration * iterScale) % (colorPaletteSize-1));
        }
    }
    return;
}

