#include <cwchar>
#include <windows.h>

#define EXPORT //__declspec(dllexport)

//-------------------------------------------------------------------------------------------------
static WORD fFind(const wchar_t *const wKey)
{
    switch (wcslen(wKey))
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
                switch(wKey[2])
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
            if (wcscmp(L"Alt", wKey) == 0) return VK_MENU;
            if (wcscmp(L"Esc", wKey) == 0) return VK_ESCAPE;
            if (wcscmp(L"Del", wKey) == 0) return VK_DELETE;
            if (wcscmp(L"End", wKey) == 0) return VK_END;
            if (wcscmp(L"Tab", wKey) == 0) return VK_TAB;
            if (wcscmp(L"Ins", wKey) == 0) return VK_INSERT;
            if (wcscmp(L"PA1", wKey) == 0) return VK_PA1;
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
                case L'0': iKey |= 0x0; break;
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
                switch(wKey[3])
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
            if (wcscmp(L"Ctrl", wKey) == 0) return VK_CONTROL;
            if (wcscmp(L"Down", wKey) == 0) return VK_DOWN;
            if (wcscmp(L"Left", wKey) == 0) return VK_LEFT;
            if (wcscmp(L"PgUp", wKey) == 0) return VK_PRIOR;
            if (wcscmp(L"PgDn", wKey) == 0) return VK_NEXT;
            if (wcscmp(L"LAlt", wKey) == 0) return VK_LMENU;
            if (wcscmp(L"LWin", wKey) == 0) return VK_LWIN;
            if (wcscmp(L"RAlt", wKey) == 0) return VK_RMENU;
            if (wcscmp(L"Home", wKey) == 0) return VK_HOME;
            if (wcscmp(L"RWin", wKey) == 0) return VK_RWIN;
            if (wcscmp(L"Apps", wKey) == 0) return VK_APPS;
            if (wcscmp(L"Help", wKey) == 0) return VK_HELP;
            if (wcscmp(L"Play", wKey) == 0) return VK_PLAY;
            if (wcscmp(L"Zoom", wKey) == 0) return VK_ZOOM;
            if (wcscmp(L"Attn", wKey) == 0) return VK_ATTN;
            if (wcscmp(L"Kana", wKey) == 0) return VK_KANA;
        }
        break;

    case 5:
        if (wcscmp(L"Shift", wKey) == 0) return VK_SHIFT;
        if (wcscmp(L"Space", wKey) == 0) return VK_SPACE;
        if (wcscmp(L"Enter", wKey) == 0) return VK_RETURN;
        if (wcscmp(L"Right", wKey) == 0) return VK_RIGHT;
        if (wcscmp(L"LCtrl", wKey) == 0) return VK_LCONTROL;
        if (wcscmp(L"RCtrl", wKey) == 0) return VK_RCONTROL;
        if (wcscmp(L"Pause", wKey) == 0) return VK_PAUSE;
        if (wcscmp(L"Print", wKey) == 0) return VK_PRINT;
        if (wcscmp(L"Sleep", wKey) == 0) return VK_SLEEP;
        if (wcscmp(L"Clear", wKey) == 0) return VK_CLEAR;
        if (wcscmp(L"CrSel", wKey) == 0) return VK_CRSEL;
        if (wcscmp(L"ExSel", wKey) == 0) return VK_EXSEL;
        if (wcscmp(L"ErEOF", wKey) == 0) return VK_EREOF;
        if (wcscmp(L"Final", wKey) == 0) return VK_FINAL;
        if (wcscmp(L"Junja", wKey) == 0) return VK_JUNJA;
        if (wcscmp(L"Hanja", wKey) == 0) return VK_HANJA;
        break;

    case 6:
        if (wcscmp(L"LShift", wKey) == 0) return VK_LSHIFT;
        if (wcscmp(L"RShift", wKey) == 0) return VK_RSHIFT;
        if (wcscmp(L"PrtScr", wKey) == 0) return VK_SNAPSHOT;
        if (wcscmp(L"Accept", wKey) == 0) return VK_ACCEPT;
        if (wcscmp(L"Select", wKey) == 0) return VK_SELECT;
        break;

    case 7:
        if (wcscmp(L"NumLock", wKey) == 0) return VK_NUMLOCK;
        if (wcscmp(L"Convert", wKey) == 0) return VK_CONVERT;
        if (wcscmp(L"Execute", wKey) == 0) return VK_EXECUTE;
        if (wcscmp(L"Process", wKey) == 0) return VK_PROCESSKEY;
        break;

    case 8:
        if (wcscmp(L"XButton1", wKey) == 0) return VK_XBUTTON1;
        if (wcscmp(L"XButton2", wKey) == 0) return VK_XBUTTON2;
        if (wcscmp(L"CapsLock", wKey) == 0) return VK_CAPITAL;
        if (wcscmp(L"VolumeUp", wKey) == 0) return VK_VOLUME_UP;
        if (wcscmp(L"OemClear", wKey) == 0) return VK_OEM_CLEAR;
        break;

    case 9:
        if (wcscmp(L"BackSpace", wKey) == 0) return VK_BACK;
        if (wcscmp(L"MediaNext", wKey) == 0) return VK_MEDIA_NEXT_TRACK;
        if (wcscmp(L"MediaPrev", wKey) == 0) return VK_MEDIA_PREV_TRACK;
        if (wcscmp(L"MediaStop", wKey) == 0) return VK_MEDIA_STOP;
        if (wcscmp(L"Separator", wKey) == 0) return VK_SEPARATOR;
        if (wcscmp(L"StartApp1", wKey) == 0) return VK_LAUNCH_APP1;
        if (wcscmp(L"StartApp2", wKey) == 0) return VK_LAUNCH_APP2;
        if (wcscmp(L"StartMail", wKey) == 0) return VK_LAUNCH_MAIL;
        break;

    case 10:
        if (wcscmp(L"LeftButton", wKey) == 0) return VK_LBUTTON;
        if (wcscmp(L"VolumeDown", wKey) == 0) return VK_VOLUME_UP;
        if (wcscmp(L"VolumeMute", wKey) == 0) return VK_VOLUME_MUTE;
        if (wcscmp(L"ScrollLock", wKey) == 0) return VK_SCROLL;
        if (wcscmp(L"NonConvert", wKey) == 0) return VK_NONCONVERT;
        if (wcscmp(L"ModeChange", wKey) == 0) return VK_MODECHANGE;
        break;

    case 11:
        if (wcscmp(L"RightButton", wKey) == 0) return VK_RBUTTON;
        if (wcscmp(L"BrowserBack", wKey) == 0) return VK_BROWSER_BACK;
        if (wcscmp(L"BrowserStop", wKey) == 0) return VK_BROWSER_STOP;
        if (wcscmp(L"BrowserHome", wKey) == 0) return VK_BROWSER_HOME;
        if (wcscmp(L"MediaSelect", wKey) == 0) return VK_LAUNCH_MEDIA_SELECT;
        break;

    case 12:
        if (wcscmp(L"MiddleButton", wKey) == 0) return VK_MBUTTON;
        break;

    case 13:
        if (wcscmp(L"Control-Break", wKey) == 0) return VK_CANCEL;
        if (wcscmp(L"BrowserSearch", wKey) == 0) return VK_BROWSER_SEARCH;
        break;

    case 14:
        if (wcscmp(L"BrowserForward", wKey) == 0) return VK_BROWSER_FORWARD;
        if (wcscmp(L"BrowserRefresh", wKey) == 0) return VK_BROWSER_REFRESH;
        if (wcscmp(L"MediaPlayPause", wKey) == 0) return VK_MEDIA_PLAY_PAUSE;
        break;

    case 16:
        if (wcscmp(L"BrowserFavorites", wKey) == 0) return VK_BROWSER_FAVORITES;
        break;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------
bool EXPORT fInit(const bool)
{
    return true;
}

//-------------------------------------------------------------------------------------------------
void EXPORT fMsg(const wchar_t *wMsg)
{
    int argc;
    if (wchar_t **const argv = CommandLineToArgvW(wMsg, &argc))
    {
        char *const cSet = new char[argc];
        unsigned int iInputSet = 0;
        for (int i = 0; i < argc; ++i)
            if (wcsncmp(L"#P_", argv[i], 3) == 0)
            {
                cSet[i] = 'P';
                ++iInputSet;
            }
            else if (wcsncmp(L"#R_", argv[i], 3) == 0)
            {
                cSet[i] = 'R';
                ++iInputSet;
            }
            else
            {
                cSet[i] = 'U';
                iInputSet += wcslen(argv[i])*2;
            }

        INPUT *const pInput = new INPUT[iInputSet];
        for (int i = 0, j = 0; i < argc; ++i)
            switch(cSet[i])
            {
            case 'P':
            {
                pInput[j].type = INPUT_KEYBOARD;
                if (!(pInput[j].ki.wVk = fFind(argv[i]+3)))
                    goto m;
                pInput[j].ki.wScan = 0;
                pInput[j].ki.dwFlags = 0;
                pInput[j].ki.time = 0;
                pInput[j].ki.dwExtraInfo = 0;
                ++j;
                break;
            }

            case 'R':
            {
                pInput[j].type = INPUT_KEYBOARD;
                if (!(pInput[j].ki.wVk = fFind(argv[i]+3)))
                    goto m;
                pInput[j].ki.wScan = 0;
                pInput[j].ki.dwFlags = KEYEVENTF_KEYUP;
                pInput[j].ki.time = 0;
                pInput[j].ki.dwExtraInfo = 0;
                ++j;
                break;
            }

            case 'U':
            {
                const wchar_t *pWchar = argv[i];
                while (*pWchar != L'\0')
                {
                    pInput[j].type = INPUT_KEYBOARD;
                    pInput[j].ki.wVk = 0;
                    pInput[j].ki.wScan = *pWchar;
                    pInput[j].ki.dwFlags = KEYEVENTF_UNICODE;
                    pInput[j].ki.time = 0;
                    pInput[j].ki.dwExtraInfo = 0;
                    ++j;
                    pInput[j].type = INPUT_KEYBOARD;
                    pInput[j].ki.wVk = 0;
                    pInput[j].ki.wScan = *pWchar;
                    pInput[j].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_UNICODE;
                    pInput[j].ki.time = 0;
                    pInput[j].ki.dwExtraInfo = 0;
                    ++j;

                    ++pWchar;
                }
                break;
            }
            }

        if (GetAsyncKeyState(VK_CONTROL) < 0)
            keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
        if (GetAsyncKeyState(VK_SHIFT) < 0)
            keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
        if (GetAsyncKeyState(VK_MENU) < 0)
            keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
        SendInput(iInputSet, pInput, sizeof(INPUT));

m:
        delete[] pInput;
        delete[] cSet;
        LocalFree(argv);
    }
}
