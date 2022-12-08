rem Run this script from the Visual Studio Developer Command Prompt.

rc winapp.rc

cl.exe -nologo /D "NDEBUG" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /GL /W3 /Zc:wchar_t /fp:fast /Gm- /O2 /GR- /EHa- /Oi /Ot /GS- /arch:AVX2 /Gd /Oi /MT /FC winapp.cpp winapp.res /link /nodefaultlib user32.lib kernel32.lib gdi32.lib
