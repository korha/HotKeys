#include <windows.h>
#include <tlhelp32.h>

#define EXPORT //__declspec(dllexport)

//-------------------------------------------------------------------------------------------------
EXPORT void fMsg(const wchar_t *wMsg)
{
    if (wMsg && *wMsg == L'/')
    {
        ++wMsg;
        if (wcscmp(wMsg, L"suspend") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                DWORD dwPid = 0;
                GetWindowThreadProcessId(hWnd, &dwPid);
                if (dwPid)
                {
                    const HANDLE hndThrd = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPid);
                    if (hndThrd != INVALID_HANDLE_VALUE)
                    {
                        THREADENTRY32 threadEntry32;
                        threadEntry32.dwSize = sizeof(THREADENTRY32);
                        if (Thread32First(hndThrd, &threadEntry32))
                        {
                            do
                            {
                                if (dwPid == threadEntry32.th32OwnerProcessID)
                                    if (const HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadEntry32.th32ThreadID))
                                    {
                                        SuspendThread(hThread);        //***it's ok        //#may be use Wow64SuspendThread if 64to32?
                                        CloseHandle(hThread);
                                    }
                            } while (Thread32Next(hndThrd, &threadEntry32));
                        }
                        CloseHandle(hndThrd);
                    }
                }
            }
        }
        else if (wcscmp(wMsg, L"resume") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                DWORD dwPid = 0;
                GetWindowThreadProcessId(hWnd, &dwPid);
                if (dwPid)
                {
                    const HANDLE hndThrd = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPid);
                    if (hndThrd != INVALID_HANDLE_VALUE)
                    {
                        THREADENTRY32 threadEntry32;
                        threadEntry32.dwSize = sizeof(THREADENTRY32);
                        if (Thread32First(hndThrd, &threadEntry32))
                        {
                            do
                            {
                                if (dwPid == threadEntry32.th32OwnerProcessID)
                                    if (const HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadEntry32.th32ThreadID))
                                    {
                                        ResumeThread(hThread);
                                        CloseHandle(hThread);
                                    }
                            } while (Thread32Next(hndThrd, &threadEntry32));
                        }
                        CloseHandle(hndThrd);
                    }
                }
            }
        }
        else if (wcscmp(wMsg, L"minimize") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                ShowWindow(hWnd, SW_MINIMIZE);
        }
        else if (wcscmp(wMsg, L"toggle-maximize") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                WINDOWPLACEMENT wndpl;
                if (GetWindowPlacement(hWnd, &wndpl))
                    ShowWindow(hWnd, (wndpl.showCmd == SW_MAXIMIZE) ? SW_RESTORE : SW_MAXIMIZE);
            }
        }
        else if (wcscmp(wMsg, L"toggle-topmost") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                SetWindowPos(hWnd, (GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else if (wcscmp(wMsg, L"toggle-caption") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) ^ WS_CAPTION);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        else if (wcscmp(wMsg, L"close") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        else if (wcsncmp(wMsg, L"terminate:", 10) == 0)
        {
            wchar_t *wOk;
            const unsigned int iExitCode = wcstoul(wMsg+10, &wOk, 10);
            if (!(*wOk || errno))
                if (const HWND hWnd = GetForegroundWindow())
                {
                    DWORD dwPid = 0;
                    GetWindowThreadProcessId(hWnd, &dwPid);
                    if (dwPid)
                        if (const HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwPid))
                        {
                            TerminateProcess(hProcess, iExitCode);
                            CloseHandle(hProcess);
                        }
                }
        }
        else if (wcsncmp(wMsg, L"opacity:", 8) == 0)
        {
            switch (wMsg[8])
            {
            case L'+':
            {
                wchar_t *wOk;
                long iOpacity = wcstol(wMsg+9, &wOk, 10);
                if (!(*wOk || errno) && iOpacity > 0 && iOpacity <= 255)
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
                break;
            }

            case L'-':
            {
                wchar_t *wOk;
                long iOpacity = wcstol(wMsg+9, &wOk, 10);
                if (!(*wOk || errno) && iOpacity > 0 && iOpacity < 255)
                    if (const HWND hWnd = GetForegroundWindow())
                    {
                        DWORD dwFlags;
                        BYTE bAlpha;
                        if (GetLayeredWindowAttributes(hWnd, 0, &bAlpha, &dwFlags) && (dwFlags & LWA_ALPHA))
                        {
                            iOpacity = bAlpha - iOpacity;
                            if (iOpacity < 1)
                                iOpacity = 1;
                        }
                        else
                        {
                            SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                            iOpacity = 255 - iOpacity;
                        }
                        SetLayeredWindowAttributes(hWnd, 0, iOpacity, LWA_ALPHA);
                    }
                break;
            }

            case L'=':
            {
                wchar_t *wOk;
                const long iOpacity = wcstol(wMsg+9, &wOk, 10);
                if (!(*wOk || errno) && iOpacity > 0 && iOpacity <= 255)
                    if (const HWND hWnd = GetForegroundWindow())
                    {
                        DWORD dwFlags;
                        if (!(GetLayeredWindowAttributes(hWnd, 0, 0, &dwFlags) && (dwFlags & LWA_ALPHA)))
                            SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                        SetLayeredWindowAttributes(hWnd, 0, iOpacity, LWA_ALPHA);
                    }
                break;
            }
            }
        }
        else if (wcscmp(wMsg, L"resize") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) | WS_SIZEBOX);
        }
        else if (wcscmp(wMsg, L"maximize") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                ShowWindow(hWnd, SW_MAXIMIZE);
        }
        else if (wcscmp(wMsg, L"restore") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                ShowWindow(hWnd, SW_RESTORE);
        }
        else if (wcscmp(wMsg, L"topmost") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else if (wcscmp(wMsg, L"notopmost") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
                SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else if (wcscmp(wMsg, L"caption-off") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) & ~WS_CAPTION);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        else if (wcscmp(wMsg, L"caption-on") == 0)
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) | WS_CAPTION);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
    }
}
