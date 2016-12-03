//HotKeys: SendInput
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define EXPORT //__declspec(dllexport)

//-------------------------------------------------------------------------------------------------
static bool FCompareMemoryW(const wchar_t *pBuf1, const wchar_t *pBuf2)
{
    while (*pBuf1 == *pBuf2 && *pBuf2)
        ++pBuf1, ++pBuf2;
    return *pBuf1 == *pBuf2;
}

static const wchar_t * FStrChrW(const wchar_t *pSrc, const wchar_t wChar)
{
    while (*pSrc && *pSrc != wChar)
        ++pSrc;
    return *pSrc == wChar ? pSrc : nullptr;
}

//-------------------------------------------------------------------------------------------------
static WORD FFind(const wchar_t *const wKey)
{
    const wchar_t *wIt = wKey;
    while (*wIt++);
    switch (wIt-wKey-1)
    {
    case 1:
        switch (*wKey)
        {
        case L'0': return L'0';
        case L'1': return L'1';
        case L'2': return L'2';
        case L'3': return L'3';
        case L'4': return L'4';
        case L'5': return L'5';
        case L'6': return L'6';
        case L'7': return L'7';
        case L'8': return L'8';
        case L'9': return L'9';
        case L'A': return L'A';
        case L'B': return L'B';
        case L'C': return L'C';
        case L'D': return L'D';
        case L'E': return L'E';
        case L'F': return L'F';
        case L'G': return L'G';
        case L'H': return L'H';
        case L'I': return L'I';
        case L'J': return L'J';
        case L'K': return L'K';
        case L'L': return L'L';
        case L'M': return L'M';
        case L'N': return L'N';
        case L'O': return L'O';
        case L'P': return L'P';
        case L'Q': return L'Q';
        case L'R': return L'R';
        case L'S': return L'S';
        case L'T': return L'T';
        case L'U': return L'U';
        case L'V': return L'V';
        case L'W': return L'W';
        case L'X': return L'X';
        case L'Y': return L'Y';
        case L'Z': return L'Z';
        case L';': return VK_OEM_1;
        case L'=': return VK_OEM_PLUS;
        case L',': return VK_OEM_COMMA;
        case L'-': return VK_OEM_MINUS;
        case L'.': return VK_OEM_PERIOD;
        case L'/': return VK_OEM_2;
        case L'`': return VK_OEM_3;
        case L'[': return VK_OEM_4;
        case L'\\':return VK_OEM_5;
        case L']': return VK_OEM_6;
        case L'\'':return VK_OEM_7;
        default: break;
        }

    case 2:
        if (*wKey == L'F')
            switch (wKey[1])
            {
            case L'1': return VK_F1;
            case L'2': return VK_F2;
            case L'3': return VK_F3;
            case L'4': return VK_F4;
            case L'5': return VK_F5;
            case L'6': return VK_F6;
            case L'7': return VK_F7;
            case L'8': return VK_F8;
            case L'9': return VK_F9;
            }
        else if (*wKey == L'U' && wKey[1] == L'P')
            return VK_UP;
        break;

    case 3:
        if (*wKey == L'F')
        {
            if (wKey[1] == L'1')
                switch (wKey[2])
                {
                case L'0': return VK_F10;
                case L'1': return VK_F11;
                case L'2': return VK_F12;
                case L'3': return VK_F13;
                case L'4': return VK_F14;
                case L'5': return VK_F15;
                case L'6': return VK_F16;
                case L'7': return VK_F17;
                case L'8': return VK_F18;
                case L'9': return VK_F19;
                }
            else if (wKey[1] == L'2')
                switch (wKey[2])
                {
                case L'0': return VK_F20;
                case L'1': return VK_F21;
                case L'2': return VK_F22;
                case L'3': return VK_F23;
                case L'4': return VK_F24;
                }
        }
        else
        {
            if (FCompareMemoryW(wKey, L"Alt")) return VK_MENU;
            if (FCompareMemoryW(wKey, L"Esc")) return VK_ESCAPE;
            if (FCompareMemoryW(wKey, L"Del")) return VK_DELETE;
            if (FCompareMemoryW(wKey, L"End")) return VK_END;
            if (FCompareMemoryW(wKey, L"Tab")) return VK_TAB;
            if (FCompareMemoryW(wKey, L"Ins")) return VK_INSERT;
            if (FCompareMemoryW(wKey, L"PA1")) return VK_PA1;
        }
        break;

    case 4:
        if (*wKey == L'0')
        {
            if (wKey[1] == L'x')
            {
                int iKey;
                switch (wKey[2])
                {
                case L'0': iKey = 0x0 << 4; break;
                case L'1': iKey = 0x1 << 4; break;
                case L'2': iKey = 0x2 << 4; break;
                case L'3': iKey = 0x3 << 4; break;
                case L'4': iKey = 0x4 << 4; break;
                case L'5': iKey = 0x5 << 4; break;
                case L'6': iKey = 0x6 << 4; break;
                case L'7': iKey = 0x7 << 4; break;
                case L'8': iKey = 0x8 << 4; break;
                case L'9': iKey = 0x9 << 4; break;
                case L'A': iKey = 0xA << 4; break;
                case L'B': iKey = 0xB << 4; break;
                case L'C': iKey = 0xC << 4; break;
                case L'D': iKey = 0xD << 4; break;
                case L'E': iKey = 0xE << 4; break;
                case L'F': iKey = 0xF << 4; break;
                default: return 0;
                }
                switch (wKey[3])
                {
                case L'0': /*iKey |= 0x0;*/ break;
                case L'1': iKey |= 0x1; break;
                case L'2': iKey |= 0x2; break;
                case L'3': iKey |= 0x3; break;
                case L'4': iKey |= 0x4; break;
                case L'5': iKey |= 0x5; break;
                case L'6': iKey |= 0x6; break;
                case L'7': iKey |= 0x7; break;
                case L'8': iKey |= 0x8; break;
                case L'9': iKey |= 0x9; break;
                case L'A': iKey |= 0xA; break;
                case L'B': iKey |= 0xB; break;
                case L'C': iKey |= 0xC; break;
                case L'D': iKey |= 0xD; break;
                case L'E': iKey |= 0xE; break;
                case L'F': iKey |= 0xF; break;
                default: return 0;
                }
                if (iKey > 0 && iKey < 0xFF && iKey != VK_PACKET)
                    return iKey;
            }
        }
        else if (*wKey == L'N')
        {
            if (wKey[1] == L'U' && wKey[2] == L'M')
                switch (wKey[3])
                {
                case L'0': return VK_NUMPAD0;
                case L'1': return VK_NUMPAD1;
                case L'2': return VK_NUMPAD2;
                case L'3': return VK_NUMPAD3;
                case L'4': return VK_NUMPAD4;
                case L'5': return VK_NUMPAD5;
                case L'6': return VK_NUMPAD6;
                case L'7': return VK_NUMPAD7;
                case L'8': return VK_NUMPAD8;
                case L'9': return VK_NUMPAD9;
                case L'*': return VK_MULTIPLY;
                case L'+': return VK_ADD;
                case L'-': return VK_SUBTRACT;
                case L'.': return VK_DECIMAL;
                case L'/': return VK_DIVIDE;
                }
        }
        else
        {
            if (FCompareMemoryW(wKey, L"Ctrl")) return VK_CONTROL;
            if (FCompareMemoryW(wKey, L"Down")) return VK_DOWN;
            if (FCompareMemoryW(wKey, L"Left")) return VK_LEFT;
            if (FCompareMemoryW(wKey, L"PgUp")) return VK_PRIOR;
            if (FCompareMemoryW(wKey, L"PgDn")) return VK_NEXT;
            if (FCompareMemoryW(wKey, L"LAlt")) return VK_LMENU;
            if (FCompareMemoryW(wKey, L"LWin")) return VK_LWIN;
            if (FCompareMemoryW(wKey, L"RAlt")) return VK_RMENU;
            if (FCompareMemoryW(wKey, L"Home")) return VK_HOME;
            if (FCompareMemoryW(wKey, L"RWin")) return VK_RWIN;
            if (FCompareMemoryW(wKey, L"Apps")) return VK_APPS;
            if (FCompareMemoryW(wKey, L"Help")) return VK_HELP;
            if (FCompareMemoryW(wKey, L"Play")) return VK_PLAY;
            if (FCompareMemoryW(wKey, L"Zoom")) return VK_ZOOM;
            if (FCompareMemoryW(wKey, L"Attn")) return VK_ATTN;
            if (FCompareMemoryW(wKey, L"Kana")) return VK_KANA;
        }
        break;

    case 5:
        if (FCompareMemoryW(wKey, L"Shift")) return VK_SHIFT;
        if (FCompareMemoryW(wKey, L"Space")) return VK_SPACE;
        if (FCompareMemoryW(wKey, L"Enter")) return VK_RETURN;
        if (FCompareMemoryW(wKey, L"Right")) return VK_RIGHT;
        if (FCompareMemoryW(wKey, L"LCtrl")) return VK_LCONTROL;
        if (FCompareMemoryW(wKey, L"RCtrl")) return VK_RCONTROL;
        if (FCompareMemoryW(wKey, L"Pause")) return VK_PAUSE;
        if (FCompareMemoryW(wKey, L"Print")) return VK_PRINT;
        if (FCompareMemoryW(wKey, L"Sleep")) return VK_SLEEP;
        if (FCompareMemoryW(wKey, L"Clear")) return VK_CLEAR;
        if (FCompareMemoryW(wKey, L"CrSel")) return VK_CRSEL;
        if (FCompareMemoryW(wKey, L"ExSel")) return VK_EXSEL;
        if (FCompareMemoryW(wKey, L"ErEOF")) return VK_EREOF;
        if (FCompareMemoryW(wKey, L"Final")) return VK_FINAL;
        if (FCompareMemoryW(wKey, L"Junja")) return VK_JUNJA;
        if (FCompareMemoryW(wKey, L"Hanja")) return VK_HANJA;
        break;

    case 6:
        if (FCompareMemoryW(wKey, L"LShift")) return VK_LSHIFT;
        if (FCompareMemoryW(wKey, L"RShift")) return VK_RSHIFT;
        if (FCompareMemoryW(wKey, L"PrtScr")) return VK_SNAPSHOT;
        if (FCompareMemoryW(wKey, L"Accept")) return VK_ACCEPT;
        if (FCompareMemoryW(wKey, L"Select")) return VK_SELECT;
        break;

    case 7:
        if (FCompareMemoryW(wKey, L"NumLock")) return VK_NUMLOCK;
        if (FCompareMemoryW(wKey, L"Convert")) return VK_CONVERT;
        if (FCompareMemoryW(wKey, L"Execute")) return VK_EXECUTE;
        if (FCompareMemoryW(wKey, L"Process")) return VK_PROCESSKEY;
        break;

    case 8:
        if (FCompareMemoryW(wKey, L"XButton1")) return VK_XBUTTON1;
        if (FCompareMemoryW(wKey, L"XButton2")) return VK_XBUTTON2;
        if (FCompareMemoryW(wKey, L"CapsLock")) return VK_CAPITAL;
        if (FCompareMemoryW(wKey, L"VolumeUp")) return VK_VOLUME_UP;
        if (FCompareMemoryW(wKey, L"OemClear")) return VK_OEM_CLEAR;
        break;

    case 9:
        if (FCompareMemoryW(wKey, L"BackSpace")) return VK_BACK;
        if (FCompareMemoryW(wKey, L"MediaNext")) return VK_MEDIA_NEXT_TRACK;
        if (FCompareMemoryW(wKey, L"MediaPrev")) return VK_MEDIA_PREV_TRACK;
        if (FCompareMemoryW(wKey, L"MediaStop")) return VK_MEDIA_STOP;
        if (FCompareMemoryW(wKey, L"Separator")) return VK_SEPARATOR;
        if (FCompareMemoryW(wKey, L"StartApp1")) return VK_LAUNCH_APP1;
        if (FCompareMemoryW(wKey, L"StartApp2")) return VK_LAUNCH_APP2;
        if (FCompareMemoryW(wKey, L"StartMail")) return VK_LAUNCH_MAIL;
        break;

    case 10:
        if (FCompareMemoryW(wKey, L"LeftButton")) return VK_LBUTTON;
        if (FCompareMemoryW(wKey, L"VolumeDown")) return VK_VOLUME_UP;
        if (FCompareMemoryW(wKey, L"VolumeMute")) return VK_VOLUME_MUTE;
        if (FCompareMemoryW(wKey, L"ScrollLock")) return VK_SCROLL;
        if (FCompareMemoryW(wKey, L"NonConvert")) return VK_NONCONVERT;
        if (FCompareMemoryW(wKey, L"ModeChange")) return VK_MODECHANGE;
        break;

    case 11:
        if (FCompareMemoryW(wKey, L"RightButton")) return VK_RBUTTON;
        if (FCompareMemoryW(wKey, L"BrowserBack")) return VK_BROWSER_BACK;
        if (FCompareMemoryW(wKey, L"BrowserStop")) return VK_BROWSER_STOP;
        if (FCompareMemoryW(wKey, L"BrowserHome")) return VK_BROWSER_HOME;
        if (FCompareMemoryW(wKey, L"MediaSelect")) return VK_LAUNCH_MEDIA_SELECT;
        break;

    case 12:
        if (FCompareMemoryW(wKey, L"MiddleButton")) return VK_MBUTTON;
        break;

    case 13:
        if (FCompareMemoryW(wKey, L"Control-Break")) return VK_CANCEL;
        if (FCompareMemoryW(wKey, L"BrowserSearch")) return VK_BROWSER_SEARCH;
        break;

    case 14:
        if (FCompareMemoryW(wKey, L"BrowserForward")) return VK_BROWSER_FORWARD;
        if (FCompareMemoryW(wKey, L"BrowserRefresh")) return VK_BROWSER_REFRESH;
        if (FCompareMemoryW(wKey, L"MediaPlayPause")) return VK_MEDIA_PLAY_PAUSE;
        break;

    case 16:
        if (FCompareMemoryW(wKey, L"BrowserFavorites")) return VK_BROWSER_FAVORITES;
        break;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------
EXPORT void FMsg(const wchar_t *wMsg)
{
    if (wMsg)
        if (const wchar_t wKeyPress = *wMsg)
        {
            if (const wchar_t wKeyUp = wMsg[1])
            {
                wMsg += 2;
                if (*wMsg)
                {
                    const wchar_t *pMsgSave = wMsg;
                    int iCount = 3;        //Ctrl+Shift+Alt KeyUp
                    while (*wMsg)
                    {
                        if (*wMsg == wKeyPress)
                        {
                            if (!(wMsg = FStrChrW(wMsg+1, wKeyPress)))
                                return;
                        }
                        else if (*wMsg == wKeyUp)
                        {
                            if (!(wMsg = FStrChrW(wMsg+1, wKeyUp)))
                                return;
                        }
                        else
                            ++iCount;
                        ++iCount;
                        ++wMsg;
                    }

                    if (const HANDLE hProcHeap = GetProcessHeap())
                        if (INPUT *const pInput = static_cast<INPUT*>(HeapAlloc(hProcHeap, HEAP_ZERO_MEMORY, iCount*sizeof(INPUT))))
                        {
                            int iIndex = 3;
                            WORD wVk;
                            wMsg = pMsgSave;
                            while (*wMsg)
                            {
                                if (*wMsg == wKeyPress)
                                {
                                    pMsgSave = wMsg+1;
                                    if (!(wMsg = FStrChrW(pMsgSave, wKeyPress)))
                                        goto m;
                                    *const_cast<wchar_t*>(wMsg) = L'\0';
                                    wVk = FFind(pMsgSave);
                                    *const_cast<wchar_t*>(wMsg) = wKeyPress;
                                    if (!wVk)
                                        goto m;
                                    pInput[iIndex].type = INPUT_KEYBOARD;
                                    pInput[iIndex].ki.wVk = wVk;
                                    //pInput[iIndex].ki.wScan = 0;
                                    //pInput[iIndex].ki.dwFlags = 0;
                                    //pInput[iIndex].ki.time = 0;
                                    //pInput[iIndex].ki.dwExtraInfo = 0;
                                }
                                else if (*wMsg == wKeyUp)
                                {
                                    pMsgSave = wMsg+1;
                                    if (!(wMsg = FStrChrW(pMsgSave, wKeyUp)))
                                        goto m;
                                    *const_cast<wchar_t*>(wMsg) = L'\0';
                                    wVk = FFind(pMsgSave);
                                    *const_cast<wchar_t*>(wMsg) = wKeyUp;
                                    if (!wVk)
                                        goto m;
                                    pInput[iIndex].type = INPUT_KEYBOARD;
                                    pInput[iIndex].ki.wVk = wVk;
                                    //pInput[iIndex].ki.wScan = 0;
                                    pInput[iIndex].ki.dwFlags = KEYEVENTF_KEYUP;
                                    //pInput[iIndex].ki.time = 0;
                                    //pInput[iIndex].ki.dwExtraInfo = 0;
                                }
                                else
                                {
                                    pInput[iIndex].type = INPUT_KEYBOARD;
                                    //pInput[iIndex].ki.wVk = 0;
                                    pInput[iIndex].ki.wScan = *wMsg;
                                    pInput[iIndex].ki.dwFlags = KEYEVENTF_UNICODE;
                                    //pInput[iIndex].ki.time = 0;
                                    //pInput[iIndex].ki.dwExtraInfo = 0;
                                    ++iIndex;
                                    pInput[iIndex].type = INPUT_KEYBOARD;
                                    //pInput[iIndex].ki.wVk = 0;
                                    pInput[iIndex].ki.wScan = *wMsg;
                                    pInput[iIndex].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_UNICODE;
                                    //pInput[iIndex].ki.time = 0;
                                    //pInput[iIndex].ki.dwExtraInfo = 0;
                                }
                                ++iIndex;
                                ++wMsg;
                            }

                            pInput[0].type = INPUT_KEYBOARD;
                            pInput[0].ki.wVk = VK_CONTROL;
                            //pInput[0].ki.wScan = 0;
                            pInput[0].ki.dwFlags = KEYEVENTF_KEYUP;
                            //pInput[0].ki.time = 0;
                            //pInput[0].ki.dwExtraInfo = 0;
                            pInput[1].type = INPUT_KEYBOARD;
                            pInput[1].ki.wVk = VK_SHIFT;
                            //pInput[1].ki.wScan = 0;
                            pInput[1].ki.dwFlags = KEYEVENTF_KEYUP;
                            //pInput[1].ki.time = 0;
                            //pInput[1].ki.dwExtraInfo = 0;
                            pInput[2].type = INPUT_KEYBOARD;
                            pInput[2].ki.wVk = VK_MENU;
                            //pInput[2].ki.wScan = 0;
                            pInput[2].ki.dwFlags = KEYEVENTF_KEYUP;
                            //pInput[2].ki.time = 0;
                            //pInput[2].ki.dwExtraInfo = 0;
                            SendInput(iCount, pInput, sizeof(INPUT));
m:
                            HeapFree(hProcHeap, 0, pInput);
                        }
                }
            }
            else
            {
                INPUT input[2];
                input[0].type = INPUT_KEYBOARD;
                input[0].ki.wVk = 0;
                input[0].ki.wScan = *wMsg;
                input[0].ki.dwFlags = KEYEVENTF_UNICODE;
                input[0].ki.time = 0;
                input[0].ki.dwExtraInfo = 0;
                input[1].type = INPUT_KEYBOARD;
                input[1].ki.wVk = 0;
                input[1].ki.wScan = *wMsg;
                input[1].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_UNICODE;
                input[1].ki.time = 0;
                input[1].ki.dwExtraInfo = 0;
                SendInput(2, input, sizeof(INPUT));
            }
        }
}

//-------------------------------------------------------------------------------------------------
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, DWORD, LPVOID)
{
    return TRUE;
}
