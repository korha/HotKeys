//HotKeys: VolumeControlXP
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#define EXPORT //__declspec(dllexport)

static constexpr const wchar_t *const g_wGuidClass = L"App::b749c579-152b-4b74-9bc2-e6948c47675c";
static constexpr const int g_iCellHeight = 144;
static constexpr const UINT g_iElapseTimer = 1200;
static constexpr const DWORD g_iMaxVol = 65535;
static HWND g_hWnd;
static COLORREF g_colRef;
static wchar_t g_wVolume[5];        //"100%`"
static bool g_bTimerActive = false;

//-------------------------------------------------------------------------------------------------
static inline bool FCompareMemoryW(const wchar_t *pBuf1, const wchar_t *pBuf2)
{
    while (*pBuf1 == *pBuf2 && *pBuf2)
        ++pBuf1, ++pBuf2;
    return *pBuf1 == *pBuf2;
}

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HFONT hFont;

    switch (uMsg)
    {
    case WM_CREATE:
        return ((hFont = CreateFontW(g_iCellHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Tahoma")) &&
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_COLORKEY)) ? 0 : -1;
    case WM_PAINT:
    {
        static PAINTSTRUCT ps;
        if (const HDC hDc = BeginPaint(hWnd, &ps))
        {
            if (const HDC hDcMem = CreateCompatibleDC(hDc))
            {
                const int iHorzRes = GetDeviceCaps(hDc, HORZRES),
                        iVertRes = GetDeviceCaps(hDc, VERTRES);
                if (const HBITMAP hBmpMem = CreateCompatibleBitmap(hDc, iHorzRes, iVertRes))
                {
                    RECT rect;
                    if (GetClientRect(hWnd, &rect))
                    {
                        rect.top = rect.bottom - (g_iCellHeight + g_iCellHeight/2);
                        SelectObject(hDcMem, hBmpMem);
                        SelectObject(hDcMem, hFont);
                        SetBkColor(hDcMem, RGB(1, 1, 1));
                        SetTextColor(hDcMem, g_colRef);
                        DrawTextW(hDcMem, g_wVolume, -1, &rect, DT_CENTER);
                        BitBlt(hDc, 0, 0, iHorzRes, iVertRes, hDcMem, 0, 0, SRCCOPY);
                    }
                    DeleteObject(hBmpMem);
                }
                DeleteDC(hDcMem);
            }
            EndPaint(hWnd, &ps);
        }
        return 0;
    }
    case WM_TIMER:
    {
        KillTimer(hWnd, 1);
        g_bTimerActive = false;
        ShowWindow(hWnd, SW_HIDE);
        return 0;
    }
    case WM_DESTROY:
    {
        if (g_bTimerActive)
            KillTimer(hWnd, 1);
        if (hFont)
            DeleteObject(hFont);
        g_hWnd = nullptr;
        return 0;
    }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
static void FUpdValue(const DWORD dwValue)
{
    if (dwValue < 10)
    {
        g_wVolume[0] = L'0' + dwValue;
        g_wVolume[1] = L'%';
        g_wVolume[2] = L'\0';
    }
    else if (dwValue < 100)
    {
        g_wVolume[0] = L'0' + dwValue/10;
        g_wVolume[1] = L'0' + dwValue%10;
        g_wVolume[2] = L'%';
        g_wVolume[3] = L'\0';
    }
    else
    {
        g_wVolume[0] = L'1';
        g_wVolume[1] = L'0';
        g_wVolume[2] = L'0';
        g_wVolume[3] = L'%';
    }
}

//-------------------------------------------------------------------------------------------------
static void FSetMute(const WINBOOL bMute)
{
    HMIXER hMixer;
    if (mixerOpen(&hMixer, 0, 0, 0, 0) == MMSYSERR_NOERROR)
    {
        HMIXEROBJ hMixerObj = static_cast<HMIXEROBJ>(static_cast<void*>(hMixer));
        MIXERLINE mixerLine;
        mixerLine.cbStruct = sizeof(MIXERLINE);
        mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
        if (mixerGetLineInfoW(hMixerObj, &mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
        {
            MIXERCONTROL mixerCtrl;
            MIXERLINECONTROLS mixerLineCtrls;
            mixerLineCtrls.cbStruct = sizeof(MIXERLINECONTROLS);
            mixerLineCtrls.dwLineID = mixerLine.dwLineID;
            mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
            mixerLineCtrls.cControls = 1;
            mixerLineCtrls.cbmxctrl = sizeof(MIXERCONTROL);
            mixerLineCtrls.pamxctrl = &mixerCtrl;
            if (mixerGetLineControlsW(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
            {
                MIXERCONTROLDETAILS_BOOLEAN mixerCtrlDtlsBool;
                mixerCtrlDtlsBool.fValue = bMute;
                MIXERCONTROLDETAILS mixerCtrlDtls;
                mixerCtrlDtls.cbStruct = sizeof(MIXERCONTROLDETAILS);
                mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                mixerCtrlDtls.cChannels = 1;
                mixerCtrlDtls.hwndOwner = nullptr;
                mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                mixerCtrlDtls.paDetails = &mixerCtrlDtlsBool;
                MIXERCONTROLDETAILS_UNSIGNED mixerCtrlDtlsUns;
                if (mixerSetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                {
                    mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                    mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
                    if (mixerGetLineControlsW(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                    {
                        mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                        mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                        mixerCtrlDtls.paDetails = &mixerCtrlDtlsUns;
                        if (mixerGetControlDetailsW(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                        {
                            FUpdValue((mixerCtrlDtlsUns.dwValue + 1)*100/g_iMaxVol);
                            g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                            InvalidateRect(g_hWnd, nullptr, FALSE);
                            ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                            if (SetTimer(g_hWnd, 1, g_iElapseTimer, nullptr))
                                g_bTimerActive = true;
                        }
                    }
                }
            }
        }
        mixerClose(hMixer);
    }
}

//-------------------------------------------------------------------------------------------------
EXPORT void FMsg(const wchar_t *wMsg)
{
    if (g_hWnd && wMsg)
    {
        if (FCompareMemoryW(wMsg, L"/mute"))
            FSetMute(TRUE);
        else if (FCompareMemoryW(wMsg, L"/unmute"))
            FSetMute(FALSE);
        else if (FCompareMemoryW(wMsg, L"/toggle-mute"))
        {
            HMIXER hMixer;
            if (mixerOpen(&hMixer, 0, 0, 0, 0) == MMSYSERR_NOERROR)
            {
                HMIXEROBJ hMixerObj = static_cast<HMIXEROBJ>(static_cast<void*>(hMixer));
                MIXERLINE mixerLine;
                mixerLine.cbStruct = sizeof(MIXERLINE);
                mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
                if (mixerGetLineInfoW(hMixerObj, &mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
                {
                    MIXERCONTROL mixerCtrl;
                    MIXERLINECONTROLS mixerLineCtrls;
                    mixerLineCtrls.cbStruct = sizeof(MIXERLINECONTROLS);
                    mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                    mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
                    mixerLineCtrls.cControls = 1;
                    mixerLineCtrls.cbmxctrl = sizeof(MIXERCONTROL);
                    mixerLineCtrls.pamxctrl = &mixerCtrl;
                    if (mixerGetLineControlsW(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                    {
                        MIXERCONTROLDETAILS_BOOLEAN mixerCtrlDtlsBool;
                        MIXERCONTROLDETAILS mixerCtrlDtls;
                        mixerCtrlDtls.cbStruct = sizeof(MIXERCONTROLDETAILS);
                        mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                        mixerCtrlDtls.cChannels = 1;
                        mixerCtrlDtls.hwndOwner = nullptr;
                        mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                        mixerCtrlDtls.paDetails = &mixerCtrlDtlsBool;
                        MIXERCONTROLDETAILS_UNSIGNED mixerCtrlDtlsUns;
                        if (mixerGetControlDetailsW(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                        {
                            mixerCtrlDtlsBool.fValue = !mixerCtrlDtlsBool.fValue;
                            if (mixerSetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                            {
                                mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                                mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
                                if (mixerGetLineControlsW(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                                {
                                    mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                                    mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                                    mixerCtrlDtls.paDetails = &mixerCtrlDtlsUns;
                                    if (mixerGetControlDetailsW(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                                    {
                                        FUpdValue((mixerCtrlDtlsUns.dwValue + 1)*100/g_iMaxVol);
                                        g_colRef = mixerCtrlDtlsBool.fValue ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                        InvalidateRect(g_hWnd, nullptr, FALSE);
                                        ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                        if (SetTimer(g_hWnd, 1, g_iElapseTimer, nullptr))
                                            g_bTimerActive = true;
                                    }
                                }
                            }
                        }
                    }
                }
                mixerClose(hMixer);
            }
        }
        else if ((*wMsg == L'+' || *wMsg == L'-' || *wMsg == L'=') &&
                 (wMsg[1] == L'0' || wMsg[1] == L'1') &&
                 (wMsg[2] >= L'0' && wMsg[2] <= L'9') &&
                 (wMsg[3] >= L'0' && wMsg[3] <= L'9') &&
                 wMsg[4] == L'\0')
        {
            DWORD dwVolume = (wMsg[1] - L'0')*100 + (wMsg[2] - L'0')*10 + (wMsg[3] - L'0');
            if (dwVolume <= 100)
            {
                HMIXER hMixer;
                if (mixerOpen(&hMixer, 0, 0, 0, 0) == MMSYSERR_NOERROR)
                {
                    HMIXEROBJ hMixerObj = static_cast<HMIXEROBJ>(static_cast<void*>(hMixer));
                    MIXERLINE mixerLine;
                    mixerLine.cbStruct = sizeof(MIXERLINE);
                    mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
                    if (mixerGetLineInfoW(hMixerObj, &mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
                    {
                        MIXERCONTROL mixerCtrl;
                        MIXERLINECONTROLS mixerLineCtrls;
                        mixerLineCtrls.cbStruct = sizeof(MIXERLINECONTROLS);
                        mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                        mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
                        mixerLineCtrls.cControls = 1;
                        mixerLineCtrls.cbmxctrl = sizeof(MIXERCONTROL);
                        mixerLineCtrls.pamxctrl = &mixerCtrl;
                        if (mixerGetLineControlsW(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                        {
                            MIXERCONTROLDETAILS_UNSIGNED mixerCtrlDtlsUns;
                            mixerCtrlDtlsUns.dwValue = dwVolume*g_iMaxVol/100;
                            MIXERCONTROLDETAILS mixerCtrlDtls;
                            mixerCtrlDtls.cbStruct = sizeof(MIXERCONTROLDETAILS);
                            mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                            mixerCtrlDtls.cChannels = 1;
                            mixerCtrlDtls.hwndOwner = nullptr;
                            mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                            mixerCtrlDtls.paDetails = &mixerCtrlDtlsUns;
                            MIXERCONTROLDETAILS_BOOLEAN mixerCtrlDtlsBool;

                            if (*wMsg == L'=')
                            {
                                if (mixerSetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                                {
                                    mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                                    mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
                                    if (mixerGetLineControlsW(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                                    {
                                        mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                                        mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                                        mixerCtrlDtls.paDetails = &mixerCtrlDtlsBool;
                                        if (mixerGetControlDetailsW(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                                        {
                                            FUpdValue(dwVolume);
                                            g_colRef = mixerCtrlDtlsBool.fValue ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                            InvalidateRect(g_hWnd, nullptr, FALSE);
                                            ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                            if (SetTimer(g_hWnd, 1, g_iElapseTimer, nullptr))
                                                g_bTimerActive = true;
                                        }
                                    }
                                }
                            }
                            else if (mixerGetControlDetailsW(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                            {
                                const DWORD dwValuePrev = (mixerCtrlDtlsUns.dwValue + 1)*100/g_iMaxVol;
                                if (*wMsg == L'+')
                                {
                                    dwVolume += dwValuePrev;
                                    if (dwVolume > 100)
                                        dwVolume = 100;
                                }
                                else        //(*wMsg == L'+')
                                {
                                    if (dwValuePrev > dwVolume)
                                        dwVolume = dwValuePrev - dwVolume;
                                    else
                                        dwVolume = 0;
                                }
                                mixerCtrlDtlsUns.dwValue = dwVolume*g_iMaxVol/100;
                                if (mixerSetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                                {
                                    mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                                    mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
                                    if (mixerGetLineControlsW(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                                    {
                                        mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                                        mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                                        mixerCtrlDtls.paDetails = &mixerCtrlDtlsBool;
                                        if (mixerGetControlDetailsW(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                                        {
                                            FUpdValue(dwVolume);
                                            g_colRef = mixerCtrlDtlsBool.fValue ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                            InvalidateRect(g_hWnd, nullptr, FALSE);
                                            ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                            if (SetTimer(g_hWnd, 1, g_iElapseTimer, nullptr))
                                                g_bTimerActive = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    mixerClose(hMixer);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved)
{
    //functions from user32.dll in DllMain is legal because main app load/unload this dll in manual (dynamical) mode
    static bool bOk = false;
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        if (lpvReserved == nullptr && ((GetVersion() & 0xFF) <= ((_WIN32_WINNT_WINXP >> 8) & 0xFF)))
        {
            WNDCLASSEXW wndCl;
            wndCl.cbSize = sizeof(WNDCLASSEX);
            wndCl.style = 0;
            wndCl.lpfnWndProc = WindowProc;
            wndCl.cbClsExtra = 0;
            wndCl.cbWndExtra = 0;
            wndCl.hInstance = hInstDll;
            wndCl.hIcon = nullptr;
            wndCl.hCursor = nullptr;
            wndCl.hbrBackground = nullptr;
            wndCl.lpszMenuName = nullptr;
            wndCl.lpszClassName = g_wGuidClass;
            wndCl.hIconSm = nullptr;

            if (RegisterClassExW(&wndCl))        //***
            {
                if ((g_hWnd = CreateWindowExW(WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST, g_wGuidClass, nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, hInstDll, nullptr)))        //***
                {
                    bOk = true;
                    return TRUE;
                }
                UnregisterClassW(g_wGuidClass, hInstDll);        //***
            }
        }
    }
    else if (fdwReason == DLL_PROCESS_DETACH && bOk)
    {
        if (g_hWnd)
            SendMessageW(g_hWnd, WM_CLOSE, 0, 0);        //***
        UnregisterClassW(g_wGuidClass, hInstDll);        //***
    }
    return FALSE;
}
