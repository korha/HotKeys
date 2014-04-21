#include <windows.h>

#ifndef __MINGW64__
#include "header.h"
#endif

#include <endpointvolume.h>
#include <mmdeviceapi.h>

#define EXPORT //__declspec(dllexport)
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
    IMMDeviceEnumerator *immDeviceEnumerator;
    if (CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
    {
        IMMDevice *immDeviceDefault;
        if (immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault) == S_OK)
        {
            immDeviceEnumerator->Release();
            IAudioEndpointVolume *iAudioEndpointVolume;
            if (immDeviceDefault->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, 0, reinterpret_cast<void**>(&iAudioEndpointVolume)) == S_OK)
            {
                immDeviceDefault->Release();
                if (iAudioEndpointVolume->SetMute(bMute, 0) == S_OK)        //return S_FALSE - quit (already mute)
                {
                    FLOAT fLevel;
                    if (iAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel) == S_OK)
                    {
                        wcscat(_itow(fLevel*100.0f + 0.05f, g_wVolume, 10), L"%");        //[0.05 - add to avoid incorrect truncation]
                        g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                        InvalidateRect(g_hWnd, 0, FALSE);
                        ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                        SetTimer(g_hWnd, 0, 1200, 0);
                    }
                }
                iAudioEndpointVolume->Release();
            }
            else
                immDeviceDefault->Release();
        }
        else
            immDeviceEnumerator->Release();
    }
}

//-------------------------------------------------------------------------------------------------
bool EXPORT fInit(const bool bInit)
{
    if (bInit)
    {
        if (static_cast<BYTE>(GetVersion()) >= 6 &&
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
void EXPORT fMsg(const wchar_t *wMsg)
{
    switch (*wMsg)
    {
    case L'+':
    {
        wchar_t *wOk;
        int iVolume = wcstol(wMsg+1, &wOk, 10);
        if (!(*wOk || errno) && iVolume > 0 && iVolume <= 100)
        {
            IMMDeviceEnumerator *immDeviceEnumerator;
            if (CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
            {
                IMMDevice *immDeviceDefault;
                if (immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault) == S_OK)
                {
                    immDeviceEnumerator->Release();
                    IAudioEndpointVolume *iAudioEndpointVolume;
                    if (immDeviceDefault->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, 0, reinterpret_cast<void**>(&iAudioEndpointVolume)) == S_OK)
                    {
                        immDeviceDefault->Release();
                        FLOAT fLevel;
                        if (iAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel) == S_OK)
                        {
                            iVolume += static_cast<int>(fLevel*100.0f + 0.05f);        //[0.05 - add to avoid incorrect truncation]
                            if (iVolume > 100)
                                iVolume = 100;
                            if (iAudioEndpointVolume->SetMasterVolumeLevelScalar(iVolume/100.0f, 0) == S_OK)
                            {
                                WINBOOL bMute;
                                if (iAudioEndpointVolume->GetMute(&bMute) == S_OK)
                                {
                                    wcscat(_itow(iVolume, g_wVolume, 10), L"%");
                                    g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                    InvalidateRect(g_hWnd, 0, FALSE);
                                    ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                    SetTimer(g_hWnd, 0, 1200, 0);
                                }
                            }
                        }
                        iAudioEndpointVolume->Release();
                    }
                    else
                        immDeviceDefault->Release();
                }
                else
                    immDeviceEnumerator->Release();
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
            IMMDeviceEnumerator *immDeviceEnumerator;
            if (CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
            {
                IMMDevice *immDeviceDefault;
                if (immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault) == S_OK)
                {
                    immDeviceEnumerator->Release();
                    IAudioEndpointVolume *iAudioEndpointVolume;
                    if (immDeviceDefault->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, 0, reinterpret_cast<void**>(&iAudioEndpointVolume)) == S_OK)
                    {
                        immDeviceDefault->Release();
                        FLOAT fLevel;
                        if (iAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel) == S_OK)
                        {
                            iVolume = static_cast<int>(fLevel*100.0f + 0.05f) - iVolume;        //[0.05 - add to avoid incorrect truncation]
                            if (iVolume < 0)
                                iVolume = 0;
                            if (iAudioEndpointVolume->SetMasterVolumeLevelScalar(iVolume/100.0f, 0) == S_OK)
                            {
                                WINBOOL bMute;
                                if (iAudioEndpointVolume->GetMute(&bMute) == S_OK)
                                {
                                    wcscat(_itow(iVolume, g_wVolume, 10), L"%");
                                    g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                    InvalidateRect(g_hWnd, 0, FALSE);
                                    ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                    SetTimer(g_hWnd, 0, 1200, 0);
                                }
                            }
                        }
                        iAudioEndpointVolume->Release();
                    }
                    else
                        immDeviceDefault->Release();
                }
                else
                    immDeviceEnumerator->Release();
            }
        }
        break;
    }

    case L'=':
    {
        wchar_t *wOk;
        const int iVolume = wcstol(wMsg+1, &wOk, 10);
        if (!(*wOk || errno) && iVolume >= 0 && iVolume <= 100)
        {
            IMMDeviceEnumerator *immDeviceEnumerator;
            if (CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
            {
                IMMDevice *immDeviceDefault;
                if (immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault) == S_OK)
                {
                    immDeviceEnumerator->Release();
                    IAudioEndpointVolume *iAudioEndpointVolume;
                    if (immDeviceDefault->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, 0, reinterpret_cast<void**>(&iAudioEndpointVolume)) == S_OK)
                    {
                        immDeviceDefault->Release();
                        if (iAudioEndpointVolume->SetMasterVolumeLevelScalar(iVolume/100.0f, 0) == S_OK)
                        {
                            WINBOOL bMute;
                            if (iAudioEndpointVolume->GetMute(&bMute) == S_OK)
                            {
                                wcscat(_itow(iVolume, g_wVolume, 10), L"%");
                                g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                InvalidateRect(g_hWnd, 0, FALSE);
                                ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                SetTimer(g_hWnd, 0, 1200, 0);
                            }
                        }
                        iAudioEndpointVolume->Release();
                    }
                    else
                        immDeviceDefault->Release();
                }
                else
                    immDeviceEnumerator->Release();
            }
        }
        break;
    }

    case L'/':
    {
        ++wMsg;
        if (wcscmp(L"toggle-mute", wMsg) == 0)
        {
            IMMDeviceEnumerator *immDeviceEnumerator;
            if (CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
            {
                IMMDevice *immDeviceDefault;
                if (immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault) == S_OK)
                {
                    immDeviceEnumerator->Release();
                    IAudioEndpointVolume *iAudioEndpointVolume;
                    if (immDeviceDefault->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, 0, reinterpret_cast<void**>(&iAudioEndpointVolume)) == S_OK)
                    {
                        immDeviceDefault->Release();
                        WINBOOL bMute;
                        if (iAudioEndpointVolume->GetMute(&bMute) == S_OK)
                        {
                            bMute = !bMute;
                            if (iAudioEndpointVolume->SetMute(bMute, 0) == S_OK)
                            {
                                FLOAT fLevel;
                                if (iAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel) == S_OK)
                                {
                                    wcscat(_itow(fLevel*100.0f + 0.05f, g_wVolume, 10), L"%");        //[0.05 - add to avoid incorrect truncation]
                                    g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                    InvalidateRect(g_hWnd, 0, FALSE);
                                    ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                    SetTimer(g_hWnd, 0, 1200, 0);
                                }
                            }
                        }
                        iAudioEndpointVolume->Release();
                    }
                    else
                        immDeviceDefault->Release();
                }
                else
                    immDeviceEnumerator->Release();
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
