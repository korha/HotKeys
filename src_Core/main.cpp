//HotKeys
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#include <windows.h>

typedef void (*PFMsg)(const wchar_t *wMsg);

static constexpr const wchar_t *const g_wGuidClass = L"App::b6c9d088-c4a5-45f2-8481-87b29bcaec50";

//static constexpr const WORD g_iUnusedCmdForMsg = SW_HIDE        //min
//static constexpr const WORD g_iUnusedCmdForMsg = SW_SHOWNORMAL
//static constexpr const WORD g_iUnusedCmdForMsg = SW_NORMAL
//static constexpr const WORD g_iUnusedCmdForMsg = SW_SHOWMINIMIZED
//static constexpr const WORD g_iUnusedCmdForMsg = SW_MAXIMIZE
//static constexpr const WORD g_iUnusedCmdForMsg = SW_SHOWNOACTIVATE
static constexpr const WORD g_iUnusedCmdForMsg = SW_SHOW;
//static constexpr const WORD g_iUnusedCmdForMsg = SW_MINIMIZE
//static constexpr const WORD g_iUnusedCmdForMsg = SW_SHOWMINNOACTIVE        //max

//-------------------------------------------------------------------------------------------------
#ifdef NDEBUG
#define ___assert___(cond) do{static_cast<void>(sizeof(cond));}while(false)
#else
#define ___assert___(cond) do{if(!(cond)){int i=__LINE__;char h[]="RUNTIME ASSERTION. Line:           "; \
    if(i>=0){char *c=h+35;do{*--c=i%10+'0';i/=10;}while(i>0);} \
    if(MessageBoxA(nullptr,__FILE__,h,MB_ICONERROR|MB_OKCANCEL)==IDCANCEL)ExitProcess(0);}}while(false)
#endif

static inline void FZeroMemory(void *const pDst__, DWORD dwSize)
{
    BYTE *pDst = static_cast<BYTE*>(pDst__);
    while (dwSize--)
        *pDst++ = '\0';
}

static inline bool FCompareMemoryW(const wchar_t *pBuf1, const wchar_t *pBuf2)
{
    while (*pBuf1 == *pBuf2 && *pBuf2)
        ++pBuf1, ++pBuf2;
    return *pBuf1 == *pBuf2;
}

static inline void FCopyMemoryW(wchar_t *pDst, const wchar_t *pSrc)
{
    while ((*pDst++ = *pSrc++));
}

static inline wchar_t * FStrChrW(wchar_t *pSrc, const wchar_t wChar)
{
    while (*pSrc && *pSrc != wChar)
        ++pSrc;
    return *pSrc == wChar ? pSrc : nullptr;
}

//-------------------------------------------------------------------------------------------------
static const wchar_t * FGetArg()
{
    if (wchar_t *wCmdLine = GetCommandLineW())
    {
        while (*wCmdLine == L' ' || *wCmdLine == L'\t')
            ++wCmdLine;
        if (*wCmdLine != L'\0')
        {
            //1st
            if (*wCmdLine++ == L'"')
            {
                while (*wCmdLine != L'\"')
                {
                    if (*wCmdLine == L'\0')
                        return nullptr;
                    ++wCmdLine;
                }
                ++wCmdLine;
                if (*wCmdLine != L' ' && *wCmdLine != L'\t')
                    return nullptr;
            }
            else
                while (*wCmdLine != L' ' && *wCmdLine != L'\t')
                {
                    if (*wCmdLine == L'\0' || *wCmdLine == L'\"')
                        return nullptr;
                    ++wCmdLine;
                }

            //2nd
            do {++wCmdLine;}
            while (*wCmdLine == L' ' || *wCmdLine == L'\t');
            if (*wCmdLine != L'\0')
            {
                const wchar_t *wArg = wCmdLine;
                if (*wCmdLine++ == L'"')
                {
                    while (*wCmdLine != L'\"')
                    {
                        if (*wCmdLine == L'\0')
                            return nullptr;
                        ++wCmdLine;
                    }
                    if (wCmdLine[1] != L' ' && wCmdLine[1] != L'\t' && wCmdLine[1] != L'\0')
                        return nullptr;
                    ++wArg;
                }
                else
                    while (*wCmdLine != L' ' && *wCmdLine != L'\t' && *wCmdLine != L'\0')
                        ++wCmdLine;
                *wCmdLine = L'\0';
                return wArg;
            }
        }
    }
    return nullptr;
}

//-------------------------------------------------------------------------------------------------
struct SEntries
{
    union
    {
        wchar_t *wStrCmdLine;
        PFMsg FMsg;
    };
    union
    {
        wchar_t *wStrWorkDir,
        *wStrMessage;
    };
    WORD iShowCmd;
};

//-------------------------------------------------------------------------------------------------
struct SCreateParams
{
    SEntries *pSEntries;
    PROCESS_INFORMATION *pPi;
    STARTUPINFO *pSi;
    bool *pbIsRegister;
    size_t szCount;
};

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static const SEntries *sEntries;
    static PROCESS_INFORMATION *pPi;
    static STARTUPINFO *pSi;
    static const bool *bIsRegister;
    static size_t szCount;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        const SCreateParams *const sCreateParams = static_cast<const SCreateParams*>(reinterpret_cast<const CREATESTRUCT*>(lParam)->lpCreateParams);
        sEntries = sCreateParams->pSEntries;
        pPi = sCreateParams->pPi;
        pSi = sCreateParams->pSi;
        bIsRegister = sCreateParams->pbIsRegister;
        szCount = sCreateParams->szCount;
        return 0;
    }
    case WM_HOTKEY:
    {
        const SEntries &sEntry = sEntries[wParam];
        if (sEntry.iShowCmd == g_iUnusedCmdForMsg)
            sEntry.FMsg(sEntry.wStrMessage);
        else
        {
            ___assert___(sEntry.iShowCmd <= SW_SHOWMINNOACTIVE);
            if (sEntry.iShowCmd != SW_SHOWNORMAL)
            {
                pSi->dwFlags = STARTF_USESHOWWINDOW;
                pSi->wShowWindow = sEntry.iShowCmd;
            }
            if (CreateProcessW(nullptr, sEntry.wStrCmdLine, nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT, nullptr, sEntry.wStrWorkDir, pSi, pPi))
            {
                CloseHandle(pPi->hThread);
                CloseHandle(pPi->hProcess);
            }
            if (sEntry.iShowCmd != SW_SHOWNORMAL)
            {
                pSi->dwFlags = 0;
                pSi->wShowWindow = 0;
            }
        }
        return 0;
    }
    case WM_DESTROY:
    {
        for (size_t i = 0; i < szCount; ++i)
            if (bIsRegister[i])
                UnregisterHotKey(hWnd, i);
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
static void FMain()
{
    //close previous app --------------------------------------------------------------------------
    if (const HWND hWnd = FindWindowExW(HWND_MESSAGE, nullptr, g_wGuidClass, nullptr))
        PostMessageW(hWnd, WM_CLOSE, 0, 0);

    //get argument --------------------------------------------------------------------------------
    const wchar_t *const wArg = FGetArg();
    if (!(wArg && wArg[0] == L'/' && wArg[1] == L'q' && wArg[2] == L'\0'))
    {
        const wchar_t *wError = nullptr;
        //get app path ----------------------------------------------------------------------------
        wchar_t wBuf[MAX_PATH];
        DWORD dwTemp = GetModuleFileNameW(nullptr, wBuf, (MAX_PATH+1)-14);        //"_plugins\a.dll"
        if (dwTemp >= 4 && dwTemp < MAX_PATH-14)
        {
            //detect plugins ----------------------------------------------------------------------
            wchar_t *wTemp = wBuf+dwTemp;
            FCopyMemoryW(wTemp, L"_plugins\\*.dll");
            size_t szCount = 0;
            WIN32_FIND_DATA findFileData;
            HANDLE hFind = FindFirstFileW(wBuf, &findFileData);
            if (hFind != INVALID_HANDLE_VALUE)
            {
                do
                {
                    if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                        ++szCount;
                } while (FindNextFileW(hFind, &findFileData));
                FindClose(hFind);
            }

            if (const HANDLE hProcHeap = GetProcessHeap())
            {
                const size_t szCountDlls = szCount;
                if (PFMsg *fMsgs = static_cast<PFMsg*>(HeapAlloc(hProcHeap, HEAP_NO_SERIALIZE, szCountDlls*sizeof(PFMsg))))
                {
                    if (wchar_t **wNames = static_cast<wchar_t**>(HeapAlloc(hProcHeap, HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY, szCountDlls*sizeof(wchar_t*))))
                    {
                        size_t szIndex = 0;
                        hFind = FindFirstFileW(wBuf, &findFileData);
                        if (hFind != INVALID_HANDLE_VALUE)
                        {
                            do
                            {
                                if (szIndex < szCount && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                                {
                                    wchar_t *wIt = findFileData.cFileName;
                                    while (*wIt++);
                                    if ((wNames[szIndex] = static_cast<wchar_t*>(HeapAlloc(hProcHeap, HEAP_NO_SERIALIZE, (wIt-findFileData.cFileName)*sizeof(wchar_t)))))
                                        FCopyMemoryW(wNames[szIndex], findFileData.cFileName);
                                    else
                                    {
                                        wError = L"Allocation memory error";
                                        break;
                                    }
                                    ++szIndex;
                                }
                            } while (FindNextFileW(hFind, &findFileData));
                            FindClose(hFind);
                        }

                        if (!wError)
                        {
                            if (szIndex == szCount)
                            {
                                //load plugins --------------------------------------------------------
                                if (HMODULE *const pModules = static_cast<HMODULE*>(HeapAlloc(hProcHeap, HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY, szCountDlls*sizeof(HMODULE))))
                                {
                                    bool bNeedCom = false;
                                    wTemp += 9;        //"_plugins\"
                                    for (szIndex = 0; szIndex < szCount; ++szIndex)
                                    {
                                        FCopyMemoryW(wTemp, wNames[szIndex]);
                                        if (const HMODULE hMod = LoadLibraryW(wBuf))
                                        {
                                            pModules[szIndex] = hMod;
                                            if (!(fMsgs[szIndex] = reinterpret_cast<PFMsg>(GetProcAddress(hMod, "FMsg"))))
                                            {                     //[C:\a_plugins\\]
                                                FCopyMemoryW(wBuf, L"FMsg failed:\n");
                                                FCopyMemoryW(wBuf+13, wTemp);
                                                wError = L"";
                                                break;
                                            }
                                            if (!bNeedCom && GetProcAddress(hMod, "FNeedCom"))
                                                bNeedCom = true;
                                        }
                                        else
                                        {                     //[C:\a_plugins\\]
                                            FCopyMemoryW(wBuf, L"Load failed:\n");
                                            FCopyMemoryW(wBuf+13, wTemp);
                                            wError = L"";
                                            break;
                                        }
                                    }

                                    if (!wError)
                                    {
                                        //open configuration file -------------------------------------
                                        HANDLE hFile = CreateFileW(wArg ? wArg : (FCopyMemoryW(wTemp-9, L".cfg"), wBuf), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                                        if (hFile != INVALID_HANDLE_VALUE)
                                        {
                                            LARGE_INTEGER iFileSize;
                                            if (GetFileSizeEx(hFile, &iFileSize) && iFileSize.HighPart == 0 && iFileSize.LowPart >= 7*sizeof(wchar_t) && iFileSize.LowPart <= 32*1024*1024 && iFileSize.LowPart%sizeof(wchar_t) == 0)
                                            {
                                                //read configuration file -----------------------------
                                                if (wchar_t *const wFile = static_cast<wchar_t*>(HeapAlloc(hProcHeap, HEAP_NO_SERIALIZE, iFileSize.LowPart + sizeof(wchar_t))))
                                                {
                                                    dwTemp = ReadFile(hFile, wFile, iFileSize.LowPart, &dwTemp, nullptr) && dwTemp == iFileSize.LowPart;
                                                    CloseHandle(hFile);
                                                    hFile = INVALID_HANDLE_VALUE;
                                                    if (dwTemp)
                                                    {
                                                        wFile[iFileSize.LowPart/sizeof(wchar_t)] = L'\0';

                                                        //num of lines --------------------------------
                                                        wTemp = wFile;
                                                        szCount = 1;
                                                        while ((wTemp = FStrChrW(wTemp+1, L'\n')))
                                                        {
                                                            *wTemp = L'\0';
                                                            ++szCount;
                                                        }

                                                        if (!(szCount & 1) && szCount <= 0xBFFF/*msdn*/ *2)
                                                        {
                                                            //fill entries ----------------------------
                                                            if (BYTE *pKeys = static_cast<BYTE*>(HeapAlloc(hProcHeap, HEAP_NO_SERIALIZE, szCount*sizeof(BYTE))))
                                                            {
                                                                szCount /= 2;
                                                                if (SEntries *const sEntries = static_cast<SEntries*>(HeapAlloc(hProcHeap, HEAP_NO_SERIALIZE, szCount*sizeof(SEntries))))
                                                                {
                                                                    wTemp = wFile;
                                                                    for (size_t i = 0, szMod, szVk; i < szCount; ++i, wTemp = FStrChrW(wTemp, L'\0')+1)
                                                                    {
                                                                        szMod = *wTemp - L'a';
                                                                        if (szMod > (MOD_WIN | MOD_CONTROL | MOD_ALT | MOD_SHIFT))
                                                                        {
                                                                            wError = L"Incorrect modifiers";
                                                                            break;
                                                                        }

                                                                        ++wTemp;
                                                                        szVk = *wTemp ? ((wTemp[0] - L'a') << 4 | (wTemp[1] - L'a')) : 0;
                                                                        switch (szVk)
                                                                        {
                                                                        case 0:           case VK_PACKET:
                                                                        case VK_LWIN:     case VK_RWIN:
                                                                        case VK_LCONTROL: case VK_RCONTROL: case VK_CONTROL:
                                                                        case VK_LMENU:    case VK_RMENU:    case VK_MENU:
                                                                        case VK_LSHIFT:   case VK_RSHIFT:   case VK_SHIFT:
                                                                            szVk = 0xFF;
                                                                        }
                                                                        if (szVk >= 0xFF)
                                                                        {
                                                                            wError = L"Incorrect virtual key";
                                                                            break;
                                                                        }
                                                                        ___assert___(wTemp[1] && wTemp[2]);

                                                                        //check repeats
                                                                        for (szIndex = 0; szIndex < i; ++szIndex)
                                                                            if (pKeys[szIndex+szCount] == szVk && pKeys[szIndex] == szMod)
                                                                            {
                                                                                wError = L"Key combination met twice";
                                                                                break;
                                                                            }
                                                                        if (wError)
                                                                            break;

                                                                        wTemp += 2;
                                                                        SEntries &refEntry = sEntries[i];
                                                                        if (*wTemp == (L'0' + g_iUnusedCmdForMsg))        //send message
                                                                        {
                                                                            refEntry.FMsg = nullptr;
                                                                            for (szIndex = 0; szIndex < szCountDlls; ++szIndex)
                                                                                if (FCompareMemoryW(wNames[szIndex], wTemp+1))
                                                                                {
                                                                                    refEntry.FMsg = fMsgs[szIndex];
                                                                                    break;
                                                                                }
                                                                            if (!refEntry.FMsg)
                                                                            {
                                                                                wError = L"Some plugins not found";
                                                                                break;
                                                                            }
                                                                        }
                                                                        else if (*wTemp >= (L'0' + SW_HIDE) && *wTemp <= (L'0' + SW_SHOWMINNOACTIVE))        //run process
                                                                        {
                                                                            if (!wTemp[1])
                                                                            {
                                                                                wError = L"Command line can't be empty";
                                                                                break;
                                                                            }
                                                                            refEntry.wStrCmdLine = wTemp+1;
                                                                        }
                                                                        else
                                                                        {
                                                                            wError = L"Incorrect show command";
                                                                            break;
                                                                        }

                                                                        refEntry.iShowCmd = *wTemp - L'0';
                                                                        ___assert___(refEntry.iShowCmd <= 7);

                                                                        wTemp = FStrChrW(wTemp, L'\0')+1;
                                                                        if (*wTemp == L'\0')
                                                                        {
                                                                            wError = L"Message or workdir can't be empty";
                                                                            break;
                                                                        }

                                                                        pKeys[i] = szMod;
                                                                        pKeys[i+szCount] = szVk;
                                                                        refEntry.wStrMessage /*= refEntry.pStrWorkDir*/ = wTemp;
                                                                    }

                                                                    if (!wError)
                                                                    {
                                                                        //register COM if needed-------
                                                                        if (!bNeedCom || SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
                                                                        {
                                                                            //create window -----------
                                                                            WNDCLASSEX wndCl;
                                                                            FZeroMemory(&wndCl, sizeof(WNDCLASSEX));
                                                                            wndCl.cbSize = sizeof(WNDCLASSEX);
                                                                            wndCl.lpfnWndProc = WindowProc;
                                                                            wndCl.lpszClassName = g_wGuidClass;
                                                                            if (RegisterClassExW(&wndCl))
                                                                            {
                                                                                //register hotkeys-----
                                                                                if (bool *const pbIsRegister = static_cast<bool*>(HeapAlloc(hProcHeap, HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY, szCount*sizeof(bool))))
                                                                                {
                                                                                    //structures for CreateProcess
                                                                                    PROCESS_INFORMATION pi;
                                                                                    STARTUPINFO si;
                                                                                    SCreateParams tgCreateParams;
                                                                                    tgCreateParams.pSEntries = sEntries;
                                                                                    tgCreateParams.pPi = &pi;
                                                                                    tgCreateParams.pSi = &si;
                                                                                    tgCreateParams.pbIsRegister = pbIsRegister;
                                                                                    tgCreateParams.szCount = szCount;
                                                                                    if (const HWND hWnd = CreateWindowExW(0, g_wGuidClass, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, &tgCreateParams))
                                                                                    {
                                                                                        FZeroMemory(&si, sizeof(STARTUPINFO));
                                                                                        si.cb = sizeof(STARTUPINFO);

                                                                                        for (szIndex = 0; szIndex < szCountDlls; ++szIndex)
                                                                                            HeapFree(hProcHeap, HEAP_NO_SERIALIZE, wNames[szIndex]);
                                                                                        HeapFree(hProcHeap, HEAP_NO_SERIALIZE, wNames);
                                                                                        wNames = nullptr;

                                                                                        HeapFree(hProcHeap, HEAP_NO_SERIALIZE, fMsgs);
                                                                                        fMsgs = nullptr;

                                                                                        for (szIndex = 0; szIndex < szCount; ++szIndex)
                                                                                            pbIsRegister[szIndex] = RegisterHotKey(hWnd, szIndex, pKeys[szIndex], pKeys[szIndex+szCount]);
                                                                                        HeapFree(hProcHeap, HEAP_NO_SERIALIZE, pKeys);
                                                                                        pKeys = nullptr;

                                                                                        //event loop --
                                                                                        MSG msg;
                                                                                        while (GetMessageW(&msg, nullptr, 0, 0) > 0)
                                                                                            DispatchMessageW(&msg);
                                                                                    }
                                                                                    else
                                                                                        wError = L"Create window failed";
                                                                                    HeapFree(hProcHeap, HEAP_NO_SERIALIZE, pbIsRegister);
                                                                                }
                                                                                else
                                                                                    wError = L"Allocation memory error";
                                                                                UnregisterClassW(g_wGuidClass, GetModuleHandleW(nullptr));
                                                                            }
                                                                            else
                                                                                wError = L"RegisterClass failed";
                                                                            if (bNeedCom)
                                                                                CoUninitialize();
                                                                        }
                                                                        else
                                                                            wError = L"CoInitializeEx failed";
                                                                    }
                                                                    HeapFree(hProcHeap, HEAP_NO_SERIALIZE, sEntries);
                                                                }
                                                                else
                                                                    wError = L"Allocation memory error";
                                                                HeapFree(hProcHeap, HEAP_NO_SERIALIZE, pKeys);
                                                            }
                                                            else
                                                                wError = L"Allocation memory error";
                                                        }
                                                        else
                                                            wError = L"Incorrect number of rows";
                                                    }
                                                    else
                                                        wError = L"Error reading file";
                                                    HeapFree(hProcHeap, HEAP_NO_SERIALIZE, wFile);
                                                }
                                                else
                                                    wError = L"Allocation memory error";
                                            }
                                            else
                                                wError = L"Incorrect file size";
                                            if (hFile != INVALID_HANDLE_VALUE)
                                                CloseHandle(hFile);
                                        }
                                        else
                                            wError = L"Error opening file";
                                    }
                                    for (szIndex = 0; szIndex < szCountDlls; ++szIndex)
                                        if (pModules[szIndex])
                                            FreeLibrary(pModules[szIndex]);
                                    HeapFree(hProcHeap, HEAP_NO_SERIALIZE, pModules);
                                }
                                else
                                    wError = L"Allocation memory error";
                            }
                            else
                                wError = L"Find plugins failed";
                        }
                        if (wNames)
                        {
                            for (szIndex = 0; szIndex < szCountDlls; ++szIndex)
                                if (wNames[szIndex])
                                    HeapFree(hProcHeap, HEAP_NO_SERIALIZE, wNames[szIndex]);
                            HeapFree(hProcHeap, HEAP_NO_SERIALIZE, wNames);
                        }
                    }
                    else
                        wError = L"Allocation memory error";
                    HeapFree(hProcHeap, HEAP_NO_SERIALIZE, fMsgs);
                }
                else
                    wError = L"Allocation memory error";
            }
            else
                wError = L"GetProcessHeap failed";
        }
        else
            wError = L"Get app name failed";
        if (wError)
            MessageBoxW(nullptr, *wError == L'\0' ? wBuf : wError, L"HotKeys", MB_ICONERROR);
    }
}

//-------------------------------------------------------------------------------------------------
extern "C" int start()
{
    FMain();
    ExitProcess(0);
    return 0;
}
