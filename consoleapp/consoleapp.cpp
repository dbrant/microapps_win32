#include <windows.h>

HANDLE hStdOut;

void printf(LPWSTR str, ...) {
    va_list myargs;
    va_start(myargs, str);
    WCHAR outStr[256];
    wvsprintf(outStr, str, myargs);
    va_end(myargs);
    DWORD dwSize = 0;
    WriteConsole(hStdOut, outStr, lstrlen(outStr), &dwSize, NULL);
}

int main()
{
    printf(L"Hello, world! %d", 123);
    return(0);
}

void mainCRTStartup()
{
    AllocConsole();
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    int Result = main();

    FreeConsole();
    ExitProcess(Result);
}
