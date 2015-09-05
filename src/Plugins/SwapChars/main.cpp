#include <windows.h>
#include "custom.h"

#define EXPORT //__declspec(dllexport)

INPUT input[7];

//-------------------------------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        memset(input, 0, 7*sizeof(INPUT));
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_CONTROL;
        //input[0].ki.wScan = 0;
        input[0].ki.dwFlags = KEYEVENTF_KEYUP;
        //input[0].ki.time = 0;
        //input[0].ki.dwExtraInfo = 0;
        input[1].type = INPUT_KEYBOARD;
        input[1].ki.wVk = VK_SHIFT;
        //input[1].ki.wScan = 0;
        input[1].ki.dwFlags = KEYEVENTF_KEYUP;
        //input[1].ki.time = 0;
        //input[1].ki.dwExtraInfo = 0;
        input[2].type = INPUT_KEYBOARD;
        input[2].ki.wVk = VK_MENU;
        //input[2].ki.wScan = 0;
        input[2].ki.dwFlags = KEYEVENTF_KEYUP;
        //input[2].ki.time = 0;
        //input[2].ki.dwExtraInfo = 0;
        input[3].type = INPUT_KEYBOARD;
        input[3].ki.wVk = VK_CONTROL;
        //input[3].ki.wScan = 0;
        //input[3].ki.dwFlags = 0;
        //input[3].ki.time = 0;
        //input[3].ki.dwExtraInfo = 0;
        input[4].type = INPUT_KEYBOARD;
        //input[4].ki.wVk = L'C';
        //input[4].ki.wScan = 0;
        //input[4].ki.dwFlags = 0;
        //input[4].ki.time = 0;
        //input[4].ki.dwExtraInfo = 0;
        input[5].type = INPUT_KEYBOARD;
        //input[5].ki.wVk = L'C';
        //input[5].ki.wScan = 0;
        input[5].ki.dwFlags = KEYEVENTF_KEYUP;
        //input[5].ki.time = 0;
        //input[5].ki.dwExtraInfo = 0;
        input[6].type = INPUT_KEYBOARD;
        input[6].ki.wVk = VK_CONTROL;
        //input[6].ki.wScan = 0;
        input[6].ki.dwFlags = KEYEVENTF_KEYUP;
        //input[6].ki.time = 0;
        //input[6].ki.dwExtraInfo = 0;
    }
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
EXPORT void fMsg(const wchar_t *wMsg)
{
    if (wMsg)
    {
        input[4].ki.wVk = L'C';
        input[5].ki.wVk = L'C';
        SendInput(7, input, sizeof(INPUT));
        Sleep(50);

        if (IsClipboardFormatAvailable(CF_UNICODETEXT) && OpenClipboard(0))
        {
            bool bValid = false;
            if (wchar_t *const hClipboardText = static_cast<wchar_t*>(GetClipboardData(CF_UNICODETEXT)))
                if (GlobalLock(hClipboardText))
                {
                    if (const HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (wcslen(hClipboardText)+1)*sizeof(wchar_t)))
                    {
                        if (wchar_t *pBuf = static_cast<wchar_t*>(GlobalLock(hGlobal)))
                        {
                            bool bOk = true;
                            const wchar_t *wChar = hClipboardText;
                            switch (*wMsg)
                            {
                            SWAPCHARS
                                    default: bOk = false;
                            }
m:
                            GlobalUnlock(hClipboardText);

                            if (bOk && EmptyClipboard())
                            {
                                if (SetClipboardData(CF_UNICODETEXT, hGlobal))
                                {
                                    if (GlobalUnlock(hGlobal) == 0)
                                    {
                                        if (GlobalFree(hGlobal) == 0)
                                            bValid = true;
                                    }
                                    else
                                        GlobalFree(hGlobal);
                                }
                                else
                                {
                                    GlobalUnlock(hGlobal);
                                    GlobalFree(hGlobal);
                                }
                            }
                            else
                            {
                                GlobalUnlock(hGlobal);
                                GlobalFree(hGlobal);
                            }
                            static_cast<void>(pBuf);
                            static_cast<void>(wChar);
                        }
                        else
                        {
                            GlobalFree(hGlobal);
                            GlobalUnlock(hClipboardText);
                        }
                    }
                    else
                        GlobalUnlock(hClipboardText);
                }

            if (CloseClipboard() && bValid)
            {
                input[4].ki.wVk = L'V';
                input[5].ki.wVk = L'V';
                SendInput(4, input+3, sizeof(INPUT));
            }
        }
    }
}
