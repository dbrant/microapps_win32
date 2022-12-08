rem Run this script from the Visual Studio Developer Command Prompt.

rem Compile the resources into a .res file, for consumption by the linker.
rc winapp.rc

cl.exe -nologo /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /GL /W3 /Zc:wchar_t /fp:fast /Gm- /O2 /GR- /EHa- /Oi /Ot /GS- /arch:AVX2 /Gd /Oi /MT /FC winapp.cpp winapp.res /link /nodefaultlib user32.lib kernel32.lib gdi32.lib

rem Embed the manifest in the output file.
mt.exe -manifest manifest.xml -outputresource:winapp.exe;1
