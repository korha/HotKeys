//HotKeys: WindowControl
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>

#define EXPORT //__declspec(dllexport)

#ifdef _WIN64
static DWORD g_dwDesireAccess;
#endif

//-------------------------------------------------------------------------------------------------
static inline bool FCompareMemoryW(const wchar_t *pBuf1, const wchar_t *pBuf2)
{
    while (*pBuf1 == *pBuf2 && *pBuf2)
        ++pBuf1, ++pBuf2;
    return *pBuf1 == *pBuf2;
}

static inline bool FIsStartWithW(const wchar_t *pFullStr, const wchar_t *pBeginStr)
{
    while (*pBeginStr)
        if (*pFullStr++ != *pBeginStr++)
            return false;
    return true;
}

//-------------------------------------------------------------------------------------------------
static void FSuspendResume(const bool bIsSuspend)
{
    if (const HWND hWnd = GetForegroundWindow())
    {
        DWORD dwPid = ASFW_ANY;
        GetWindowThreadProcessId(hWnd, &dwPid);
        if (dwPid != ASFW_ANY)
        {
            typedef DWORD (WINAPI *PThreadFunc)(HANDLE hThread);
            PThreadFunc ThreadFuncPtr = nullptr;
            if (bIsSuspend)
            {
#ifdef _WIN64
                if (const HANDLE hProcess = OpenProcess(g_dwDesireAccess, FALSE, dwPid))
                {
                    BOOL bIsWow64;
                    if (IsWow64Process(hProcess, &bIsWow64))
                        ThreadFuncPtr = bIsWow64 ? Wow64SuspendThread : SuspendThread;
                    CloseHandle(hProcess);
                }
#else
                ThreadFuncPtr = SuspendThread;
#endif
            }
            else
                ThreadFuncPtr = ResumeThread;
            if (ThreadFuncPtr)
            {
                const HANDLE hSnapshotThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPid);
                if (hSnapshotThread != INVALID_HANDLE_VALUE)
                {
                    THREADENTRY32 threadEntry32;
                    threadEntry32.dwSize = sizeof(THREADENTRY32);
                    if (Thread32First(hSnapshotThread, &threadEntry32))
                    {
                        do
                        {
                            if (threadEntry32.th32OwnerProcessID == dwPid)
                                if (const HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadEntry32.th32ThreadID))
                                {
                                    ThreadFuncPtr(hThread);
                                    CloseHandle(hThread);
                                }
                        } while (Thread32Next(hSnapshotThread, &threadEntry32));
                    }
                    CloseHandle(hSnapshotThread);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
EXPORT void FMsg(const wchar_t *wMsg)
{
    if (wMsg && *wMsg == L'/')
    {
        ++wMsg;
        if (FCompareMemoryW(wMsg, L"suspend"))
            FSuspendResume(true);
        else if (FCompareMemoryW(wMsg, L"resume"))
            FSuspendResume(false);
        else if (FCompareMemoryW(wMsg, L"minimize"))
        {
            if (const HWND hWnd = GetForegroundWindow())
                ShowWindow(hWnd, SW_MINIMIZE);
        }
        else if (FCompareMemoryW(wMsg, L"maximize"))
        {
            if (const HWND hWnd = GetForegroundWindow())
                ShowWindow(hWnd, SW_MAXIMIZE);
        }
        else if (FCompareMemoryW(wMsg, L"restore"))
        {
            if (const HWND hWnd = GetForegroundWindow())
                ShowWindow(hWnd, SW_RESTORE);
        }
        else if (FCompareMemoryW(wMsg, L"toggle-maximize"))
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                WINDOWPLACEMENT wndpl;
                wndpl.length = sizeof(WINDOWPLACEMENT);
                if (GetWindowPlacement(hWnd, &wndpl))
                    ShowWindow(hWnd, (wndpl.showCmd == SW_MAXIMIZE) ? SW_RESTORE : SW_MAXIMIZE);
            }
        }
        else if (FCompareMemoryW(wMsg, L"topmost"))
        {
            if (const HWND hWnd = GetForegroundWindow())
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else if (FCompareMemoryW(wMsg, L"notopmost"))
        {
            if (const HWND hWnd = GetForegroundWindow())
                SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else if (FCompareMemoryW(wMsg, L"toggle-topmost"))
        {
            if (const HWND hWnd = GetForegroundWindow())
                SetWindowPos(hWnd, (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }
        else if (FCompareMemoryW(wMsg, L"caption-off"))
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                SetWindowLongPtrW(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) & ~WS_CAPTION);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        else if (FCompareMemoryW(wMsg, L"caption-on"))
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                SetWindowLongPtrW(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) | WS_CAPTION);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        else if (FCompareMemoryW(wMsg, L"toggle-caption"))
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                SetWindowLongPtrW(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) ^ WS_CAPTION);
                SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
        else if (FCompareMemoryW(wMsg, L"resize"))
        {
            if (const HWND hWnd = GetForegroundWindow())
                SetWindowLongPtrW(hWnd, GWL_STYLE, GetWindowLongPtr(hWnd, GWL_STYLE) | WS_SIZEBOX);
        }
        else if (FCompareMemoryW(wMsg, L"close"))
        {
            if (const HWND hWnd = GetForegroundWindow())
                PostMessageW(hWnd, WM_CLOSE, 0, 0);
        }
        else if (FCompareMemoryW(wMsg, L"terminate"))
        {
            if (const HWND hWnd = GetForegroundWindow())
            {
                DWORD dwPid = ASFW_ANY;
                GetWindowThreadProcessId(hWnd, &dwPid);
                if (dwPid != ASFW_ANY)
                    if (const HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwPid))
                    {
                        TerminateProcess(hProcess, 0);
                        CloseHandle(hProcess);
                    }
            }
        }
        else if (FIsStartWithW(wMsg, L"opacity"))
        {
            wMsg += 7;
            if ((*wMsg == L'+' || *wMsg == L'-' || *wMsg == L'=') &&
                    (wMsg[1] >= L'0' && wMsg[1] <= L'2') &&
                    (wMsg[2] >= L'0' && wMsg[2] <= L'9') &&
                    (wMsg[3] >= L'0' && wMsg[3] <= L'9') &&
                    wMsg[4] == L'\0')
            {
                DWORD dwOpacity = (wMsg[1] - L'0')*100 + (wMsg[2] - L'0')*10 + (wMsg[3] - L'0');
                if (dwOpacity <= 255)
                    if (const HWND hWnd = GetForegroundWindow())
                    {
                        DWORD dwFlags;
                        BYTE bAlpha;
                        if (*wMsg == L'+')
                        {
                            if (GetLayeredWindowAttributes(hWnd, nullptr, &bAlpha, &dwFlags) && (dwFlags & LWA_ALPHA))
                            {
                                dwOpacity += bAlpha;
                                if (dwOpacity > 255)
                                    dwOpacity = 255;
                                SetLayeredWindowAttributes(hWnd, 0, dwOpacity, LWA_ALPHA);
                            }
                        }
                        else if (*wMsg == L'-')
                        {
                            if (GetLayeredWindowAttributes(hWnd, nullptr, &bAlpha, &dwFlags) && (dwFlags & LWA_ALPHA))
                            {
                                if (bAlpha > dwOpacity)
                                    dwOpacity = bAlpha - dwOpacity;
                                else
                                    dwOpacity = 0;
                            }
                            else
                            {
                                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                                dwOpacity = 255 - dwOpacity;
                            }
                            SetLayeredWindowAttributes(hWnd, 0, dwOpacity, LWA_ALPHA);
                        }
                        else if (*wMsg == L'=')
                        {
                            if (!(GetLayeredWindowAttributes(hWnd, nullptr, nullptr, &dwFlags) && (dwFlags & LWA_ALPHA)))
                                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                            SetLayeredWindowAttributes(hWnd, 0, dwOpacity, LWA_ALPHA);
                        }
                    }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, DWORD fdwReason, LPVOID)
{
#ifdef _WIN64
    if (fdwReason == DLL_PROCESS_ATTACH)
        g_dwDesireAccess = ((GetVersion() & 0xFF) >= ((_WIN32_WINNT_VISTA >> 8) & 0xFF)) ?
                    PROCESS_QUERY_LIMITED_INFORMATION : PROCESS_QUERY_INFORMATION;
#endif
    return TRUE;
    static_cast<void>(fdwReason);
}
