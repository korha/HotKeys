#include <endpointvolume.h>
#include <mmdeviceapi.h>

#define EXPORT //__declspec(dllexport)

static const wchar_t *const g_wGuidClass = L"App::b749c579-152b-4b74-9bc2-e6948c47675c";

enum {eCellHeight = 144};

static HWND g_hWnd;
static COLORREF g_colRef;
static wchar_t g_wVolume[5];        //[100%\0]
static bool g_bTimerActive = false;

//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HFONT hFont;
    static PAINTSTRUCT ps;

    switch (uMsg)
    {
    case WM_CREATE:
        return ((hFont = CreateFont(eCellHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Tahoma")) &&
                SetLayeredWindowAttributes(hWnd, 0, 0, LWA_COLORKEY)) ? 0 : -1;
    case WM_PAINT:
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
                        rect.top = rect.bottom - (eCellHeight + eCellHeight/2);
                        SelectObject(hDcMem, hBmpMem);
                        SelectObject(hDcMem, hFont);
                        SetBkColor(hDcMem, RGB(1, 1, 1));
                        SetTextColor(hDcMem, g_colRef);
                        DrawText(hDcMem, g_wVolume, -1, &rect, DT_CENTER);
                        BitBlt(hDc, 0, 0, iHorzRes, iVertRes, hDcMem, 0, 0, SRCCOPY);
                    }
                    DeleteObject(hBmpMem);
                }
                DeleteDC(hDcMem);
            }
            EndPaint(hWnd, &ps);
        }
        return 0;
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
        g_hWnd = 0;
        return 0;
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved)
{
    static bool bOk = false;
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        if (lpvReserved == 0/*dynamic linking*/ && (GetVersion() & 0xFF) >= 6)        //WinVista+ (NT6+)
        {
            WNDCLASSEX wndCl;
            memset(&wndCl, 0, sizeof(WNDCLASSEX));
            wndCl.cbSize = sizeof(WNDCLASSEX);
            wndCl.lpfnWndProc = WindowProc;
            wndCl.hInstance = hInstDll;
            wndCl.lpszClassName = g_wGuidClass;

            if (RegisterClassEx(&wndCl))        //***it's ok
            {
                if ((g_hWnd = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST, g_wGuidClass, 0, WS_POPUP, 0, 0, 0, 0, 0, 0, hInstDll, 0)))
                {
                    bOk = true;
                    return TRUE;
                }
                UnregisterClass(g_wGuidClass, hInstDll);        //***it's ok
            }
        }
    }
    else if (fdwReason == DLL_PROCESS_DETACH && bOk)
    {
        if (g_hWnd)
            SendMessage(g_hWnd, WM_CLOSE, 0, 0);
        UnregisterClass(g_wGuidClass, hInstDll);        //***it's ok
    }
    return FALSE;
}

//-------------------------------------------------------------------------------------------------
void fUpdValue(const int iValue)
{
    if (iValue < 10)
    {
        g_wVolume[0] = L'0' + iValue;
        g_wVolume[1] = L'%';
        g_wVolume[2] = L'\0';
    }
    else if (iValue < 100)
    {
        g_wVolume[0] = L'0' + iValue/10;
        g_wVolume[1] = L'0' + iValue%10;
        g_wVolume[2] = L'%';
        g_wVolume[3] = L'\0';
    }
    else
        wcscpy(g_wVolume, L"100%");
}

//-------------------------------------------------------------------------------------------------
void fSetMute(const bool bMute)
{
    IMMDeviceEnumerator *immDeviceEnumerator;
    if (CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
    {
        IMMDevice *immDeviceDefault;
        HRESULT hr = immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault);
        immDeviceEnumerator->Release();
        if (hr == S_OK)
        {
            IAudioEndpointVolume *iAudioEndpointVolume;
            hr = immDeviceDefault->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, 0, reinterpret_cast<void**>(&iAudioEndpointVolume));
            immDeviceDefault->Release();
            if (hr == S_OK)
            {
                if (iAudioEndpointVolume->SetMute(bMute, 0) == S_OK)        //quit if S_FALSE (already mute)
                {
                    FLOAT fLevel;
                    hr = iAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel);
                    iAudioEndpointVolume->Release();
                    if (hr == S_OK)
                    {
                        fUpdValue(fLevel*100.0f + 0.05f);        //[0.05 - add to avoid incorrect truncation]
                        g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                        InvalidateRect(g_hWnd, 0, FALSE);
                        ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                        if (SetTimer(g_hWnd, 1, 1200, 0))
                            g_bTimerActive = true;
                    }
                }
                else
                    iAudioEndpointVolume->Release();
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
EXPORT void fMsg(const wchar_t *wMsg)
{
    if (g_hWnd && wMsg)
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
                if (CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
                {
                    IMMDevice *immDeviceDefault;
                    HRESULT hr = immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault);
                    immDeviceEnumerator->Release();
                    if (hr == S_OK)
                    {
                        IAudioEndpointVolume *iAudioEndpointVolume;
                        hr = immDeviceDefault->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, 0, reinterpret_cast<void**>(&iAudioEndpointVolume));
                        immDeviceDefault->Release();
                        if (hr == S_OK)
                        {
                            FLOAT fLevel;
                            if (iAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel) == S_OK)
                            {
                                iVolume += static_cast<int>(fLevel*100.0f + 0.05f);        //[0.05 - add to avoid incorrect truncation]
                                if (iVolume > 100)
                                    iVolume = 100;
                                if (iAudioEndpointVolume->SetMasterVolumeLevelScalar(iVolume/100.0f, 0) == S_OK)
                                {
                                    WINBOOL bMute;
                                    hr = iAudioEndpointVolume->GetMute(&bMute);
                                    iAudioEndpointVolume->Release();
                                    if (hr == S_OK)
                                    {
                                        fUpdValue(iVolume);
                                        g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                        InvalidateRect(g_hWnd, 0, FALSE);
                                        ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                        if (SetTimer(g_hWnd, 1, 1200, 0))
                                            g_bTimerActive = true;
                                    }
                                }
                                else
                                    iAudioEndpointVolume->Release();
                            }
                            else
                                iAudioEndpointVolume->Release();
                        }
                    }
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
                if (CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
                {
                    IMMDevice *immDeviceDefault;
                    HRESULT hr = immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault);
                    immDeviceEnumerator->Release();
                    if (hr == S_OK)
                    {
                        IAudioEndpointVolume *iAudioEndpointVolume;
                        hr = immDeviceDefault->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, 0, reinterpret_cast<void**>(&iAudioEndpointVolume));
                        immDeviceDefault->Release();
                        if (hr == S_OK)
                        {
                            FLOAT fLevel;
                            if (iAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel) == S_OK)
                            {
                                iVolume = static_cast<int>(fLevel*100.0f + 0.05f) - iVolume;        //[0.05 - add to avoid incorrect truncation]
                                if (iVolume < 0)
                                    iVolume = 0;
                                if (iAudioEndpointVolume->SetMasterVolumeLevelScalar(iVolume/100.0f, 0) == S_OK)
                                {
                                    WINBOOL bMute;
                                    hr = iAudioEndpointVolume->GetMute(&bMute);
                                    iAudioEndpointVolume->Release();
                                    if (hr == S_OK)
                                    {
                                        fUpdValue(iVolume);
                                        g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                        InvalidateRect(g_hWnd, 0, FALSE);
                                        ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                        if (SetTimer(g_hWnd, 1, 1200, 0))
                                            g_bTimerActive = true;
                                    }
                                }
                                else
                                    iAudioEndpointVolume->Release();
                            }
                            else
                                iAudioEndpointVolume->Release();
                        }
                    }
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
                if (CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
                {
                    IMMDevice *immDeviceDefault;
                    HRESULT hr = immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault);
                    immDeviceEnumerator->Release();
                    if (hr == S_OK)
                    {
                        IAudioEndpointVolume *iAudioEndpointVolume;
                        hr = immDeviceDefault->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, 0, reinterpret_cast<void**>(&iAudioEndpointVolume));
                        immDeviceDefault->Release();
                        if (hr == S_OK)
                        {
                            if (iAudioEndpointVolume->SetMasterVolumeLevelScalar(iVolume/100.0f, 0) == S_OK)
                            {
                                WINBOOL bMute;
                                hr = iAudioEndpointVolume->GetMute(&bMute);
                                iAudioEndpointVolume->Release();
                                if (hr == S_OK)
                                {
                                    fUpdValue(iVolume);
                                    g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                    InvalidateRect(g_hWnd, 0, FALSE);
                                    ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                    if (SetTimer(g_hWnd, 1, 1200, 0))
                                        g_bTimerActive = true;
                                }
                            }
                            else
                                iAudioEndpointVolume->Release();
                        }
                    }
                }
            }
            break;
        }

        case L'/':
        {
            ++wMsg;
            if (wcscmp(wMsg, L"toggle-mute") == 0)
            {
                IMMDeviceEnumerator *immDeviceEnumerator;
                if (CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
                {
                    IMMDevice *immDeviceDefault;
                    HRESULT hr = immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault);
                    immDeviceEnumerator->Release();
                    if (hr == S_OK)
                    {
                        IAudioEndpointVolume *iAudioEndpointVolume;
                        hr = immDeviceDefault->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, 0, reinterpret_cast<void**>(&iAudioEndpointVolume));
                        immDeviceDefault->Release();
                        if (hr == S_OK)
                        {
                            WINBOOL bMute;
                            if (iAudioEndpointVolume->GetMute(&bMute) == S_OK)
                            {
                                bMute = bMute ? FALSE : TRUE;
                                if (iAudioEndpointVolume->SetMute(bMute, 0) == S_OK)
                                {
                                    FLOAT fLevel;
                                    hr = iAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel);
                                    iAudioEndpointVolume->Release();
                                    if (hr == S_OK)
                                    {
                                        fUpdValue(fLevel*100.0f + 0.05f);        //[0.05 - add to avoid incorrect truncation]
                                        g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                        InvalidateRect(g_hWnd, 0, FALSE);
                                        ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                        if (SetTimer(g_hWnd, 1, 1200, 0))
                                            g_bTimerActive = true;
                                    }
                                }
                                else
                                    iAudioEndpointVolume->Release();
                            }
                            else
                                iAudioEndpointVolume->Release();
                        }
                    }
                }
            }
            else if (wcscmp(wMsg, L"mute") == 0)
                fSetMute(true);
            else if (wcscmp(wMsg, L"unmute") == 0)
                fSetMute(false);
            break;
        }
        }
    }
}

//-------------------------------------------------------------------------------------------------
EXPORT void fNeedCom() {}
