//HotKeys: CapsSwitch
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define EXPORT //__declspec(dllexport)

static INPUT g_input[2];

//-------------------------------------------------------------------------------------------------
EXPORT void FMsg(const wchar_t *)
{
    static char cState = '0';
    if (cState == '2')
        cState = '0';
    else
    {
        SendInput(2, g_input, sizeof(INPUT));
        if (cState == '0')
            if (HWND hWnd = GetForegroundWindow())
            {
                HWND hWndParent;
                while ((hWndParent = GetParent(hWnd)))
                    hWnd = hWndParent;
                PostMessageW(hWnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_FORWARD, 0);
            }
        ++cState;
    }
}

//-------------------------------------------------------------------------------------------------
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, DWORD fdwReason, LPVOID)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        g_input[0].type = INPUT_KEYBOARD;
        g_input[0].ki.wVk = VK_CAPITAL;
        //input[0].ki.wScan = 0;
        //input[0].ki.dwFlags = 0;
        //input[0].ki.time = 0;
        //input[0].ki.dwExtraInfo = 0;
        g_input[1].type = INPUT_KEYBOARD;
        g_input[1].ki.wVk = VK_CAPITAL;
        //input[1].ki.wScan = 0;
        g_input[1].ki.dwFlags = KEYEVENTF_KEYUP;
        //input[1].ki.time = 0;
        //input[1].ki.dwExtraInfo = 0;
    }
    return TRUE;
}
