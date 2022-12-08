/*
Example native Windows app, not using any runtime libraries!

Copyright Dmitry Brant.
*/

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
// Windows Header Files
#include <windows.h>
#include <commctrl.h>

#include "resource.h"

extern "C"
{
    // required by the compiler for floating-point usage.
    int _fltused;
}

// Global Variables:
HINSTANCE hInst;
HWND mainWindow;


LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    MainDlgProc(HWND, UINT, WPARAM, LPARAM);

// Insert manifest for using themed Windows components.
#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")


// The actual entry point of the program, which will in turn call WinMain.
void WinMainCRTStartup()
{
    STARTUPINFO StartupInfo;
    LPWSTR lpszCommandLine = GetCommandLine();
    // TODO: do something with the command line.

    GetStartupInfo(&StartupInfo);

    int ret = WinMain(GetModuleHandle(NULL), 0, 0,
        StartupInfo.dwFlags & STARTF_USESHOWWINDOW ? StartupInfo.wShowWindow : SW_SHOWDEFAULT);
    ExitProcess(ret);
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    hInst = hInstance;

    // Create our main dialog window and show it!
    mainWindow = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_MAIN), 0, MainDlgProc, 0);
    ShowWindow(mainWindow, nCmdShow);

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}


INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    mainWindow = hDlg;

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_EXIT:
        case IDOK:
        case IDCANCEL:
            DestroyWindow(hDlg);
            break;
        case ID_FILE_ABOUT:
            MessageBox(hDlg, L"WinApp!", L"Caption", MB_OK);
        }
    }
    return TRUE;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hDlg, &ps);

        RECT rect{};
        rect.left = 32;
        rect.top = 32;
        rect.right = 200;
        rect.bottom = 120;

        HBRUSH brush = CreateSolidBrush(RGB(50, 151, 151));
        FillRect(hdc, &rect, brush);
        DrawText(hdc, L"Foo", 3, &rect, 0);
        EndPaint(hDlg, &ps);
    }
    return TRUE;

    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;
    }
    return FALSE;
}
