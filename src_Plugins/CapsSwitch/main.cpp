#include <windows.h>

#define EXPORT //__declspec(dllexport)

INPUT input[2];

//-------------------------------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_CAPITAL;
        input[0].ki.wScan = 0;
        input[0].ki.dwFlags = 0;
        input[0].ki.time = 0;
        input[0].ki.dwExtraInfo = 0;
        input[1].type = INPUT_KEYBOARD;
        input[1].ki.wVk = VK_CAPITAL;
        input[1].ki.wScan = 0;
        input[1].ki.dwFlags = KEYEVENTF_KEYUP;
        input[1].ki.time = 0;
        input[1].ki.dwExtraInfo = 0;
    }
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
EXPORT void fMsg(const wchar_t *)
{
    static char cState = '0';
    if (cState == '2')
        cState = '0';
    else
    {
        SendInput(2, input, sizeof(INPUT));
        if (cState == '0')
            if (HWND hWnd = GetForegroundWindow())
            {
                HWND hWndParent;
                while ((hWndParent = GetParent(hWnd)))
                    hWnd = hWndParent;
                PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_FORWARD, 0);
            }
        ++cState;
    }
}
