#include <cstdio>
#include <windows.h>
#include <cassert>

typedef void (*PfMsg)(const wchar_t *wMsg);

static const wchar_t *const g_wGuidClass = L"b6c9d088-c4a5-45f2-8481-87b29bcaec50";

//-------------------------------------------------------------------------------------------------
wchar_t* fGetArgument()
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

//-------------------------------------------------------------------------------------------------
class ClsEntries
{
public:
    struct TagEntry
    {
        union
        {
            wchar_t *pStrCmdLine;
            PfMsg fMsg;
        };
        union
        {
            wchar_t *pStrMessage,
            *pStrWorkDir;
        };
        WORD iShowCmd;
    };

    ClsEntries() :
        tgEntry(0)
    {

    }
    inline void fin()        //destructor
    {
        free(tgEntry);
    }
    inline bool resize(const size_t szSize)
    {
        assert(!tgEntry);
        assert(szSize > 0);
        return (tgEntry = static_cast<TagEntry*>(malloc(szSize*sizeof(TagEntry))));
    }
    inline const TagEntry& operator[](const size_t szIndex) const
    {
        assert(tgEntry);
        return tgEntry[szIndex];
    }
    inline TagEntry& ref(const size_t szIndex) const
    {
        assert(tgEntry);
        return tgEntry[szIndex];
    }

private:
    explicit ClsEntries(const ClsEntries &);
    void operator=(const ClsEntries &);
    TagEntry *tgEntry;
};

//-------------------------------------------------------------------------------------------------
class ClsDlls
{
public:
    ClsDlls() :
        pModules(0),
        szSize(0)
    {

    }
    void fin()        //destructor
    {
        for (size_t i = 0; i < szSize; ++i)
            if (pModules[i])
                FreeLibrary(pModules[i]);
        free(pModules);
    }
    bool resize(const size_t szSizeInit)
    {
        assert(!pModules);
        if ((pModules = static_cast<HMODULE*>(malloc(szSizeInit*sizeof(HMODULE)))))
        {
            szSize = szSizeInit;
            memset(pModules, 0, sizeof(HMODULE)*szSizeInit);
            return true;
        }
        return false;
    }
    inline void set(const size_t szIndex, const HMODULE hMod)
    {
        assert(pModules && szSize);
        pModules[szIndex] = hMod;
    }

private:
    explicit ClsDlls(const ClsDlls &);
    void operator=(const ClsDlls &);
    HMODULE *pModules;
    size_t szSize;
};

//-------------------------------------------------------------------------------------------------
class ClsPlugs
{
public:
    explicit ClsPlugs(const size_t szSizeInit) :
        wNames(static_cast<wchar_t**>(malloc(szSizeInit*sizeof(wchar_t*)))),
        fMsgs(static_cast<PfMsg*>(malloc(szSizeInit*sizeof(PfMsg)))),
        szSize(szSizeInit)
    {
        if (wNames)
            memset(wNames, 0, szSizeInit*sizeof(wchar_t*));
    }
    void fin()        //destructor
    {
        for (size_t i = 0; i < szSize; ++i)
            free(wNames[i]);
        free(wNames);
        free(fMsgs);
    }
    bool set(const size_t szIndex, const wchar_t *const wName)
    {
        assert(szIndex < szSize);
        const size_t szLen = wcslen(wName);
        if ((wNames[szIndex] = static_cast<wchar_t*>(malloc((szLen+1)*sizeof(wchar_t)))))
        {
            wcscpy(wNames[szIndex], wName);
            return true;
        }
        return false;
    }
    inline void set(const size_t szIndex, const FARPROC fMsg)
    {
        assert(szIndex < szSize);
        fMsgs[szIndex] = reinterpret_cast<PfMsg>(fMsg);
    }
    inline const wchar_t* getName(const size_t szIndex) const
    {
        assert(szIndex < szSize);
        return wNames[szIndex];
    }
    PfMsg getPfMsg(const wchar_t *const wName) const
    {
        for (size_t i = 0; i < szSize; ++i)
            if (wcscmp(wNames[i], wName) == 0)
                return fMsgs[i];
        return 0;
    }
    inline bool isValid() const
    {
        return (wNames && fMsgs);
    }

private:
    explicit ClsPlugs(const ClsPlugs &);
    void operator=(const ClsPlugs &);
    wchar_t **const wNames;        //***fin() eliminates leaks
    PfMsg *fMsgs;        //***fin() eliminates leaks
    const size_t szSize;
};

//-------------------------------------------------------------------------------------------------
class ClsKeys
{
public:
    explicit ClsKeys(const size_t szSize) :
        p(static_cast<u_char*>(malloc(szSize*sizeof(u_char))))
    {

    }
    inline void fin()        //destructor
    {
        free(p);
    }
    inline void set(const size_t szIndex, const u_char szVal)
    {
        assert(p);
        p[szIndex] = szVal;
    }
    inline u_char operator[](const size_t szIndex) const
    {
        return p[szIndex];
    }
    inline bool isValid() const
    {
        return p;
    }

private:
    explicit ClsKeys(const ClsKeys &);
    void operator=(const ClsKeys &);
    u_char *p;
};

//-------------------------------------------------------------------------------------------------
void Clear(const wchar_t *const wMsg, wchar_t *const wFile, ClsEntries *clsEntries, ClsDlls *clsDlls, ClsPlugs *clsPlugs, ClsKeys *clsKeys)
{
    free(wFile);
    if (clsEntries)
        clsEntries->fin();
    if (clsDlls)
        clsDlls->fin();
    if (clsPlugs)
        clsPlugs->fin();
    if (clsKeys)
        clsKeys->fin();
    if (wMsg)
        MessageBox(0, wMsg, L"HotKeys", MB_ICONERROR);
}

//-------------------------------------------------------------------------------------------------
ClsEntries clsEntries;
PROCESS_INFORMATION *pPi;
STARTUPINFO *pSi;

//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_HOTKEY:
    {
        const ClsEntries::TagEntry &tgEntry = clsEntries[wParam];
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
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
int main()
{
    ClsDlls clsDlls;        //Unload libraries
    wchar_t *wFile = 0;        //Free file content
    size_t szCount;        //Count of rules
    HWND hWnd = 0;        //ID window
    bool bNeedCom = false;        //COM needed
    bool *bIsRegister;        //RegisterHotkey is ok
    {
        //close previous app ----------------------------------------------------------------------
        while ((hWnd = FindWindowEx(HWND_MESSAGE, hWnd, g_wGuidClass, 0)))
            PostMessage(hWnd, WM_CLOSE, 0, 0);

        //get argument ----------------------------------------------------------------------------
        wchar_t *pForLine = fGetArgument();
        if (pForLine && wcscmp(pForLine, L"/q") == 0)
            return 0;

        //get app path ----------------------------------------------------------------------------
        wchar_t wBuf[MAX_PATH];
        szCount = GetModuleFileName(0, wBuf, MAX_PATH-9);
        wchar_t *pBuf = wBuf+szCount-4;        //[4 = ".exe"]
        if (!(szCount >= 8 && szCount < MAX_PATH-10 && wcscmp(pBuf, L".exe") == 0))        //[10 - "_plugins\d.dll"-".exe"]
        {
            Clear(L"Get app name failed", wFile, 0, 0, 0, 0);
            return 0;
        }

        //detect plugins --------------------------------------------------------------------------
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

        ClsPlugs clsPlugs(szCount);
        if (!clsPlugs.isValid())
        {
            Clear(L"Allocation memory error", wFile, 0, 0, 0, 0);
            return 0;
        }
        size_t szIndex = 0;
        hFind = FindFirstFile(wBuf, &findFileData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (szIndex < szCount && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    if (!clsPlugs.set(szIndex++, findFileData.cFileName))
                    {
                        Clear(L"Allocation memory error", wFile, 0, 0, &clsPlugs, 0);
                        return 0;
                    }
            } while (FindNextFile(hFind, &findFileData));
            FindClose(hFind);
        }
        if (szIndex != szCount)
        {
            Clear(L"Find plugins failed", wFile, 0, 0, &clsPlugs, 0);
            return 0;
        }

        //load plugins ----------------------------------------------------------------------------
        pBuf += 9;
        if (!clsDlls.resize(szCount))
        {
            Clear(L"Allocation memory error", wFile, 0, 0, &clsPlugs, 0);
            return 0;
        }
        for (szIndex = 0; szIndex < szCount; ++szIndex)
        {
            wcscpy(pBuf, clsPlugs.getName(szIndex));
            if (const HMODULE hMod = LoadLibrary(wBuf))
            {
                clsDlls.set(szIndex, hMod);
                if (const FARPROC fMsg = GetProcAddress(hMod, "fMsg"))
                    clsPlugs.set(szIndex, fMsg);
                else
                {
                    wcscpy(wBuf, L"Can't find fMsg:\n");
                    wcscpy(wBuf+17, clsPlugs.getName(szIndex));
                    Clear(wBuf, wFile, 0, &clsDlls, &clsPlugs, 0);
                    return 0;
                }
                if (!bNeedCom && GetProcAddress(hMod, "fNeedCom"))
                    bNeedCom = true;
            }
            else
            {
                wcscpy(wBuf, L"Load failed:\n");
                wcscpy(wBuf+13, clsPlugs.getName(szIndex));
                Clear(wBuf, wFile, 0, &clsDlls, &clsPlugs, 0);
                return 0;
            }
        }

        //open configuration file -----------------------------------------------------------------
        FILE *const file = _wfopen(pForLine ? pForLine : (wcscpy(pBuf-9, L".cfg"), wBuf), L"rb");
        if (!file)
        {
            Clear(L"Error opening file", wFile, 0, &clsDlls, &clsPlugs, 0);
            return 0;
        }
        fseek(file, 0, SEEK_END);
        szIndex = ftell(file);
        if (!(szIndex >= 7*sizeof(wchar_t) && szIndex <= 50*1024*1024/*very big*/ && szIndex%sizeof(wchar_t) == 0))        //[7 - "MVVSC\nF"]
        {
            fclose(file);
            Clear(L"Incorrect file size", wFile, 0, &clsDlls, &clsPlugs, 0);
            return 0;
        }

        //read configuration file -----------------------------------------------------------------
        if (!(wFile = static_cast<wchar_t*>(malloc(szIndex + sizeof(wchar_t)))))
        {
            fclose(file);
            Clear(L"Allocation memory error", wFile, 0, &clsDlls, &clsPlugs, 0);
            return 0;
        }
        pBuf = wFile;
        rewind(file);
        szCount = fread(pBuf, 1, szIndex, file);
        fclose(file);
        if (szCount != szIndex)
        {
            Clear(L"Error reading file", wFile, 0, &clsDlls, &clsPlugs, 0);
            return 0;
        }
        pBuf[szIndex/sizeof(wchar_t)] = L'\0';

        //num of lines ----------------------------------------------------------------------------
        pForLine = pBuf;
        szCount = 1;
        while ((pForLine = wcschr(pForLine+1, L'\n')))
        {
            *pForLine = L'\0';
            ++szCount;
        }
        if (szCount & 1 && szCount <= 0xBFFF/*msdn*/)
        {
            Clear(L"Incorrect number of rows", wFile, 0, &clsDlls, &clsPlugs, 0);
            return 0;
        }

        //fill entries ----------------------------------------------------------------------------
        pForLine = pBuf;
        ClsKeys clsKeys(szCount);
        szCount/=2;
        if (!clsKeys.isValid())
        {
            Clear(L"Allocation memory error", wFile, 0, &clsDlls, &clsPlugs, 0);
            return 0;
        }
        if (!clsEntries.resize(szCount))
        {
            Clear(L"Allocation memory error", wFile, 0, &clsDlls, &clsPlugs, &clsKeys);
            return 0;
        }
        for (size_t i = 0; i < szCount; ++i, pForLine = wcschr(pForLine, L'\0')+1)
        {
            ClsEntries::TagEntry &refEntry = clsEntries.ref(i);
            const size_t szMod = *pForLine - L'a';
            if (szMod > 16)
            {
                Clear(L"Incorrect modifiers", wFile, &clsEntries, &clsDlls, &clsPlugs, &clsKeys);
                return 0;
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
                Clear(L"Incorrect virtual key", wFile, &clsEntries, &clsDlls, &clsPlugs, &clsKeys);
                return 0;
            }
            assert(pForLine[1] && pForLine[2]);

            //check repeats
            for (szIndex = 0; szIndex < i; ++szIndex)
                if (clsKeys[szIndex+szCount] == szVk && clsKeys[szIndex] == szMod)
                {
                    Clear(L"Key combination met twice", wFile, &clsEntries, &clsDlls, &clsPlugs, &clsKeys);
                    return 0;
                }

            pForLine += 2;
            if (*pForLine == L'5')        //send message
            {
                if (!(refEntry.fMsg = clsPlugs.getPfMsg(pForLine+1)))
                {
                    Clear(L"Some plugins not found", wFile, &clsEntries, &clsDlls, &clsPlugs, &clsKeys);
                    return 0;
                }
            }
            else if (*pForLine >= L'0' && *pForLine <= L'7')        //run process
            {
                if (!pForLine[1])
                {
                    Clear(L"Command line can't be empty", wFile, &clsEntries, &clsDlls, &clsPlugs, &clsKeys);
                    return 0;
                }
                refEntry.pStrCmdLine = pForLine+1;
            }
            else
            {
                Clear(L"Incorrect show command", wFile, &clsEntries, &clsDlls, &clsPlugs, &clsKeys);
                return 0;
            }

            clsKeys.set(i, szMod);
            clsKeys.set(i+szCount, szVk);

            refEntry.iShowCmd = *pForLine - L'0';
            assert(refEntry.iShowCmd <= 7);

            pForLine = wcschr(pForLine, L'\0')+1;
            if (*pForLine == L'\0')
            {
                Clear(L"Message or workdir can't be empty", wFile, &clsEntries, &clsDlls, &clsPlugs, &clsKeys);
                return 0;
            }

            refEntry.pStrMessage /*= refEntry.pStrWorkDir*/ = pForLine;
        }
        clsPlugs.fin();

        //create window ---------------------------------------------------------------------------
        WNDCLASS wndCl;
        memset(&wndCl, 0, sizeof(WNDCLASS));
        wndCl.lpfnWndProc = WindowProc;
        wndCl.lpszClassName = g_wGuidClass;
        if (!RegisterClass(&wndCl))
        {
            Clear(L"Registration failed", wFile, &clsEntries, &clsDlls, 0, &clsKeys);
            return 0;
        }
        if (!(hWnd = CreateWindowEx(0, g_wGuidClass, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0)))
        {
            UnregisterClass(g_wGuidClass, GetModuleHandle(0));
            Clear(L"Create window failed", wFile, &clsEntries, &clsDlls, 0, &clsKeys);
            return 0;
        }

        //register COM if needed
        if (bNeedCom && !SUCCEEDED(CoInitializeEx(0, COINIT_APARTMENTTHREADED)))
        {
            UnregisterClass(g_wGuidClass, GetModuleHandle(0));
            Clear(L"CoInitializeEx failed", wFile, &clsEntries, &clsDlls, 0, &clsKeys);
            return 0;
        }

        //register hotkeys ------------------------------------------------------------------------
        if ((bIsRegister = static_cast<bool*>(malloc(szCount*sizeof(bool)))))
        {
            for (szIndex = 0; szIndex < szCount; ++szIndex)
                bIsRegister[szCount] = RegisterHotKey(hWnd, szIndex, clsKeys[szIndex], clsKeys[szIndex+szCount]);
            clsKeys.fin();
        }
        else
        {
            UnregisterClass(g_wGuidClass, GetModuleHandle(0));
            Clear(L"Allocation memory error", wFile, &clsEntries, &clsDlls, 0, &clsKeys);
            return 0;
        }
    }

    //structures for CreateProcess
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW;
    pPi = &pi;        //***it's ok
    pSi = &si;        //***it's ok
    memset(pSi, 0, sizeof(STARTUPINFO));


    //event loop ----------------------------------------------------------------------------------
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0) > 0)
        DispatchMessage(&msg);

    //cleaning ------------------------------------------------------------------------------------
    for (size_t i = 0; i < szCount; ++i)
        if (bIsRegister[i])
            UnregisterHotKey(hWnd, i);
    free(bIsRegister);
    UnregisterClass(g_wGuidClass, GetModuleHandle(0));
    Clear(0, wFile, &clsEntries, &clsDlls, 0, 0);
    if (bNeedCom)
        CoUninitialize();
    return 0;
}
