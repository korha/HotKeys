#include <windows.h>
#include <cassert>

typedef void (*PfMsg)(const wchar_t *wMsg);

static const wchar_t *const g_wGuidClass = L"App::b6c9d088-c4a5-45f2-8481-87b29bcaec50";

//-------------------------------------------------------------------------------------------------
const wchar_t* fGetArgument()
{
    if (wchar_t *wCmdLine = GetCommandLine())
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
                        return 0;
                    ++wCmdLine;
                }
                ++wCmdLine;
                if (*wCmdLine != L' ' && *wCmdLine != L'\t')
                    return 0;
            }
            else
                while (*wCmdLine != L' ' && *wCmdLine != L'\t')
                {
                    if (*wCmdLine == L'\0' || *wCmdLine == L'\"')
                        return 0;
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
                            return 0;
                        ++wCmdLine;
                    }
                    if (wCmdLine[1] != L' ' && wCmdLine[1] != L'\t' && wCmdLine[1] != L'\0')
                        return 0;
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
    return 0;
}

//-------------------------------------------------------------------------------------------------
struct TagEntries
{
    union
    {
        wchar_t *pStrCmdLine;
        PfMsg fMsg;
    };
    union
    {
        wchar_t *pStrWorkDir,
        *pStrMessage;
    };
    WORD iShowCmd;
};

//-------------------------------------------------------------------------------------------------
struct TagCreateParams
{
    TagEntries *tgEntries;
    PROCESS_INFORMATION *pPi;
    STARTUPINFO *pSi;
    bool *bIsRegister;
    size_t szCount;
};

//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static const TagEntries *tgEntries;
    static PROCESS_INFORMATION *pPi;
    static STARTUPINFO *pSi;
    static const bool *bIsRegister;
    static size_t szCount;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        const TagCreateParams *const tgCreateParams = static_cast<const TagCreateParams*>(reinterpret_cast<const CREATESTRUCT*>(lParam)->lpCreateParams);
        tgEntries = tgCreateParams->tgEntries;
        pPi = tgCreateParams->pPi;
        pSi = tgCreateParams->pSi;
        bIsRegister = tgCreateParams->bIsRegister;
        szCount = tgCreateParams->szCount;
        return 0;
    }
    case WM_HOTKEY:
    {
        const TagEntries &tgEntry = tgEntries[wParam];
        if (tgEntry.iShowCmd == 5)
            tgEntry.fMsg(tgEntry.pStrMessage);
        else
        {
            assert(tgEntry.iShowCmd <= 7);
            pSi->wShowWindow = tgEntry.iShowCmd;
            if (CreateProcess(0, tgEntry.pStrCmdLine, 0, 0, FALSE, CREATE_UNICODE_ENVIRONMENT, 0, tgEntry.pStrWorkDir, pSi, pPi))
            {
                CloseHandle(pPi->hThread);
                CloseHandle(pPi->hProcess);
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
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
int main()
{
    //close previous app --------------------------------------------------------------------------
    if (const HWND hWnd = FindWindowEx(HWND_MESSAGE, 0, g_wGuidClass, 0))
        PostMessage(hWnd, WM_CLOSE, 0, 0);

    //get argument --------------------------------------------------------------------------------
    const wchar_t *const wArg = fGetArgument();
    if (!(wArg && wcscmp(wArg, L"/q") == 0))
    {
        const wchar_t *wError = 0;
        //get app path ----------------------------------------------------------------------------
        wchar_t wBuf[MAX_PATH];
        DWORD dwTemp = GetModuleFileName(0, wBuf, MAX_PATH+1-14/*_plugins\a.dll*/);
        if (dwTemp >= 4 && dwTemp < MAX_PATH-14)
        {
            //detect plugins ----------------------------------------------------------------------
            wchar_t *pFor = wBuf+dwTemp;
            wcscpy(pFor, L"_plugins\\*.dll");
            size_t szCount = 0;
            WIN32_FIND_DATA findFileData;
            HANDLE hFind = FindFirstFile(wBuf, &findFileData);
            if (hFind != INVALID_HANDLE_VALUE)
            {
                do
                {
                    if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                        ++szCount;
                } while (FindNextFile(hFind, &findFileData));
                FindClose(hFind);
            }

            const size_t szCountDlls = szCount;
            if (PfMsg *fMsgs = static_cast<PfMsg*>(malloc(szCountDlls*sizeof(PfMsg))))
            {
                if (wchar_t **wNames = static_cast<wchar_t**>(malloc(szCountDlls*sizeof(wchar_t*))))
                {
                    memset(wNames, 0, szCountDlls*sizeof(wchar_t*));
                    size_t szIndex = 0;
                    hFind = FindFirstFile(wBuf, &findFileData);
                    if (hFind != INVALID_HANDLE_VALUE)
                    {
                        do
                        {
                            if (szIndex < szCount && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                            {
                                if ((wNames[szIndex] = static_cast<wchar_t*>(malloc((wcslen(findFileData.cFileName)+1)*sizeof(wchar_t)))))
                                    wcscpy(wNames[szIndex], findFileData.cFileName);
                                else
                                {
                                    wError = L"Allocation memory error";
                                    break;
                                }
                                ++szIndex;
                            }
                        } while (FindNextFile(hFind, &findFileData));
                        FindClose(hFind);
                    }

                    if (!wError)
                    {
                        if (szIndex == szCount)
                        {
                            //load plugins --------------------------------------------------------
                            if (HMODULE *const pModules = static_cast<HMODULE*>(malloc(szCountDlls*sizeof(HMODULE))))
                            {
                                memset(pModules, 0, szCountDlls*sizeof(HMODULE));
                                bool bNeedCom = false;
                                pFor += 9/*_plugins\*/;
                                for (szIndex = 0; szIndex < szCount; ++szIndex)
                                {
                                    wcscpy(pFor, wNames[szIndex]);
                                    if (const HMODULE hMod = LoadLibrary(wBuf))
                                    {
                                        pModules[szIndex] = hMod;
                                        if (!(fMsgs[szIndex] = reinterpret_cast<PfMsg>(GetProcAddress(hMod, "fMsg"))))
                                        {               //[C:\a_plugins\\]
                                            wcscpy(wBuf, L"fMsg failed:\n");
                                            wcscpy(wBuf+13, pFor);
                                            wError = L"";
                                            break;
                                        }
                                        if (!bNeedCom && GetProcAddress(hMod, "fNeedCom"))
                                            bNeedCom = true;
                                    }
                                    else
                                    {               //[C:\a_plugins\\]
                                        wcscpy(wBuf, L"Load failed:\n");
                                        wcscpy(wBuf+13, pFor);
                                        wError = L"";
                                        break;
                                    }
                                }

                                if (!wError)
                                {
                                    //open configuration file -------------------------------------
                                    HANDLE hFile = CreateFile(wArg ? wArg : (wcscpy(pFor-9, L".cfg"), wBuf), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                                    if (hFile != INVALID_HANDLE_VALUE)
                                    {
                                        LARGE_INTEGER iFileSize;
                                        if (GetFileSizeEx(hFile, &iFileSize) && iFileSize.HighPart == 0 && iFileSize.LowPart >= 7*sizeof(wchar_t) && iFileSize.LowPart <= 30*1024*1024/*too large*/ && iFileSize.LowPart%sizeof(wchar_t) == 0)
                                        {
                                            //read configuration file -----------------------------
                                            if (wchar_t *const wFile = static_cast<wchar_t*>(malloc(iFileSize.LowPart + sizeof(wchar_t))))
                                            {
                                                dwTemp = ReadFile(hFile, wFile, iFileSize.LowPart, &dwTemp, 0) && dwTemp == iFileSize.LowPart;
                                                CloseHandle(hFile);
                                                hFile = INVALID_HANDLE_VALUE;
                                                if (dwTemp)
                                                {
                                                    wFile[iFileSize.LowPart/sizeof(wchar_t)] = L'\0';

                                                    //num of lines --------------------------------
                                                    pFor = wFile;
                                                    szCount = 1;
                                                    while ((pFor = wcschr(pFor+1, L'\n')))
                                                    {
                                                        *pFor = L'\0';
                                                        ++szCount;
                                                    }

                                                    if (!(szCount & 1) && szCount <= 0xBFFF/*msdn*/ *2)
                                                    {
                                                        //fill entries ----------------------------
                                                        if (BYTE *pKeys = static_cast<BYTE*>(malloc(szCount*sizeof(BYTE))))
                                                        {
                                                            szCount /= 2;
                                                            if (TagEntries *const tgEntries = static_cast<TagEntries*>(malloc(szCount*sizeof(TagEntries))))
                                                            {
                                                                pFor = wFile;
                                                                for (size_t i = 0, szMod, szVk; i < szCount; ++i, pFor = wcschr(pFor, L'\0')+1)
                                                                {
                                                                    szMod = *pFor - L'a';
                                                                    if (szMod > 16)
                                                                    {
                                                                        wError = L"Incorrect modifiers";
                                                                        break;
                                                                    }

                                                                    ++pFor;
                                                                    szVk = *pFor ? ((pFor[0] - L'a') << 4 | (pFor[1] - L'a')) : 0;
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
                                                                    assert(pFor[1] && pFor[2]);

                                                                    //check repeats
                                                                    for (szIndex = 0; szIndex < i; ++szIndex)
                                                                        if (pKeys[szIndex+szCount] == szVk && pKeys[szIndex] == szMod)
                                                                        {
                                                                            wError = L"Key combination met twice";
                                                                            break;
                                                                        }
                                                                    if (wError)
                                                                        break;

                                                                    pFor += 2;
                                                                    TagEntries &refEntry = tgEntries[i];
                                                                    if (*pFor == L'5')        //send message
                                                                    {
                                                                        refEntry.fMsg = 0;
                                                                        for (szIndex = 0; szIndex < szCountDlls; ++szIndex)
                                                                            if (wcscmp(wNames[szIndex], pFor+1) == 0)
                                                                            {
                                                                                refEntry.fMsg = fMsgs[szIndex];
                                                                                break;
                                                                            }
                                                                        if (!refEntry.fMsg)
                                                                        {
                                                                            wError = L"Some plugins not found";
                                                                            break;
                                                                        }
                                                                    }
                                                                    else if (*pFor >= L'0' && *pFor <= L'7')        //run process
                                                                    {
                                                                        if (!pFor[1])
                                                                        {
                                                                            wError = L"Command line can't be empty";
                                                                            break;
                                                                        }
                                                                        refEntry.pStrCmdLine = pFor+1;
                                                                    }
                                                                    else
                                                                    {
                                                                        wError = L"Incorrect show command";
                                                                        break;
                                                                    }

                                                                    refEntry.iShowCmd = *pFor - L'0';
                                                                    assert(refEntry.iShowCmd <= 7);

                                                                    pFor = wcschr(pFor, L'\0')+1;
                                                                    if (*pFor == L'\0')
                                                                    {
                                                                        wError = L"Message or workdir can't be empty";
                                                                        break;
                                                                    }

                                                                    pKeys[i] = szMod;
                                                                    pKeys[i+szCount] = szVk;
                                                                    refEntry.pStrMessage /*= refEntry.pStrWorkDir*/ = pFor;
                                                                }

                                                                if (!wError)
                                                                {
                                                                    //register COM if needed-------
                                                                    if (!bNeedCom || SUCCEEDED(CoInitializeEx(0, COINIT_APARTMENTTHREADED)))
                                                                    {
                                                                        //create window -----------
                                                                        WNDCLASSEX wndCl;
                                                                        memset(&wndCl, 0, sizeof(WNDCLASSEX));
                                                                        wndCl.cbSize = sizeof(WNDCLASSEX);
                                                                        wndCl.lpfnWndProc = WindowProc;
                                                                        wndCl.lpszClassName = g_wGuidClass;
                                                                        if (RegisterClassEx(&wndCl))
                                                                        {
                                                                            //register hotkeys-----
                                                                            if (bool *const bIsRegister = static_cast<bool*>(malloc(szCount*sizeof(bool))))
                                                                            {
                                                                                memset(bIsRegister, 0, szCount*sizeof(bool));

                                                                                //structures for CreateProcess
                                                                                PROCESS_INFORMATION pi;
                                                                                STARTUPINFO si;
                                                                                TagCreateParams tgCreateParams;
                                                                                tgCreateParams.tgEntries = tgEntries;
                                                                                tgCreateParams.pPi = &pi;
                                                                                tgCreateParams.pSi = &si;
                                                                                tgCreateParams.bIsRegister = bIsRegister;
                                                                                tgCreateParams.szCount = szCount;
                                                                                if (const HWND hWnd = CreateWindowEx(0, g_wGuidClass, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, &tgCreateParams))
                                                                                {
                                                                                    memset(&si, 0, sizeof(STARTUPINFO));
                                                                                    si.cb = sizeof(STARTUPINFO);
                                                                                    si.dwFlags = STARTF_USESHOWWINDOW;

                                                                                    for (szIndex = 0; szIndex < szCountDlls; ++szIndex)
                                                                                        free(wNames[szIndex]);
                                                                                    free(wNames);
                                                                                    wNames = 0;

                                                                                    free(fMsgs);
                                                                                    fMsgs = 0;

                                                                                    for (szIndex = 0; szIndex < szCount; ++szIndex)
                                                                                        bIsRegister[szIndex] = RegisterHotKey(hWnd, szIndex, pKeys[szIndex], pKeys[szIndex+szCount]);
                                                                                    free(pKeys);
                                                                                    pKeys = 0;

                                                                                    //event loop --
                                                                                    MSG msg;
                                                                                    while (GetMessage(&msg, 0, 0, 0) > 0)
                                                                                        DispatchMessage(&msg);
                                                                                }
                                                                                else
                                                                                    wError = L"Create window failed";
                                                                                free(bIsRegister);
                                                                            }
                                                                            else
                                                                                wError = L"Allocation memory error";
                                                                            UnregisterClass(g_wGuidClass, GetModuleHandle(0));
                                                                        }
                                                                        else
                                                                            wError = L"RegisterClass failed";
                                                                        if (bNeedCom)
                                                                            CoUninitialize();
                                                                    }
                                                                    else
                                                                        wError = L"CoInitializeEx failed";
                                                                }
                                                                free(tgEntries);
                                                            }
                                                            else
                                                                wError = L"Allocation memory error";
                                                            free(pKeys);
                                                        }
                                                        else
                                                            wError = L"Allocation memory error";
                                                    }
                                                    else
                                                        wError = L"Incorrect number of rows";
                                                }
                                                else
                                                    wError = L"Error reading file";
                                                free(wFile);
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
                                free(pModules);
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
                            free(wNames[szIndex]);
                        free(wNames);
                    }
                }
                else
                    wError = L"Allocation memory error";
                free(fMsgs);
            }
            else
                wError = L"Allocation memory error";
        }
        else
            wError = L"Get app name failed";
        if (wError)
            MessageBox(0, *wError == L'\0' ? wBuf : wError, L"HotKeys", MB_ICONERROR);
    }
    return 0;
}
