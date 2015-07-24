#include <windows.h>

#define EXPORT //__declspec(dllexport)

//-------------------------------------------------------------------------------------------------
WORD fFind(const wchar_t *const wKey)
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
            if (wcscmp(wKey, L"Alt") == 0) return VK_MENU;
            if (wcscmp(wKey, L"Esc") == 0) return VK_ESCAPE;
            if (wcscmp(wKey, L"Del") == 0) return VK_DELETE;
            if (wcscmp(wKey, L"End") == 0) return VK_END;
            if (wcscmp(wKey, L"Tab") == 0) return VK_TAB;
            if (wcscmp(wKey, L"Ins") == 0) return VK_INSERT;
            if (wcscmp(wKey, L"PA1") == 0) return VK_PA1;
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
            if (wcscmp(wKey, L"Ctrl") == 0) return VK_CONTROL;
            if (wcscmp(wKey, L"Down") == 0) return VK_DOWN;
            if (wcscmp(wKey, L"Left") == 0) return VK_LEFT;
            if (wcscmp(wKey, L"PgUp") == 0) return VK_PRIOR;
            if (wcscmp(wKey, L"PgDn") == 0) return VK_NEXT;
            if (wcscmp(wKey, L"LAlt") == 0) return VK_LMENU;
            if (wcscmp(wKey, L"LWin") == 0) return VK_LWIN;
            if (wcscmp(wKey, L"RAlt") == 0) return VK_RMENU;
            if (wcscmp(wKey, L"Home") == 0) return VK_HOME;
            if (wcscmp(wKey, L"RWin") == 0) return VK_RWIN;
            if (wcscmp(wKey, L"Apps") == 0) return VK_APPS;
            if (wcscmp(wKey, L"Help") == 0) return VK_HELP;
            if (wcscmp(wKey, L"Play") == 0) return VK_PLAY;
            if (wcscmp(wKey, L"Zoom") == 0) return VK_ZOOM;
            if (wcscmp(wKey, L"Attn") == 0) return VK_ATTN;
            if (wcscmp(wKey, L"Kana") == 0) return VK_KANA;
        }
        break;

    case 5:
        if (wcscmp(wKey, L"Shift") == 0) return VK_SHIFT;
        if (wcscmp(wKey, L"Space") == 0) return VK_SPACE;
        if (wcscmp(wKey, L"Enter") == 0) return VK_RETURN;
        if (wcscmp(wKey, L"Right") == 0) return VK_RIGHT;
        if (wcscmp(wKey, L"LCtrl") == 0) return VK_LCONTROL;
        if (wcscmp(wKey, L"RCtrl") == 0) return VK_RCONTROL;
        if (wcscmp(wKey, L"Pause") == 0) return VK_PAUSE;
        if (wcscmp(wKey, L"Print") == 0) return VK_PRINT;
        if (wcscmp(wKey, L"Sleep") == 0) return VK_SLEEP;
        if (wcscmp(wKey, L"Clear") == 0) return VK_CLEAR;
        if (wcscmp(wKey, L"CrSel") == 0) return VK_CRSEL;
        if (wcscmp(wKey, L"ExSel") == 0) return VK_EXSEL;
        if (wcscmp(wKey, L"ErEOF") == 0) return VK_EREOF;
        if (wcscmp(wKey, L"Final") == 0) return VK_FINAL;
        if (wcscmp(wKey, L"Junja") == 0) return VK_JUNJA;
        if (wcscmp(wKey, L"Hanja") == 0) return VK_HANJA;
        break;

    case 6:
        if (wcscmp(wKey, L"LShift") == 0) return VK_LSHIFT;
        if (wcscmp(wKey, L"RShift") == 0) return VK_RSHIFT;
        if (wcscmp(wKey, L"PrtScr") == 0) return VK_SNAPSHOT;
        if (wcscmp(wKey, L"Accept") == 0) return VK_ACCEPT;
        if (wcscmp(wKey, L"Select") == 0) return VK_SELECT;
        break;

    case 7:
        if (wcscmp(wKey, L"NumLock") == 0) return VK_NUMLOCK;
        if (wcscmp(wKey, L"Convert") == 0) return VK_CONVERT;
        if (wcscmp(wKey, L"Execute") == 0) return VK_EXECUTE;
        if (wcscmp(wKey, L"Process") == 0) return VK_PROCESSKEY;
        break;

    case 8:
        if (wcscmp(wKey, L"XButton1") == 0) return VK_XBUTTON1;
        if (wcscmp(wKey, L"XButton2") == 0) return VK_XBUTTON2;
        if (wcscmp(wKey, L"CapsLock") == 0) return VK_CAPITAL;
        if (wcscmp(wKey, L"VolumeUp") == 0) return VK_VOLUME_UP;
        if (wcscmp(wKey, L"OemClear") == 0) return VK_OEM_CLEAR;
        break;

    case 9:
        if (wcscmp(wKey, L"BackSpace") == 0) return VK_BACK;
        if (wcscmp(wKey, L"MediaNext") == 0) return VK_MEDIA_NEXT_TRACK;
        if (wcscmp(wKey, L"MediaPrev") == 0) return VK_MEDIA_PREV_TRACK;
        if (wcscmp(wKey, L"MediaStop") == 0) return VK_MEDIA_STOP;
        if (wcscmp(wKey, L"Separator") == 0) return VK_SEPARATOR;
        if (wcscmp(wKey, L"StartApp1") == 0) return VK_LAUNCH_APP1;
        if (wcscmp(wKey, L"StartApp2") == 0) return VK_LAUNCH_APP2;
        if (wcscmp(wKey, L"StartMail") == 0) return VK_LAUNCH_MAIL;
        break;

    case 10:
        if (wcscmp(wKey, L"LeftButton") == 0) return VK_LBUTTON;
        if (wcscmp(wKey, L"VolumeDown") == 0) return VK_VOLUME_UP;
        if (wcscmp(wKey, L"VolumeMute") == 0) return VK_VOLUME_MUTE;
        if (wcscmp(wKey, L"ScrollLock") == 0) return VK_SCROLL;
        if (wcscmp(wKey, L"NonConvert") == 0) return VK_NONCONVERT;
        if (wcscmp(wKey, L"ModeChange") == 0) return VK_MODECHANGE;
        break;

    case 11:
        if (wcscmp(wKey, L"RightButton") == 0) return VK_RBUTTON;
        if (wcscmp(wKey, L"BrowserBack") == 0) return VK_BROWSER_BACK;
        if (wcscmp(wKey, L"BrowserStop") == 0) return VK_BROWSER_STOP;
        if (wcscmp(wKey, L"BrowserHome") == 0) return VK_BROWSER_HOME;
        if (wcscmp(wKey, L"MediaSelect") == 0) return VK_LAUNCH_MEDIA_SELECT;
        break;

    case 12:
        if (wcscmp(wKey, L"MiddleButton") == 0) return VK_MBUTTON;
        break;

    case 13:
        if (wcscmp(wKey, L"Control-Break") == 0) return VK_CANCEL;
        if (wcscmp(wKey, L"BrowserSearch") == 0) return VK_BROWSER_SEARCH;
        break;

    case 14:
        if (wcscmp(wKey, L"BrowserForward") == 0) return VK_BROWSER_FORWARD;
        if (wcscmp(wKey, L"BrowserRefresh") == 0) return VK_BROWSER_REFRESH;
        if (wcscmp(wKey, L"MediaPlayPause") == 0) return VK_MEDIA_PLAY_PAUSE;
        break;

    case 16:
        if (wcscmp(wKey, L"BrowserFavorites") == 0) return VK_BROWSER_FAVORITES;
        break;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------
EXPORT void fMsg(const wchar_t *wMsg)
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
                            if (!(wMsg = wcschr(wMsg+1, wKeyPress)))
                                return;
                        }
                        else if (*wMsg == wKeyUp)
                        {
                            if (!(wMsg = wcschr(wMsg+1, wKeyUp)))
                                return;
                        }
                        else
                            ++iCount;
                        ++iCount;
                        ++wMsg;
                    }

                    if (INPUT *const pInput = static_cast<INPUT*>(malloc(iCount*sizeof(INPUT))))
                    {
                        memset(pInput, 0, iCount*sizeof(INPUT));
                        int iIndex = 3;
                        WORD wVk;
                        wMsg = pMsgSave;
                        while (*wMsg)
                        {
                            if (*wMsg == wKeyPress)
                            {
                                pMsgSave = wMsg+1;
                                if (!(wMsg = wcschr(pMsgSave, wKeyPress)))
                                    goto m;
                                *const_cast<wchar_t*>(wMsg) = L'\0';
                                wVk = fFind(pMsgSave);
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
                                if (!(wMsg = wcschr(pMsgSave, wKeyUp)))
                                    goto m;
                                *const_cast<wchar_t*>(wMsg) = L'\0';
                                wVk = fFind(pMsgSave);
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
                        free(pInput);
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
