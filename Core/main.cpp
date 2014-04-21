#include <cstdio>
#include <windows.h>
#include <userenv.h>
#include <cassert>

#define GUID_CLASS L"b6c9d088-c4a5-45f2-8481-87b29bcaec50"
#define DEFAULT_DLLS 16
#define LINES_PER_RULE 8

typedef int (*PfInit)(const bool bInit);
typedef void (*PfMsg)(const wchar_t *wMsg);

//-------------------------------------------------------------------------------------------------
class ClsString
{
public:
    explicit ClsString(const wchar_t *const pStr) :
        str(new wchar_t[wcslen(pStr)+1])
    {
        wcscpy(str, pStr);
    }
    ClsString(const wchar_t *const pStr1, const wchar_t *const pStr2)
    {
        const size_t sz1 = wcslen(pStr1);
        wcscpy(wcscpy((str = new wchar_t[sz1 + wcslen(pStr2) + 1]), pStr1) + sz1, pStr2);
    }
    ~ClsString()
    {
        delete[] str;
    }
    void operator=(const wchar_t *const pStr)
    {
        delete[] str;
        wcscpy((str = new wchar_t[wcslen(pStr)+1]), pStr);
    }
    wchar_t *str;

private:
    explicit ClsString(const ClsString &);
    void operator=(const ClsString &);
};

//-------------------------------------------------------------------------------------------------
class ClsVector
{
public:
    explicit ClsVector(const size_t iSize) :
        str(new wchar_t*[iSize]),
        size(0),
        reserve(iSize)
    {
        assert(reserve > 0);
    }
    ~ClsVector()
    {
        for (size_t i = 0; i < size; ++i)
            delete[] str[i];
        delete[] str;
    }
    void push_back(const wchar_t *const pStr)
    {
        if (size >= reserve)
        {
            reserve *= 2;
            wchar_t **strNew = new wchar_t*[reserve];
            for (size_t i = 0; i < size; ++i)
            {
                wcscpy((strNew[i] = new wchar_t[wcslen(str[i])+1]), str[i]);
                delete[] str[i];
            }
            delete[] str;
            str = strNew;
        }
        wcscpy((str[size++] = new wchar_t[wcslen(pStr)+1]), pStr);
    }
    void push_back(const wchar_t *const pStr, const size_t iSize)
    {
        assert(size < reserve);
        wchar_t *const pNew = (str[size++] = new wchar_t[iSize+1]);
        wcsncpy(pNew, pStr, iSize);
        pNew[iSize] = L'\0';
    }
    wchar_t **str;
    size_t size;

private:
    explicit ClsVector(const ClsVector &);
    void operator=(const ClsVector &);
    size_t reserve;
};

//-------------------------------------------------------------------------------------------------
class ClsDll
{
public:
    struct TagDll
    {
        HMODULE hMod;
        PfInit fInit;
        PfMsg fMsg;
    };

    ClsDll() :
        pArr(0),
        size(0),
        bInitAll(false)
    {

    }
    ~ClsDll()
    {
        if (bInitAll)
            for (size_t i = 0; i < size; ++i)
            {
                pArr[i].fInit(false);
                FreeLibrary(pArr[i].hMod);
            }
        delete[] pArr;
    }
    void resize(const size_t iSize)
    {
        pArr = new TagDll[iSize];
        size = iSize;
    }
    TagDll *pArr;
    size_t size;
    bool bInitAll;

private:
    explicit ClsDll(const ClsDll &);
    void operator=(const ClsDll &);
};

//-------------------------------------------------------------------------------------------------
class ClsEntries
{
public:
    struct TagEntry
    {
        unsigned int iModifiers,
        iVirtualKey;
        PfMsg fMsg;
        union
        {
            wchar_t *pStrMessage,
            *pStrFile;
        };
        wchar_t *pStrParam,
        *pStrWorkDir;
        int iShowCmd;
    };

    ClsEntries() :
        pArr(0),
        size(0)
    {

    }
    ~ClsEntries()
    {
        for (size_t i = 0; i < size; ++i)
        {
            delete[] pArr[i].pStrMessage;
            delete[] pArr[i].pStrParam;
            delete[] pArr[i].pStrWorkDir;
        }
        delete[] pArr;
    }
    void resize(const size_t iSize)
    {
        memset(pArr = new TagEntry[iSize], 0, sizeof(TagEntry)*iSize);
        size = iSize;
    }
    TagEntry *pArr;
    size_t size;

private:
    explicit ClsEntries(const ClsEntries &);
    void operator=(const ClsEntries &);
};

//-------------------------------------------------------------------------------------------------
static inline bool fOk(const wchar_t *const wOk)
{
    return !(*wOk || errno);
}

//-------------------------------------------------------------------------------------------------
static inline bool fVkOk(const unsigned int iVk)
{
    switch(iVk)
    {
    case 0x00: case VK_PACKET:
    case VK_LWIN: case VK_RWIN:
    case VK_LCONTROL: case VK_RCONTROL: case VK_CONTROL:
    case VK_LMENU: case VK_RMENU: case VK_MENU:
    case VK_LSHIFT: case VK_RSHIFT: case VK_SHIFT:
        return false;
    default: return (iVk < 0xFF);
    }
}

//-------------------------------------------------------------------------------------------------
static const wchar_t* fModToString(const unsigned int iMod)
{
    switch(iMod)
    {
    case 0x1: return L"Alt";
    case 0x2: return L"Ctrl";
    case 0x3: return L"Ctrl+Alt";
    case 0x4: return L"Shift";
    case 0x5: return L"Alt+Shift";
    case 0x6: return L"Ctrl+Shift";
    case 0x7: return L"Ctrl+Alt+Shift";
    case 0x8: return L"Win";
    case 0x9: return L"Win+Alt";
    case 0xA: return L"Win+Ctrl";
    case 0xB: return L"Win+Ctrl+Alt";
    case 0xC: return L"Win+Shift";
    case 0xD: return L"Win+Alt+Shift";
    case 0xE: return L"Win+Ctrl+Shift";
    case 0xF: return L"Win+Ctrl+Alt+Shift";
    default : return L"?";
    }
}

//-------------------------------------------------------------------------------------------------
static const wchar_t* fKeyToString(const unsigned int iKey)
{
    switch (iKey)
    {
    case 0x01: return L"LeftButton";
    case 0x02: return L"RightButton";
    case 0x03: return L"Control-Break";
    case 0x04: return L"MiddleButton";
    case 0x05: return L"XButton1";
    case 0x06: return L"XButton2";
    case 0x07: return L"0x07";
    case 0x08: return L"BackSpace";
    case 0x09: return L"Tab";
    case 0x0A: return L"0x0A";
    case 0x0B: return L"0x0B";
    case 0x0C: return L"Clear";
    case 0x0D: return L"Enter";
    case 0x0E: return L"0x0E";
    case 0x0F: return L"0x0F";
        //case 0x10: return L"Shift";        //modifier
        //case 0x11: return L"Ctrl";        //modifier
        //case 0x12: return L"Alt";        //modifier
    case 0x13: return L"Pause";
    case 0x14: return L"CapsLock";
    case 0x15: return L"Kana";
    case 0x16: return L"0x16";
    case 0x17: return L"Junja";
    case 0x18: return L"Final";
    case 0x19: return L"Hanja";
    case 0x1A: return L"0x1A";
    case 0x1B: return L"Esc";
    case 0x1C: return L"Convert";
    case 0x1D: return L"NonConvert";
    case 0x1E: return L"Accept";
    case 0x1F: return L"ModeChange";
    case 0x20: return L"Space";
    case 0x21: return L"PgUp";
    case 0x22: return L"PgDn";
    case 0x23: return L"End";
    case 0x24: return L"Home";
    case 0x25: return L"Left";
    case 0x26: return L"Up";
    case 0x27: return L"Right";
    case 0x28: return L"Down";
    case 0x29: return L"Select";
    case 0x2A: return L"Print";
    case 0x2B: return L"Execute";
    case 0x2C: return L"PrtScr";
    case 0x2D: return L"Ins";
    case 0x2E: return L"Del";
    case 0x2F: return L"Help";
    case 0x30: return L"0";
    case 0x31: return L"1";
    case 0x32: return L"2";
    case 0x33: return L"3";
    case 0x34: return L"4";
    case 0x35: return L"5";
    case 0x36: return L"6";
    case 0x37: return L"7";
    case 0x38: return L"8";
    case 0x39: return L"9";
    case 0x3A: return L"0x3A";
    case 0x3B: return L"0x3B";
    case 0x3C: return L"0x3C";
    case 0x3D: return L"0x3D";
    case 0x3E: return L"0x3E";
    case 0x3F: return L"0x3F";
    case 0x40: return L"0x40";
    case 0x41: return L"A";
    case 0x42: return L"B";
    case 0x43: return L"C";
    case 0x44: return L"D";
    case 0x45: return L"E";
    case 0x46: return L"F";
    case 0x47: return L"G";
    case 0x48: return L"H";
    case 0x49: return L"I";
    case 0x4A: return L"J";
    case 0x4B: return L"K";
    case 0x4C: return L"L";
    case 0x4D: return L"M";
    case 0x4E: return L"N";
    case 0x4F: return L"O";
    case 0x50: return L"P";
    case 0x51: return L"Q";
    case 0x52: return L"R";
    case 0x53: return L"S";
    case 0x54: return L"T";
    case 0x55: return L"U";
    case 0x56: return L"V";
    case 0x57: return L"W";
    case 0x58: return L"X";
    case 0x59: return L"Y";
    case 0x5A: return L"Z";
        //case 0x5B: return L"LWin";        //modifier
        //case 0x5C: return L"RWin";        //modifier
    case 0x5D: return L"Apps";
    case 0x5E: return L"0x5E";
    case 0x5F: return L"Sleep";
    case 0x60: return L"Num0";
    case 0x61: return L"Num1";
    case 0x62: return L"Num2";
    case 0x63: return L"Num3";
    case 0x64: return L"Num4";
    case 0x65: return L"Num5";
    case 0x66: return L"Num6";
    case 0x67: return L"Num7";
    case 0x68: return L"Num8";
    case 0x69: return L"Num9";
    case 0x6A: return L"Num*";
    case 0x6B: return L"Num+";
    case 0x6C: return L"Separator";
    case 0x6D: return L"Num-";
    case 0x6E: return L"Num.";
    case 0x6F: return L"Num/";
    case 0x70: return L"F1";
    case 0x71: return L"F2";
    case 0x72: return L"F3";
    case 0x73: return L"F4";
    case 0x74: return L"F5";
    case 0x75: return L"F6";
    case 0x76: return L"F7";
    case 0x77: return L"F8";
    case 0x78: return L"F9";
    case 0x79: return L"F10";
    case 0x7A: return L"F11";
    case 0x7B: return L"F12";
    case 0x7C: return L"F13";
    case 0x7D: return L"F14";
    case 0x7E: return L"F15";
    case 0x7F: return L"F16";
    case 0x80: return L"F17";
    case 0x81: return L"F18";
    case 0x82: return L"F19";
    case 0x83: return L"F20";
    case 0x84: return L"F21";
    case 0x85: return L"F22";
    case 0x86: return L"F23";
    case 0x87: return L"F24";
    case 0x88: return L"0x88";
    case 0x89: return L"0x89";
    case 0x8A: return L"0x8A";
    case 0x8B: return L"0x8B";
    case 0x8C: return L"0x8C";
    case 0x8D: return L"0x8D";
    case 0x8E: return L"0x8E";
    case 0x8F: return L"0x8F";
    case 0x90: return L"NumLock";
    case 0x91: return L"ScrollLock";
    case 0x92: return L"0x92";
    case 0x93: return L"0x93";
    case 0x94: return L"0x94";
    case 0x95: return L"0x95";
    case 0x96: return L"0x96";
    case 0x97: return L"0x97";
    case 0x98: return L"0x98";
    case 0x99: return L"0x99";
    case 0x9A: return L"0x9A";
    case 0x9B: return L"0x9B";
    case 0x9C: return L"0x9C";
    case 0x9D: return L"0x9D";
    case 0x9E: return L"0x9E";
    case 0x9F: return L"0x9F";
        //case 0xA0: return L"LShift";        //modifier
        //case 0xA1: return L"RShift";        //modifier
        //case 0xA2: return L"LCtrl";        //modifier
        //case 0xA3: return L"RCtrl";        //modifier
        //case 0xA4: return L"LAlt";        //modifier
        //case 0xA5: return L"RAlt";        //modifier
    case 0xA6: return L"BrowserBack";
    case 0xA7: return L"BrowserForward";
    case 0xA8: return L"BrowserRefresh";
    case 0xA9: return L"BrowserStop";
    case 0xAA: return L"BrowserSearch";
    case 0xAB: return L"BrowserFavorites";
    case 0xAC: return L"BrowserHome";
    case 0xAD: return L"VolumeMute";
    case 0xAE: return L"VolumeDown";
    case 0xAF: return L"VolumeUp";
    case 0xB0: return L"MediaNext";
    case 0xB1: return L"MediaPrev";
    case 0xB2: return L"MediaStop";
    case 0xB3: return L"MediaPlayPause";
    case 0xB4: return L"StartMail";
    case 0xB5: return L"MediaSelect";
    case 0xB6: return L"StartApp1";
    case 0xB7: return L"StartApp2";
    case 0xB8: return L"0xB8";
    case 0xB9: return L"0xB9";
    case 0xBA: return L";";
    case 0xBB: return L"=";
    case 0xBC: return L",";
    case 0xBD: return L"-";
    case 0xBE: return L".";
    case 0xBF: return L"/";
    case 0xC0: return L"`";
    case 0xC1: return L"0xC1";
    case 0xC2: return L"0xC2";
    case 0xC3: return L"0xC3";
    case 0xC4: return L"0xC4";
    case 0xC5: return L"0xC5";
    case 0xC6: return L"0xC6";
    case 0xC7: return L"0xC7";
    case 0xC8: return L"0xC8";
    case 0xC9: return L"0xC9";
    case 0xCA: return L"0xCA";
    case 0xCB: return L"0xCB";
    case 0xCC: return L"0xCC";
    case 0xCD: return L"0xCD";
    case 0xCE: return L"0xCE";
    case 0xCF: return L"0xCF";
    case 0xD0: return L"0xD0";
    case 0xD1: return L"0xD1";
    case 0xD2: return L"0xD2";
    case 0xD3: return L"0xD3";
    case 0xD4: return L"0xD4";
    case 0xD5: return L"0xD5";
    case 0xD6: return L"0xD6";
    case 0xD7: return L"0xD7";
    case 0xD8: return L"0xD8";
    case 0xD9: return L"0xD9";
    case 0xDA: return L"0xDA";
    case 0xDB: return L"[";
    case 0xDC: return L"\\";
    case 0xDD: return L"]";
    case 0xDE: return L"'";
    case 0xDF: return L"0xDF";
    case 0xE0: return L"0xE0";
    case 0xE1: return L"0xE1";
    case 0xE2: return L"0xE2";
    case 0xE3: return L"0xE3";
    case 0xE4: return L"0xE4";
    case 0xE5: return L"Process";
    case 0xE6: return L"0xE6";
        //case 0xE7: return L"Packet";        //used to pass unicode characters
    case 0xE8: return L"0xE8";
    case 0xE9: return L"0xE9";
    case 0xEA: return L"0xEA";
    case 0xEB: return L"0xEB";
    case 0xEC: return L"0xEC";
    case 0xED: return L"0xED";
    case 0xEE: return L"0xEE";
    case 0xEF: return L"0xEF";
    case 0xF0: return L"0xF0";
    case 0xF1: return L"0xF1";
    case 0xF2: return L"0xF2";
    case 0xF3: return L"0xF3";
    case 0xF4: return L"0xF4";
    case 0xF5: return L"0xF5";
    case 0xF6: return L"Attn";
    case 0xF7: return L"CrSel";
    case 0xF8: return L"ExSel";
    case 0xF9: return L"ErEOF";
    case 0xFA: return L"Play";
    case 0xFB: return L"Zoom";
    case 0xFC: return L"0xFC";
    case 0xFD: return L"PA1";
    case 0xFE: return L"OemClear";
    default  : return L"?";
    }
}

//-------------------------------------------------------------------------------------------------
static ClsEntries g_clEntries;

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_HOTKEY:
    {
        const ClsEntries::TagEntry &tgEntry = g_clEntries.pArr[wParam];
        if (tgEntry.fMsg)
            tgEntry.fMsg(tgEntry.pStrMessage);
        else
            ShellExecute(0, L"open",
                         tgEntry.pStrFile,
                         tgEntry.pStrParam,
                         tgEntry.pStrWorkDir,
                         tgEntry.iShowCmd);
        return 0;
    }

    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
int main()
{
    assert(static_cast<unsigned int>(~0ULL) == 4294967295ULL && static_cast<unsigned int>(~0ULL) == ULONG_MAX);
    assert(sizeof(int) == sizeof(long));

    HWND hWndApp = 0;
    ClsDll clDll;

    {
        while ((hWndApp = FindWindowEx(HWND_MESSAGE, hWndApp, GUID_CLASS, 0)))
            PostMessage(hWndApp, WM_CLOSE, 0, 0);

        int iTemp;
        wchar_t **const argv = CommandLineToArgvW(GetCommandLine(), &iTemp);
        if (!argv)
        {
            MessageBox(0, L"Error: CommandLineToArgvW", L"HotKeys", MB_ICONERROR);
            return 0;
        }
        ClsString strFileName(L"");
        if (iTemp > 1)
        {
            if (wcscmp(argv[1], L"/q") == 0)
            {
                LocalFree(argv);
                return 0;
            }
            strFileName = argv[1];
        }
        const wchar_t *const pAppName = wcsrchr(*argv, L'\\');
        ClsString strAppName(pAppName ? (pAppName+1) : *argv);
        LocalFree(argv);
        size_t iCount = wcslen(strAppName.str);
        if (!(iCount > 4 && wcscmp(strAppName.str + iCount - 4, L".exe") == 0))
        {
            MessageBox(0, L"Error: Application name", L"HotKeys", MB_ICONERROR);
            return 0;
        }

        ClsString strUserPath(L"");
        HANDLE hToken;
        DWORD dwSizeUserPath = 0;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        {
            if (!GetUserProfileDirectory(hToken, 0, &dwSizeUserPath) && dwSizeUserPath)
            {
                delete[] strUserPath.str;
                strUserPath.str = new wchar_t[dwSizeUserPath];
                if (!GetUserProfileDirectory(hToken, strUserPath.str, &dwSizeUserPath))
                    strUserPath = L"";
            }
            CloseHandle(hToken);
        }
        if (*strUserPath.str == L'\0')
        {
            MessageBox(0, L"Error: OpenProcessToken/GetUserProfileDirectory", strAppName.str, MB_ICONERROR);
            return 0;
        }

        if (*strFileName.str == L'\0')
        {
            strFileName = strAppName.str;
            wcscpy(strFileName.str + iCount - 3, L"cfg");
        }
        wchar_t *const pSuffix = strAppName.str + iCount - 4;
        *pSuffix = L'\0';

        ClsVector vectPluginNames(DEFAULT_DLLS);
        WIN32_FIND_DATA findFileData;
        const HANDLE hFind = FindFirstFile(ClsString(strAppName.str, L"_plugins\\*.dll").str, &findFileData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    vectPluginNames.push_back(findFileData.cFileName);
            } while(FindNextFile(hFind, &findFileData));
            FindClose(hFind);
        }

        clDll.resize(vectPluginNames.size);
        const ClsString strPathDll(strAppName.str, L"_plugins\\");
        for (size_t i = 0; i < vectPluginNames.size; ++i)
        {
            ClsDll::TagDll &refDll = clDll.pArr[i];
            if (!(refDll.hMod = LoadLibrary(ClsString(strPathDll.str, vectPluginNames.str[i]).str)))
            {
                for (size_t j = 0; j < i; ++j)
                    FreeLibrary(clDll.pArr[j].hMod);
                MessageBox(0, ClsString(L"Error loading plugin:\n", vectPluginNames.str[i]).str, strAppName.str, MB_ICONERROR);
                return 0;
            }
            if (!(refDll.fInit = reinterpret_cast<PfInit>(GetProcAddress(refDll.hMod, "fInit"))))
            {
                for (size_t j = 0; j <= i; ++j)
                    FreeLibrary(clDll.pArr[j].hMod);
                MessageBox(0, ClsString(L"Can't find import function fInit in:\n", vectPluginNames.str[i]).str, strAppName.str, MB_ICONERROR);
                return 0;
            }
            if (!(refDll.fMsg = reinterpret_cast<PfMsg>(GetProcAddress(refDll.hMod, "fMsg"))))
            {
                for (size_t j = 0; j <= i; ++j)
                    FreeLibrary(clDll.pArr[j].hMod);
                MessageBox(0, ClsString(L"Can't find import function fMsg in:\n", vectPluginNames.str[i]).str, strAppName.str, MB_ICONERROR);
                return 0;
            }
        }
        for (size_t i = 0; i < vectPluginNames.size; ++i)
            if (!clDll.pArr[i].fInit(true))
            {
                size_t j = 0;
                for (; j < i; ++j)
                {
                    clDll.pArr[j].fInit(false);
                    FreeLibrary(clDll.pArr[j].hMod);
                }
                for (; j < vectPluginNames.size; ++j)
                    FreeLibrary(clDll.pArr[j].hMod);
                MessageBox(0, ClsString(L"Fail init plugin:\n", vectPluginNames.str[i]).str, strAppName.str, MB_ICONERROR);
                return 0;
            }
        clDll.bInitAll = true;

        FILE *file = _wfopen(strFileName.str, L"rb");
        if (!file)
        {
            MessageBox(0, ClsString(L"Error opening file:\n", strFileName.str).str, strAppName.str, MB_ICONERROR);
            return 0;
        }

        fseek(file, 0, SEEK_END);
        iTemp = ftell(file);
        if (iTemp < 12)        //[12 - min bytes for one rule]
        {
            fclose(file);
            MessageBox(0, ClsString(L"Error file size:\n", strFileName.str).str, strAppName.str, MB_ICONERROR);
            return 0;
        }
        rewind(file);

        char *const pFileByte = new char[iTemp];
        iCount = fread(pFileByte, 1, iTemp, file);
        fclose(file);
        if (static_cast<size_t>(iTemp) != iCount)
        {
            delete[] pFileByte;
            MessageBox(0, ClsString(L"Error reading file:\n", strFileName.str).str, strAppName.str, MB_ICONERROR);
            return 0;
        }

        const char *const pNewLineN = strchr(pFileByte, '\n');
        const char *const pNewLineR = strchr(pFileByte, '\r');
        if (!(pNewLineN && pNewLineR && pNewLineN-pNewLineR == 1))
        {
            delete[] pFileByte;
            MessageBox(0, ClsString(L"Incorrect end-of-line format:\n", strFileName.str).str, strAppName.str, MB_ICONERROR);
            return 0;
        }

        iTemp = MultiByteToWideChar(CP_UTF8, 0, pFileByte, -1, 0, 0);
        wchar_t *pFileUnicode = new wchar_t[iTemp];
        MultiByteToWideChar(CP_UTF8, 0, pFileByte, -1, pFileUnicode, iTemp);
        delete[] pFileByte;

        const wchar_t *pForLine = pFileUnicode;
        iCount = 1;
        while ((pForLine = wcschr(pForLine+1, L'\n')))
            ++iCount;

        if (!iCount || iCount%LINES_PER_RULE)
        {
            delete[] pFileUnicode;
            MessageBox(0, ClsString(L"Incorrect number of rows rules:\n", strFileName.str).str, strAppName.str, MB_ICONERROR);
            return 0;
        }

        pForLine = pFileUnicode;
        ClsVector vectLines(iCount);
        for (size_t i = 0; i < iCount-1; ++i)
        {
            const wchar_t *const pNewLine = wcschr(pForLine, L'\n');
            vectLines.push_back(pForLine, pNewLine - pForLine - 1);
            pForLine = pNewLine+1;
        }
        delete[] pFileUnicode;
        vectLines.push_back(L"");

        wchar_t wBuf[11];        //[11 = 4294967295]
        g_clEntries.resize(iCount/LINES_PER_RULE);
        for (size_t i = 0; i < iCount; ++i)
        {
            ClsEntries::TagEntry &refEntry = g_clEntries.pArr[i/LINES_PER_RULE];
            wchar_t *wOk;

            refEntry.iModifiers = wcstoul(vectLines.str[i], &wOk, 16);
            if (!(fOk(wOk) && refEntry.iModifiers <= 16))
            {
                MessageBox(0, ClsString(L"Incorrect line: #", _ultow(i+1, wBuf, 10)).str, strAppName.str, MB_ICONERROR);
                return 0;
            }

            refEntry.iVirtualKey = wcstoul(vectLines.str[++i], &wOk, 16);
            if (!(fOk(wOk) && fVkOk(refEntry.iVirtualKey)))
            {
                MessageBox(0, ClsString(L"Incorrect line: #", _ultow(i+1, wBuf, 10)).str, strAppName.str, MB_ICONERROR);
                return 0;
            }

            //check repeats
            for (size_t j = 0; j < i/LINES_PER_RULE; ++j)
                if (g_clEntries.pArr[j].iVirtualKey == refEntry.iVirtualKey && g_clEntries.pArr[j].iModifiers == refEntry.iModifiers)
                {
                    MessageBox(0, ClsString(L"Key combination met twice in line: #", _ultow(i, wBuf, 10)).str, strAppName.str, MB_ICONERROR);
                    return 0;
                }

            if (wcscmp(vectLines.str[++i], L"M") == 0)        //send message
            {
                ++i;
                for (size_t j = 0; j < vectPluginNames.size; ++j)
                    if (wcscmp(vectPluginNames.str[j], vectLines.str[i]) == 0)
                    {
                        refEntry.fMsg = clDll.pArr[j].fMsg;
                        break;
                    }
                if (!refEntry.fMsg)
                {
                    MessageBox(0, ClsString(L"Plugin not found:\n", vectLines.str[i]).str, strAppName.str, MB_ICONERROR);
                    return 0;
                }

                if (*vectLines.str[++i] == L'\0')
                {
                    MessageBox(0, ClsString(L"Empty line: #", _ultow(i+1, wBuf, 10)).str, strAppName.str, MB_ICONERROR);
                    return 0;
                }
                wcscpy(refEntry.pStrMessage = new wchar_t[wcslen(vectLines.str[i])+1], vectLines.str[i]);
                if (!(wcscmp(vectLines.str[++i], L"-") == 0 && wcscmp(vectLines.str[++i], L"-") == 0 && *vectLines.str[++i] == L'\0'))
                {
                    MessageBox(0, ClsString(L"Incorrect line: #", _ultow(i+1, wBuf, 10)).str, strAppName.str, MB_ICONERROR);
                    return 0;
                }
            }
            else if (wcscmp(vectLines.str[i], L"F") == 0)        //Run file
            {
                if (*vectLines.str[++i] == L'\0')
                {
                    MessageBox(0, ClsString(L"Empty line: #", _ultow(i+1, wBuf, 10)).str, strAppName.str, MB_ICONERROR);
                    return 0;
                }
                wcscpy((refEntry.pStrFile = new wchar_t[wcslen(vectLines.str[i])+1]), vectLines.str[i]);

                if (*vectLines.str[++i] != L'\0')
                    wcscpy(refEntry.pStrParam = new wchar_t[wcslen(vectLines.str[i])+1], vectLines.str[i]);

                if (*vectLines.str[++i] == L'\0')
                {
                    const wchar_t *const pStrFile = vectLines.str[i-2];
                    if (const wchar_t *const pDelim = wcsrchr(pStrFile, L'\\'))
                    {
                        const size_t iDistance = pDelim - pStrFile;
                        refEntry.pStrWorkDir = new wchar_t[iDistance+1];
                        wcsncpy(refEntry.pStrWorkDir, pStrFile, iDistance);
                        refEntry.pStrWorkDir[iDistance] = L'\0';
                    }
                    else
                        wcscpy(refEntry.pStrWorkDir = new wchar_t[dwSizeUserPath], strUserPath.str);
                }
                else
                    wcscpy(refEntry.pStrWorkDir = new wchar_t[wcslen(vectLines.str[i])+1], vectLines.str[i]);

                refEntry.iShowCmd = wcstol(vectLines.str[++i], &wOk, 10);
                if (!(fOk(wOk) && refEntry.iShowCmd >= 0 && refEntry.iShowCmd <= 10 && *vectLines.str[++i] == L'\0'))
                {
                    MessageBox(0, ClsString(L"Incorrect line: #", _ultow(i+1, wBuf, 10)).str, strAppName.str, MB_ICONERROR);
                    return 0;
                }
            }
            else
            {
                MessageBox(0, ClsString(L"Incorrect line: #", _ultow(i+1, wBuf, 10)).str, strAppName.str, MB_ICONERROR);
                return 0;
            }
        }

        WNDCLASS wndClass;
        wndClass.style = 0;
        wndClass.lpfnWndProc = WindowProc;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = 0;
        wndClass.hIcon = 0;
        wndClass.hCursor = 0;
        wndClass.hbrBackground = 0;
        wndClass.lpszMenuName = 0;
        wndClass.lpszClassName = GUID_CLASS;

        if (!RegisterClass(&wndClass))
        {
            MessageBox(0, L"Error: RegisterClass", strAppName.str, MB_ICONERROR);
            return 0;
        }

        if (!(hWndApp = CreateWindowEx(0, GUID_CLASS, L"HotKeyWindow", 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0)))
        {
            UnregisterClass(GUID_CLASS, GetModuleHandle(0));
            MessageBox(0, L"Error: CreateWindowEx", strAppName.str, MB_ICONERROR);
            return 0;
        }

        if (CoInitialize(0) != S_OK)
        {
            DestroyWindow(hWndApp);
            UnregisterClass(GUID_CLASS, GetModuleHandle(0));
            MessageBox(0, L"Error: CoInitialize", strAppName.str, MB_ICONERROR);
            return 0;
        }

        wcscpy(pSuffix, L".log");
        if ((file = _wfopen(strAppName.str, L"wt")))
        {
            ClsString strMod(L""),
                    strKey(L"");
            for (size_t i = 0; i < g_clEntries.size; ++i)
                if (!RegisterHotKey(hWndApp, i, g_clEntries.pArr[i].iModifiers, g_clEntries.pArr[i].iVirtualKey))
                {
                    strMod = fModToString(g_clEntries.pArr[i].iModifiers);
                    strKey = fKeyToString(g_clEntries.pArr[i].iVirtualKey);
                    fprintf(file, "%S+%S\n", strMod.str, strKey.str);
                }
            fclose(file);
        }
        else
            for (size_t i = 0; i < g_clEntries.size; ++i)
                RegisterHotKey(hWndApp, i, g_clEntries.pArr[i].iModifiers, g_clEntries.pArr[i].iVirtualKey);
    }

    MSG msg;
    while (GetMessage(&msg, 0, 0, 0))
        DispatchMessage(&msg);

    for (size_t i = 0; i < g_clEntries.size; ++i)
        UnregisterHotKey(hWndApp, i);

    DestroyWindow(hWndApp);
    UnregisterClass(GUID_CLASS, GetModuleHandle(0));
    CoUninitialize();
    return 0;
}
