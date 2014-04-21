#include <windows.h>

#define EXPORT //__declspec(dllexport)

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (wParam == WM_KEYDOWN && nCode == HC_ACTION && reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam)->vkCode == VK_CAPITAL)
    {
        HWND hWnd = GetForegroundWindow(),
                hWndParent;
        while ((hWndParent = GetParent(hWnd)))
            hWnd = hWndParent;
        PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_FORWARD, 0);
        return 1;
    }
    return CallNextHookEx(0, nCode, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
bool EXPORT fInit(const bool bInit)
{
    static HHOOK hHook;
    if (bInit)
        return (hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(0), 0));
    else
        UnhookWindowsHookEx(hHook);
    return true;
}

//-------------------------------------------------------------------------------------------------
void EXPORT fMsg(const wchar_t *)
{

}
