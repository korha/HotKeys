//HotKeys: VolumeControl
#define _WIN32_WINNT _WIN32_IE_WINBLUE
#define WIN32_LEAN_AND_MEAN
#include <endpointvolume.h>
#include <mmdeviceapi.h>

#define EXPORT //__declspec(dllexport)

static constexpr const wchar_t *const g_wGuidClass = L"App::b749c579-152b-4b74-9bc2-e6948c47675c";
static constexpr const int g_iCellHeight = 144;
static constexpr const UINT g_iElapseTimer = 1200;
static HWND g_hWnd;
static COLORREF g_colRef;
static wchar_t g_wVolume[5];        //"100%`"
static bool g_bTimerActive = false;
static IID g_iidMMDeviceEnumerator;
static IID g_iidIMMDeviceEnumerator;
static IID g_iidIAudioEndpointVolume;

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
    IMMDeviceEnumerator *immDeviceEnumerator;
    if (CoCreateInstance(g_iidMMDeviceEnumerator, nullptr, CLSCTX_ALL, g_iidIMMDeviceEnumerator, reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
    {
        IMMDevice *immDeviceDefault;
        HRESULT hr = immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault);
        immDeviceEnumerator->Release();
        if (hr == S_OK)
        {
            IAudioEndpointVolume *iAudioEndpointVolume;
            hr = immDeviceDefault->Activate(g_iidIAudioEndpointVolume, CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&iAudioEndpointVolume));
            immDeviceDefault->Release();
            if (hr == S_OK)
            {
                if (iAudioEndpointVolume->SetMute(bMute, nullptr) == S_OK)        //quit if S_FALSE (already mute/unmute)
                {
                    FLOAT fLevel;
                    hr = iAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel);
                    iAudioEndpointVolume->Release();
                    if (hr == S_OK)
                    {
                        FUpdValue(fLevel*100.0f + 0.05f);        //0.05 add to avoid incorrect truncation
                        g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                        InvalidateRect(g_hWnd, nullptr, FALSE);
                        ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                        if (SetTimer(g_hWnd, 1, g_iElapseTimer, nullptr))
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
            IMMDeviceEnumerator *immDeviceEnumerator;
            if (CoCreateInstance(g_iidMMDeviceEnumerator, nullptr, CLSCTX_ALL, g_iidIMMDeviceEnumerator, reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
            {
                IMMDevice *immDeviceDefault;
                HRESULT hr = immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault);
                immDeviceEnumerator->Release();
                if (hr == S_OK)
                {
                    IAudioEndpointVolume *iAudioEndpointVolume;
                    hr = immDeviceDefault->Activate(g_iidIAudioEndpointVolume, CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&iAudioEndpointVolume));
                    immDeviceDefault->Release();
                    if (hr == S_OK)
                    {
                        WINBOOL bMute;
                        if (iAudioEndpointVolume->GetMute(&bMute) == S_OK)
                        {
                            bMute = bMute ? FALSE : TRUE;
                            if (iAudioEndpointVolume->SetMute(bMute, nullptr) == S_OK)
                            {
                                FLOAT fLevel;
                                hr = iAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel);
                                iAudioEndpointVolume->Release();
                                if (hr == S_OK)
                                {
                                    FUpdValue(fLevel*100.0f + 0.05f);        //0.05 - add to avoid incorrect truncation
                                    g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                    InvalidateRect(g_hWnd, nullptr, FALSE);
                                    ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                    if (SetTimer(g_hWnd, 1, g_iElapseTimer, nullptr))
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
        else if ((*wMsg == L'+' || *wMsg == L'-' || *wMsg == L'=') &&
                 (wMsg[1] == L'0' || wMsg[1] == L'1') &&
                 (wMsg[2] >= L'0' && wMsg[2] <= L'9') &&
                 (wMsg[3] >= L'0' && wMsg[3] <= L'9') &&
                 wMsg[4] == L'\0')
        {
            DWORD dwVolume = (wMsg[1] - L'0')*100 + (wMsg[2] - L'0')*10 + (wMsg[3] - L'0');
            if (dwVolume <= 100)
            {
                IMMDeviceEnumerator *immDeviceEnumerator;
                if (CoCreateInstance(g_iidMMDeviceEnumerator, nullptr, CLSCTX_ALL, g_iidIMMDeviceEnumerator, reinterpret_cast<LPVOID*>(&immDeviceEnumerator)) == S_OK)
                {
                    IMMDevice *immDeviceDefault;
                    HRESULT hr = immDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &immDeviceDefault);
                    immDeviceEnumerator->Release();
                    if (hr == S_OK)
                    {
                        IAudioEndpointVolume *iAudioEndpointVolume;
                        hr = immDeviceDefault->Activate(g_iidIAudioEndpointVolume, CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&iAudioEndpointVolume));
                        immDeviceDefault->Release();
                        if (hr == S_OK)
                        {
                            if (*wMsg == L'=')
                            {
                                if (iAudioEndpointVolume->SetMasterVolumeLevelScalar(dwVolume/100.0f, nullptr) == S_OK)
                                {
                                    WINBOOL bMute;
                                    hr = iAudioEndpointVolume->GetMute(&bMute);
                                    iAudioEndpointVolume->Release();
                                    if (hr == S_OK)
                                    {
                                        FUpdValue(dwVolume);
                                        g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                        InvalidateRect(g_hWnd, nullptr, FALSE);
                                        ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                        if (SetTimer(g_hWnd, 1, g_iElapseTimer, nullptr))
                                            g_bTimerActive = true;
                                    }
                                }
                                else
                                    iAudioEndpointVolume->Release();
                            }
                            else
                            {
                                FLOAT fLevel;
                                if (iAudioEndpointVolume->GetMasterVolumeLevelScalar(&fLevel) == S_OK)
                                {
                                    const DWORD dwValuePrev = fLevel*100.0f + 0.05f;        //0.05 add to avoid incorrect truncation
                                    if (*wMsg == L'+')
                                    {
                                        dwVolume += dwValuePrev;
                                        if (dwVolume > 100)
                                            dwVolume = 100;
                                    }
                                    else if (*wMsg == L'-')
                                    {
                                        if (dwValuePrev > dwVolume)
                                            dwVolume = dwValuePrev - dwVolume;
                                        else
                                            dwVolume = 0;
                                    }
                                    if (iAudioEndpointVolume->SetMasterVolumeLevelScalar(dwVolume/100.0f, nullptr) == S_OK)
                                    {
                                        WINBOOL bMute;
                                        hr = iAudioEndpointVolume->GetMute(&bMute);
                                        iAudioEndpointVolume->Release();
                                        if (hr == S_OK)
                                        {
                                            FUpdValue(dwVolume);
                                            g_colRef = bMute ? RGB(255, 0, 0) : RGB(0, 255, 0);
                                            InvalidateRect(g_hWnd, nullptr, FALSE);
                                            ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
                                            if (SetTimer(g_hWnd, 1, g_iElapseTimer, nullptr))
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
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
EXPORT void FNeedCom()
{}

//-------------------------------------------------------------------------------------------------
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved)
{
    //functions from user32.dll in DllMain is legal because main app load/unload this dll in manual (dynamical) mode
    static bool bOk = false;
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        if (lpvReserved == nullptr && ((GetVersion() & 0xFF) >= ((_WIN32_WINNT_VISTA >> 8) & 0xFF)))
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
                    g_iidMMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
                    g_iidIMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
                    g_iidIAudioEndpointVolume = __uuidof(IAudioEndpointVolume);
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
