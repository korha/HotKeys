#include <cstdio>
#include <windows.h>
#include <cassert>

typedef void (*PfMsg)(const wchar_t *wMsg);

static const wchar_t *const g_wGuidClass = L"b6c9d088-c4a5-45f2-8481-87b29bcaec50";

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
                wchar_t *wArg = wCmdLine;
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
TagEntries *tgEntries;
PROCESS_INFORMATION *pPi;
STARTUPINFO *pSi;
bool *bIsRegister = 0;
size_t szCount;

//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
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
        if (bIsRegister)
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
        szCount = GetModuleFileName(0, wBuf, MAX_PATH-9);
        wchar_t *pBuf = wBuf+szCount-4;        //[4 = ".exe"]
        if (szCount >= 8 && szCount < MAX_PATH-14+4 && wcscmp(pBuf, L".exe") == 0)        //[10 - "_plugins\a.dll", 4 - ".exe"]
        {
            //detect plugins ----------------------------------------------------------------------
            wcscpy(pBuf, L"_plugins\\*.dll");
            szCount = 0;
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
                                pBuf += 9;
                                for (szIndex = 0; szIndex < szCount; ++szIndex)
                                {
                                    wcscpy(pBuf, wNames[szIndex]);
                                    if (const HMODULE hMod = LoadLibrary(wBuf))
                                    {
                                        if (!(fMsgs[szIndex] = reinterpret_cast<PfMsg>(GetProcAddress(hMod, "fMsg"))))
                                        {               //[C:\e_plugins\\]
                                            wcscpy(wBuf, L"fMsg failed:\n");
                                            wcscpy(wBuf+13, pBuf);
                                            wError = L"";
                                            break;
                                        }
                                        pModules[szIndex] = hMod;
                                        if (!bNeedCom && GetProcAddress(hMod, "fNeedCom"))
                                            bNeedCom = true;
                                    }
                                    else
                                    {               //[C:\e_plugins\\]
                                        wcscpy(wBuf, L"Load failed:\n");
                                        wcscpy(wBuf+13, pBuf);
                                        wError = L"";
                                        break;
                                    }
                                }

                                if (!wError)
                                {
                                    //open configuration file -------------------------------------
                                    if (FILE *file = _wfopen(wArg ? wArg : (wcscpy(pBuf-9, L".cfg"), wBuf), L"rb"))
                                    {
                                        fseek(file, 0, SEEK_END);
                                        szIndex = ftell(file);
                                        if (szIndex >= 7*sizeof(wchar_t) && szIndex <= 50*1024*1024/*very big*/ && szIndex%sizeof(wchar_t) == 0)        //[7 - "MVVSC\nF"]
                                        {
                                            //read configuration file -----------------------------
                                            if (wchar_t *const wFile = static_cast<wchar_t*>(malloc(szIndex + sizeof(wchar_t))))
                                            {
                                                rewind(file);
                                                pBuf = wFile;
                                                szCount = fread(pBuf, 1, szIndex, file);
                                                fclose(file);
                                                file = 0;
                                                if (szCount == szIndex)
                                                {
                                                    pBuf[szIndex/sizeof(wchar_t)] = L'\0';

                                                    //num of lines --------------------------------
                                                    wchar_t *pForLine = pBuf;
                                                    szCount = 1;
                                                    while ((pForLine = wcschr(pForLine+1, L'\n')))
                                                    {
                                                        *pForLine = L'\0';
                                                        ++szCount;
                                                    }

                                                    if (!(szCount & 1) && szCount <= 0xBFFF/*msdn*/*2)
                                                    {
                                                        //fill entries ----------------------------
                                                        if (BYTE *pKeys = static_cast<BYTE*>(malloc(szCount*sizeof(BYTE))))
                                                        {
                                                            szCount /= 2;
                                                            if ((tgEntries = static_cast<TagEntries*>(malloc(szCount*sizeof(TagEntries)))))
                                                            {
                                                                pForLine = pBuf;
                                                                for (size_t i = 0; i < szCount; ++i, pForLine = wcschr(pForLine, L'\0')+1)
                                                                {
                                                                    TagEntries &refEntry = tgEntries[i];
                                                                    const size_t szMod = *pForLine - L'a';
                                                                    if (szMod > 16)
                                                                    {
                                                                        wError = L"Incorrect modifiers";
                                                                        break;
                                                                    }
                                                                    ++pForLine;

                                                                    size_t szVk = *pForLine ? ((*pForLine - L'a') << 4 | (pForLine[1] - L'a')) : 0;
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
                                                                    assert(pForLine[1] && pForLine[2]);

                                                                    //check repeats
                                                                    for (szIndex = 0; szIndex < i; ++szIndex)
                                                                        if (pKeys[szIndex+szCount] == szVk && pKeys[szIndex] == szMod)
                                                                        {
                                                                            wError = L"Key combination met twice";
                                                                            break;
                                                                        }
                                                                    if (wError)
                                                                        break;

                                                                    pForLine += 2;
                                                                    if (*pForLine == L'5')        //send message
                                                                    {
                                                                        for (szIndex = 0; szIndex < szCountDlls; ++szIndex)
                                                                            if (wcscmp(wNames[szIndex], pForLine+1) == 0)
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
                                                                    else if (*pForLine >= L'0' && *pForLine <= L'7')        //run process
                                                                    {
                                                                        if (!pForLine[1])
                                                                        {
                                                                            wError = L"Command line can't be empty";
                                                                            break;
                                                                        }
                                                                        refEntry.pStrCmdLine = pForLine+1;
                                                                    }
                                                                    else
                                                                    {
                                                                        wError = L"Incorrect show command";
                                                                        break;
                                                                    }

                                                                    refEntry.iShowCmd = *pForLine - L'0';
                                                                    assert(refEntry.iShowCmd <= 7);

                                                                    pForLine = wcschr(pForLine, L'\0')+1;
                                                                    if (*pForLine == L'\0')
                                                                    {
                                                                        wError = L"Message or workdir can't be empty";
                                                                        break;
                                                                    }

                                                                    pKeys[i] = szMod;
                                                                    pKeys[i+szCount] = szVk;
                                                                    refEntry.pStrMessage /*= refEntry.pStrWorkDir*/ = pForLine;
                                                                }

                                                                if (!wError)
                                                                {
                                                                    //create window ---------------
                                                                    WNDCLASS wndCl;
                                                                    memset(&wndCl, 0, sizeof(WNDCLASS));
                                                                    wndCl.lpfnWndProc = WindowProc;
                                                                    wndCl.lpszClassName = g_wGuidClass;
                                                                    if (RegisterClass(&wndCl))
                                                                    {
                                                                        if (const HWND hWnd = CreateWindowEx(0, g_wGuidClass, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0))
                                                                        {
                                                                            //register COM if needed
                                                                            if (!bNeedCom || SUCCEEDED(CoInitializeEx(0, COINIT_APARTMENTTHREADED)))
                                                                            {
                                                                                //register hotkeys
                                                                                if ((bIsRegister = static_cast<bool*>(malloc(szCount*sizeof(bool)))))
                                                                                {
                                                                                    //structures for CreateProcess
                                                                                    PROCESS_INFORMATION pi;
                                                                                    STARTUPINFO si;
                                                                                    si.cb = sizeof(STARTUPINFO);
                                                                                    si.dwFlags = STARTF_USESHOWWINDOW;
                                                                                    pPi = &pi;        //***it's ok
                                                                                    pSi = &si;        //***it's ok
                                                                                    memset(pSi, 0, sizeof(STARTUPINFO));

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

                                                                                    //cleaning ----
                                                                                    free(bIsRegister);
                                                                                }
                                                                                else
                                                                                    wError = L"Allocation memory error";
                                                                                if (bNeedCom)
                                                                                    CoUninitialize();
                                                                            }
                                                                            else
                                                                                wError = L"CoInitializeEx failed";
                                                                        }
                                                                        else
                                                                            wError = L"Create window failed";
                                                                        UnregisterClass(g_wGuidClass, GetModuleHandle(0));
                                                                    }
                                                                    else
                                                                        wError = L"RegisterClass failed";
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
                                        if (file)
                                            fclose(file);
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
