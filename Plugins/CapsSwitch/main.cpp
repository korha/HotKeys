#include <windows.h>

#define EXPORT //__declspec(dllexport)

//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam)->vkCode == VK_CAPITAL)
    {
        if (wParam == WM_KEYDOWN)
            if (HWND hWnd = GetForegroundWindow())
            {
                HWND hWndParent;
                while ((hWndParent = GetParent(hWnd)))
                    hWnd = hWndParent;
                PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_FORWARD, 0);
            }
        return 1;
    }
    return CallNextHookEx(0, nCode, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
    static HHOOK hHook;
    if (fdwReason == DLL_PROCESS_ATTACH)
        return (hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0)) ? TRUE : FALSE;        //***sorry for calling SetWindowsHookEx in DllMain
    else if (fdwReason == DLL_PROCESS_DETACH && hHook)
        UnhookWindowsHookEx(hHook);        //***sorry for calling UnhookWindowsHookEx in DllMain
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
EXPORT void fMsg(const wchar_t *) {}
