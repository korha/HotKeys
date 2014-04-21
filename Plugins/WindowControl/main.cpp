#include <windows.h>

#define EXPORT //__declspec(dllexport)

//-------------------------------------------------------------------------------------------------
bool EXPORT fInit(const bool)
{
    return true;
}

//-------------------------------------------------------------------------------------------------
void EXPORT fMsg(const wchar_t *wMsg)
{
    if (*wMsg == L'/')
    {
        ++wMsg;
        if (wcscmp(L"minimize", wMsg) == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                ShowWindow(hWnd, SW_MINIMIZE);
        }
        else if (wcscmp(L"toggle-maximize", wMsg) == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                WINDOWPLACEMENT wndpl;
                if (GetWindowPlacement(hWnd, &wndpl))
                    ShowWindow(hWnd, (wndpl.showCmd == SW_MAXIMIZE) ? SW_RESTORE : SW_MAXIMIZE);
            }
        }
        else if (wcscmp(L"toggle-topmost", wMsg) == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                SetWindowPos(hWnd, (GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else if (wcscmp(L"toggle-caption", wMsg) == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) ^ WS_CAPTION);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        else if (wcscmp(L"close", wMsg) == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        else if (wcsncmp(L"terminate:", wMsg, 10) == 0)
        {
            wchar_t *wOk;
            const unsigned int iExitCode = wcstoul(wMsg+10, &wOk, 10);
            if (!(*wOk || errno))
                if (const HWND hWnd = GetForegroundWindow())
                {
                    DWORD dwProcessId;
                    GetWindowThreadProcessId(hWnd, &dwProcessId);
                    if (const HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId))
                    {
                        TerminateProcess(hProcess, iExitCode);
                        CloseHandle(hProcess);
                    }
                }
        }
        else if (wcsncmp(L"opacity:", wMsg, 8) == 0)
        {
            switch(wMsg[8])
            {
            case L'+':
            {
                wchar_t *wOk;
                unsigned long iOpacity = wcstoul(wMsg+9, &wOk, 10);
                if (!(*wOk || errno) && iOpacity > 0 && iOpacity <= 255)
                {
                    if (const HWND hWnd = GetForegroundWindow())
                    {
                        DWORD dwFlags;
                        BYTE bAlpha;
                        if (GetLayeredWindowAttributes(hWnd, 0, &bAlpha, &dwFlags) && (dwFlags & LWA_ALPHA))
                        {
                            iOpacity += bAlpha;
                            if (iOpacity > 255)
                                iOpacity = 255;
                            SetLayeredWindowAttributes(hWnd, 0, iOpacity, LWA_ALPHA);
                        }
                    }
                }
                break;
            }

            case L'-':
            {
                wchar_t *wOk;
                long iOpacity = wcstol(wMsg+9, &wOk, 10);
                if (!(*wOk || errno) && iOpacity > 0 && iOpacity < 255)
                {
                    if (const HWND hWnd = GetForegroundWindow())
                    {
                        DWORD dwFlags;
                        BYTE bAlpha;
                        if (GetLayeredWindowAttributes(hWnd, 0, &bAlpha, &dwFlags) && (dwFlags & LWA_ALPHA))
                        {
                            iOpacity = bAlpha - iOpacity;
                            if (iOpacity < 1)
                                iOpacity = 1;
                            SetLayeredWindowAttributes(hWnd, 0, iOpacity, LWA_ALPHA);
                        }
                        else
                        {
                            SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                            SetLayeredWindowAttributes(hWnd, 0, 255-iOpacity, LWA_ALPHA);
                        }
                    }
                }
                break;
            }

            case L'=':
            {
                wchar_t *wOk;
                const unsigned long iOpacity = wcstoul(wMsg+9, &wOk, 10);
                if (!(*wOk || errno) && iOpacity > 0 && iOpacity <= 255)
                {
                    if (const HWND hWnd = GetForegroundWindow())
                    {
                        DWORD dwFlags;
                        if (!(GetLayeredWindowAttributes(hWnd, 0, 0, &dwFlags) && (dwFlags & LWA_ALPHA)))
                            SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                        SetLayeredWindowAttributes(hWnd, 0, iOpacity, LWA_ALPHA);
                    }
                }
                break;
            }
            }
        }
        else if (wcscmp(L"maximize", wMsg) == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                ShowWindow(hWnd, SW_MAXIMIZE);
        }
        else if (wcscmp(L"restore", wMsg) == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                ShowWindow(hWnd, SW_RESTORE);
        }
        else if (wcscmp(L"topmost", wMsg) == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else if (wcscmp(L"notopmost", wMsg) == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else if (wcscmp(L"caption-off", wMsg) == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) & ~WS_CAPTION);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        else if (wcscmp(L"caption-on", wMsg) == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) | WS_CAPTION);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
    }
}
