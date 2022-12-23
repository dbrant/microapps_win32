#define IDM_ABOUT 100

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);

void FullRedraw(HWND);
void RecreateBitmap(HWND);
void DrawMandelbrot(void);
void ZoomBy(double, int, int);
int abs(int);
