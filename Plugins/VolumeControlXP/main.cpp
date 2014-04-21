#include <windows.h>
#include <mmsystem.h>

#define EXPORT //__declspec(dllexport)
#define MAX_VOL 65535
#define CLASS_NAME L"b749c579-152b-4b74-9bc2-e6948c47675c"
#define FONT_HEIGHT 144

static HWND g_hWnd;
static HFONT g_hFont;
static COLORREF g_colRef;
static wchar_t g_wVolume[5];        //[100%\0]

//-------------------------------------------------------------------------------------------------
static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_PAINT)
    {
        PAINTSTRUCT ps;
        if (const HDC hDc = BeginPaint(hWnd, &ps))
        {
            if (const HDC hDcMem = CreateCompatibleDC(hDc))
            {
                const int iHorzRes = GetDeviceCaps(hDc, HORZRES),
                        iVertRes = GetDeviceCaps(hDc, VERTRES);
                if (const HBITMAP hBmpMem = CreateCompatibleBitmap(hDc, iHorzRes, iVertRes))
                {
                    RECT rect;
                    GetClientRect(hWnd, &rect);
                    rect.top = rect.bottom - (FONT_HEIGHT + 70);
                    SelectObject(hDcMem, hBmpMem);
                    SelectObject(hDcMem, g_hFont);
                    SetBkColor(hDcMem, RGB(1, 1, 1));
                    SetTextColor(hDcMem, g_colRef);
                    DrawText(hDcMem, g_wVolume, -1, &rect, DT_CENTER);
                    BitBlt(hDc, 0, 0, iHorzRes, iVertRes, hDcMem, 0, 0, SRCCOPY);
                    DeleteObject(hBmpMem);
                }
                DeleteDC(hDcMem);
            }
            EndPaint(hWnd, &ps);
        }
        return 0;
    }
    if (uMsg == WM_TIMER)
    {
        ShowWindow(hWnd, SW_HIDE);
        KillTimer(hWnd, 0);
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
static void fSetMute(const bool bMute)
{
    HMIXER hMixer;
    if (mixerOpen(&hMixer, 0, 0, 0, 0) == MMSYSERR_NOERROR)
    {
        HMIXEROBJ hMixerObj = static_cast<HMIXEROBJ>(static_cast<void*>(hMixer));
        MIXERLINE mixerLine;
        mixerLine.cbStruct = sizeof(MIXERLINE);
        mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
        if (mixerGetLineInfo(hMixerObj, &mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
        {
            MIXERCONTROL mixerCtrl;
            MIXERLINECONTROLS mixerLineCtrls;
            mixerLineCtrls.cbStruct = sizeof(MIXERLINECONTROLS);
            mixerLineCtrls.dwLineID = mixerLine.dwLineID;
            mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
            mixerLineCtrls.cControls = 1;
            mixerLineCtrls.cbmxctrl = sizeof(MIXERCONTROL);
            mixerLineCtrls.pamxctrl = &mixerCtrl;
            if (mixerGetLineControls(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
            {
                MIXERCONTROLDETAILS_BOOLEAN mixerCtrlDtlsBool;
                mixerCtrlDtlsBool.fValue = bMute;
                MIXERCONTROLDETAILS mixerCtrlDtls;
                mixerCtrlDtls.cbStruct = sizeof(MIXERCONTROLDETAILS);
                mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                mixerCtrlDtls.cChannels = 1;
                mixerCtrlDtls.hwndOwner = 0;
                mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                mixerCtrlDtls.paDetails = &mixerCtrlDtlsBool;
                if (mixerSetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                {
                    mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                    mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
                    if (mixerGetLineControls(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                    {
                        MIXERCONTROLDETAILS_UNSIGNED mixerCtrlDtlsUns;
                        mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                        mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                        mixerCtrlDtls.paDetails = &mixerCtrlDtlsUns;
                        if (mixerGetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                        {
                            wcscat(_ultow((mixerCtrlDtlsUns.dwValue + 1)*100/MAX_VOL, g_wVolume, 10), L"%");
                            g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                            InvalidateRect(g_hWnd, 0, FALSE);
                            ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                            SetTimer(g_hWnd, 0, 1200, 0);
                        }
                    }
                }
            }
        }
        mixerClose(hMixer);
    }
}

//-------------------------------------------------------------------------------------------------
bool EXPORT fInit(const bool bInit)
{
    if (bInit)
    {
        if (static_cast<BYTE>(GetVersion()) == 5 &&        //Win2000/XP (NT5)
                (g_hFont = CreateFont(FONT_HEIGHT, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Tahoma")))
        {
            const HINSTANCE hInstance = GetModuleHandle(0);
            WNDCLASS wndCl;
            wndCl.style = 0;
            wndCl.lpfnWndProc = WindowProc;
            wndCl.cbClsExtra = 0;
            wndCl.cbWndExtra = 0;
            wndCl.hInstance = hInstance;
            wndCl.hIcon = 0;
            wndCl.hCursor = 0;
            wndCl.hbrBackground = 0;
            wndCl.lpszMenuName = 0;
            wndCl.lpszClassName = CLASS_NAME;

            if (RegisterClass(&wndCl))
            {
                if ((g_hWnd = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST, CLASS_NAME, 0, WS_POPUP, 0, 0, 0, 0, 0, 0, hInstance, 0)))
                {
                    if (SetLayeredWindowAttributes(g_hWnd, 0, 0, LWA_COLORKEY))
                        return true;
                    DestroyWindow(g_hWnd);
                    UnregisterClass(CLASS_NAME, hInstance);
                    DeleteObject(g_hFont);
                }
                else
                {
                    UnregisterClass(CLASS_NAME, hInstance);
                    DeleteObject(g_hFont);
                }
            }
            else
                DeleteObject(g_hFont);
        }
    }
    else
    {
        DestroyWindow(g_hWnd);
        UnregisterClass(CLASS_NAME, GetModuleHandle(0));
        DeleteObject(g_hFont);
    }
    return false;
}

//-------------------------------------------------------------------------------------------------
void fMsg(const wchar_t *wMsg)
{
    switch (*wMsg)
    {
    case L'+':
    {
        wchar_t *wOk;
        unsigned int iVolume = wcstoul(wMsg+1, &wOk, 10);
        if (!(*wOk || errno) && iVolume > 0 && iVolume <= 100)
        {
            HMIXER hMixer;
            if (mixerOpen(&hMixer, 0, 0, 0, 0) == MMSYSERR_NOERROR)
            {
                HMIXEROBJ hMixerObj = static_cast<HMIXEROBJ>(static_cast<void*>(hMixer));
                MIXERLINE mixerLine;
                mixerLine.cbStruct = sizeof(MIXERLINE);
                mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
                if (mixerGetLineInfo(hMixerObj, &mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
                {
                    MIXERCONTROL mixerCtrl;
                    MIXERLINECONTROLS mixerLineCtrls;
                    mixerLineCtrls.cbStruct = sizeof(MIXERLINECONTROLS);
                    mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                    mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
                    mixerLineCtrls.cControls = 1;
                    mixerLineCtrls.cbmxctrl = sizeof(MIXERCONTROL);
                    mixerLineCtrls.pamxctrl = &mixerCtrl;
                    if (mixerGetLineControls(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                    {
                        MIXERCONTROLDETAILS_UNSIGNED mixerCtrlDtlsUns;
                        MIXERCONTROLDETAILS mixerCtrlDtls;
                        mixerCtrlDtls.cbStruct = sizeof(MIXERCONTROLDETAILS);
                        mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                        mixerCtrlDtls.cChannels = 1;
                        mixerCtrlDtls.hwndOwner = 0;
                        mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                        mixerCtrlDtls.paDetails = &mixerCtrlDtlsUns;
                        if (mixerGetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                        {
                            iVolume += (mixerCtrlDtlsUns.dwValue + 1)*100/MAX_VOL;
                            if (iVolume > 100)
                                iVolume = 100;
                            mixerCtrlDtlsUns.dwValue = iVolume*MAX_VOL/100;
                            if (mixerSetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                            {
                                mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                                mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
                                if (mixerGetLineControls(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                                {
                                    MIXERCONTROLDETAILS_BOOLEAN mixerCtrlDtlsBool;
                                    mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                                    mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                                    mixerCtrlDtls.paDetails = &mixerCtrlDtlsBool;
                                    if (mixerGetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                                    {
                                        wcscat(_ultow(iVolume, g_wVolume, 10), L"%");
                                        g_colRef = mixerCtrlDtlsBool.fValue ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                        InvalidateRect(g_hWnd, 0, FALSE);
                                        ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                        SetTimer(g_hWnd, 0, 1200, 0);
                                    }
                                }
                            }
                        }
                    }
                }
                mixerClose(hMixer);
            }
        }
        break;
    }

    case L'-':
    {
        wchar_t *wOk;
        int iVolume = wcstol(wMsg+1, &wOk, 10);
        if (!(*wOk || errno) && iVolume > 0 && iVolume <= 100)
        {
            HMIXER hMixer;
            if (mixerOpen(&hMixer, 0, 0, 0, 0) == MMSYSERR_NOERROR)
            {
                HMIXEROBJ hMixerObj = static_cast<HMIXEROBJ>(static_cast<void*>(hMixer));
                MIXERLINE mixerLine;
                mixerLine.cbStruct = sizeof(MIXERLINE);
                mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
                if (mixerGetLineInfo(hMixerObj, &mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
                {
                    MIXERCONTROL mixerCtrl;
                    MIXERLINECONTROLS mixerLineCtrls;
                    mixerLineCtrls.cbStruct = sizeof(MIXERLINECONTROLS);
                    mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                    mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
                    mixerLineCtrls.cControls = 1;
                    mixerLineCtrls.cbmxctrl = sizeof(MIXERCONTROL);
                    mixerLineCtrls.pamxctrl = &mixerCtrl;
                    if (mixerGetLineControls(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                    {
                        MIXERCONTROLDETAILS_UNSIGNED mixerCtrlDtlsUns;
                        MIXERCONTROLDETAILS mixerCtrlDtls;
                        mixerCtrlDtls.cbStruct = sizeof(MIXERCONTROLDETAILS);
                        mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                        mixerCtrlDtls.cChannels = 1;
                        mixerCtrlDtls.hwndOwner = 0;
                        mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                        mixerCtrlDtls.paDetails = &mixerCtrlDtlsUns;
                        if (mixerGetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                        {
                            iVolume = (mixerCtrlDtlsUns.dwValue + 1)*100/MAX_VOL - iVolume;
                            if (iVolume < 0)
                                iVolume = 0;
                            mixerCtrlDtlsUns.dwValue = iVolume*MAX_VOL/100;
                            if (mixerSetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                            {
                                mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                                mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
                                if (mixerGetLineControls(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                                {
                                    MIXERCONTROLDETAILS_BOOLEAN mixerCtrlDtlsBool;
                                    mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                                    mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                                    mixerCtrlDtls.paDetails = &mixerCtrlDtlsBool;
                                    if (mixerGetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                                    {
                                        wcscat(_itow(iVolume, g_wVolume, 10), L"%");
                                        g_colRef = mixerCtrlDtlsBool.fValue ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                        InvalidateRect(g_hWnd, 0, FALSE);
                                        ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                        SetTimer(g_hWnd, 0, 1200, 0);
                                    }
                                }
                            }
                        }
                    }
                }
                mixerClose(hMixer);
            }
        }
        break;
    }

    case L'=':
    {
        wchar_t *wOk;
        const unsigned int iVolume = wcstoul(wMsg+1, &wOk, 10);
        if (!(*wOk || errno) && iVolume <= 100)
        {
            HMIXER hMixer;
            if (mixerOpen(&hMixer, 0, 0, 0, 0) == MMSYSERR_NOERROR)
            {
                HMIXEROBJ hMixerObj = static_cast<HMIXEROBJ>(static_cast<void*>(hMixer));
                MIXERLINE mixerLine;
                mixerLine.cbStruct = sizeof(MIXERLINE);
                mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
                if (mixerGetLineInfo(hMixerObj, &mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
                {
                    MIXERCONTROL mixerCtrl;
                    MIXERLINECONTROLS mixerLineCtrls;
                    mixerLineCtrls.cbStruct = sizeof(MIXERLINECONTROLS);
                    mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                    mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
                    mixerLineCtrls.cControls = 1;
                    mixerLineCtrls.cbmxctrl = sizeof(MIXERCONTROL);
                    mixerLineCtrls.pamxctrl = &mixerCtrl;
                    if (mixerGetLineControls(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                    {
                        MIXERCONTROLDETAILS_UNSIGNED mixerCtrlDtlsUns;
                        mixerCtrlDtlsUns.dwValue = iVolume*MAX_VOL/100;
                        MIXERCONTROLDETAILS mixerCtrlDtls;
                        mixerCtrlDtls.cbStruct = sizeof(MIXERCONTROLDETAILS);
                        mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                        mixerCtrlDtls.cChannels = 1;
                        mixerCtrlDtls.hwndOwner = 0;
                        mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                        mixerCtrlDtls.paDetails = &mixerCtrlDtlsUns;
                        if (mixerSetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                        {
                            mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                            mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
                            if (mixerGetLineControls(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                            {
                                MIXERCONTROLDETAILS_BOOLEAN mixerCtrlDtlsBool;
                                mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                                mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                                mixerCtrlDtls.paDetails = &mixerCtrlDtlsBool;
                                if (mixerGetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                                {
                                    wcscat(_ultow(iVolume, g_wVolume, 10), L"%");
                                    g_colRef = mixerCtrlDtlsBool.fValue ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                    InvalidateRect(g_hWnd, 0, FALSE);
                                    ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                    SetTimer(g_hWnd, 0, 1200, 0);
                                }
                            }
                        }
                    }
                }
                mixerClose(hMixer);
            }
        }
        break;
    }

    case L'/':
    {
        ++wMsg;
        if (wcscmp(L"toggle-mute", wMsg) == 0)
        {
            HMIXER hMixer;
            if (mixerOpen(&hMixer, 0, 0, 0, 0) == MMSYSERR_NOERROR)
            {
                HMIXEROBJ hMixerObj = static_cast<HMIXEROBJ>(static_cast<void*>(hMixer));
                MIXERLINE mixerLine;
                mixerLine.cbStruct = sizeof(MIXERLINE);
                mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
                if (mixerGetLineInfo(hMixerObj, &mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR)
                {
                    MIXERCONTROL mixerCtrl;
                    MIXERLINECONTROLS mixerLineCtrls;
                    mixerLineCtrls.cbStruct = sizeof(MIXERLINECONTROLS);
                    mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                    mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
                    mixerLineCtrls.cControls = 1;
                    mixerLineCtrls.cbmxctrl = sizeof(MIXERCONTROL);
                    mixerLineCtrls.pamxctrl = &mixerCtrl;
                    if (mixerGetLineControls(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                    {
                        MIXERCONTROLDETAILS_BOOLEAN mixerCtrlDtlsBool;
                        MIXERCONTROLDETAILS mixerCtrlDtls;
                        mixerCtrlDtls.cbStruct = sizeof(MIXERCONTROLDETAILS);
                        mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                        mixerCtrlDtls.cChannels = 1;
                        mixerCtrlDtls.hwndOwner = 0;
                        mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                        mixerCtrlDtls.paDetails = &mixerCtrlDtlsBool;
                        if (mixerGetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                        {
                            mixerCtrlDtlsBool.fValue = !mixerCtrlDtlsBool.fValue;
                            if (mixerSetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                            {
                                mixerLineCtrls.dwLineID = mixerLine.dwLineID;
                                mixerLineCtrls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
                                if (mixerGetLineControls(hMixerObj, &mixerLineCtrls, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR)
                                {
                                    MIXERCONTROLDETAILS_UNSIGNED mixerCtrlDtlsUns;
                                    mixerCtrlDtls.dwControlID = mixerCtrl.dwControlID;
                                    mixerCtrlDtls.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                                    mixerCtrlDtls.paDetails = &mixerCtrlDtlsUns;
                                    if (mixerGetControlDetails(hMixerObj, &mixerCtrlDtls, MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
                                    {
                                        wcscat(_ultow((mixerCtrlDtlsUns.dwValue + 1)*100/MAX_VOL, g_wVolume, 10), L"%");
                                        g_colRef = mixerCtrlDtlsBool.fValue ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                        InvalidateRect(g_hWnd, 0, FALSE);
                                        ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                        SetTimer(g_hWnd, 0, 1200, 0);
                                    }
                                }
                            }
                        }
                    }
                }
                mixerClose(hMixer);
            }
        }
        else if (wcscmp(L"mute", wMsg) == 0)
            fSetMute(true);
        else if (wcscmp(L"unmute", wMsg) == 0)
            fSetMute(false);
        break;
    }
    }
}
