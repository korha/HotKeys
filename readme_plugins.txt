When writing your plugin:
- Use DllMain function if needed;
- Prototype export function for messages
void fMsg(const wchar_t *wMsg) {/*code (may be empty)*/}
- Not initialize COM. If need COM add empty export function:
void fNeedCom() {}

Ready plugins. Remove unwanted plugins before running.

-------------------- CapsSwitch ------------------------------------------------
Switch the input language.
Bind only to CapsLock key with any message.

-------------------- SendInput -------------------------------------------------
Print text or keystrokes.

Messages:
%s% - one symbol. Print as is;
%string% - insert string or emulate keydown or keyup starting with the 3rd symbol.

1st symbol quoted keydown, 2nd symbol quoyed keyup.
E.g.: ><12345>Enter><Enter<abcdefghijk>Ctrl>>F1><F1<<Ctrl< - print "12345", press Enter, print "abcdefghijk" and send Ctrl+F1 keystroke.

Available key names (hex code or string):
0x01 - LeftButton  0x02 - RightButton   0x03 - Control-Break    0x04 - MiddleButton 
0x05 - XButton1    0x06 - XButton2      0x07                    0x08 - BackSpace
0x09 - Tab         0x0A                 0x0B                    0x0C - Clear
0x0D - Enter       0x0E                 0x0F                    0x10 - Shift
0x11 - Ctrl        0x12 - Alt           0x13 - Pause            0x14 - CapsLock
0x15 - Kana        0x16                 0x17 - Junja            0x18 - Final
0x19 - Hanja       0x1A                 0x1B - Esc              0x1C - Convert
0x1D - NonConvert  0x1E - Accept        0x1F - ModeChange       0x20 - Space
0x21 - PgUp        0x22 - PgDn          0x23 - End              0x24 - Home
0x25 - Left        0x26 - Up            0x27 - Right            0x28 - Down
0x29 - Select      0x2A - Print         0x2B - Execute          0x2C - PrtScr
0x2D - Ins         0x2E - Del           0x2F - Help             0x30 - 0
0x31 - 1           0x32 - 2             0x33 - 3                0x34 - 4
0x35 - 5           0x36 - 6             0x37 - 7                0x38 - 8
0x39 - 9           0x3A                 0x3B                    0x3C
0x3D               0x3E                 0x3F                    0x40
0x41 - A           0x42 - B             0x43 - C                0x44 - D
0x45 - E           0x46 - F             0x47 - G                0x48 - H
0x49 - I           0x4A - J             0x4B - K                0x4C - L
0x4D - M           0x4E - N             0x4F - O                0x50 - P
0x51 - Q           0x52 - R             0x53 - S                0x54 - T
0x55 - U           0x56 - V             0x57 - W                0x58 - X
0x59 - Y           0x5A - Z             0x5B - LWin             0x5C - RWin
0x5D - Apps        0x5E                 0x5F - Sleep            0x60 - Num0
0x61 - Num1        0x62 - Num2          0x63 - Num3             0x64 - Num4
0x65 - Num5        0x66 - Num6          0x67 - Num7             0x68 - Num8
0x69 - Num9        0x6A - Num*          0x6B - Num+             0x6C - Separator
0x6D - Num-        0x6E - Num.          0x6F - Num/             0x70 - F1
0x71 - F2          0x72 - F3            0x73 - F4               0x74 - F5
0x75 - F6          0x76 - F7            0x77 - F8               0x78 - F9
0x79 - F10         0x7A - F11           0x7B - F12              0x7C - F13
0x7D - F14         0x7E - F15           0x7F - F16              0x80 - F17
0x81 - F18         0x82 - F19           0x83 - F20              0x84 - F21
0x85 - F22         0x86 - F23           0x87 - F24              0x88
0x89               0x8A                 0x8B                    0x8C
0x8D               0x8E                 0x8F                    0x90 - NumLock
0x91 - ScrollLock  0x92                 0x93                    0x94
0x95               0x96                 0x97                    0x98
0x99               0x9A                 0x9B                    0x9C
0x9D               0x9E                 0x9F                    0xA0 - LShift
0xA1 - RShift      0xA2 - LCtrl         0xA3 - RCtrl            0xA4 - LAlt
0xA5 - RAlt        0xA6 - BrowserBack   0xA7 - BrowserForward   0xA8 - BrowserRefresh
0xA9 - BrowserStop 0xAA - BrowserSearch 0xAB - BrowserFavorites 0xAC - BrowserHome
0xAD - VolumeMute  0xAE - VolumeDown    0xAF - VolumeUp         0xB0 - MediaNext
0xB1 - MediaPrev   0xB2 - MediaStop     0xB3 - MediaPlayPause   0xB4 - StartMail
0xB5 - MediaSelect 0xB6 - StartApp1     0xB7 - StartApp2        0xB8
0xB9               0xBA - ;             0xBB - =                0xBC - ,
0xBD - -           0xBE - .             0xBF - /                0xC0 - `
0xC1               0xC2                 0xC3                    0xC4
0xC5               0xC6                 0xC7                    0xC8
0xC9               0xCA                 0xCB                    0xCC
0xCD               0xCE                 0xCF                    0xD0
0xD1               0xD2                 0xD3                    0xD4
0xD5               0xD6                 0xD7                    0xD8
0xD9               0xDA                 0xDB - [                0xDC - \
0xDD - ]           0xDE - '             0xDF                    0xE0
0xE1               0xE2                 0xE3                    0xE4
0xE5 - Process     0xE6                                         0xE8
0xE9               0xEA                 0xEB                    0xEC
0xED               0xEE                 0xEF                    0xF0
0xF1               0xF2                 0xF3                    0xF4
0xF5               0xF6 - Attn          0xF7 - CrSel            0xF8 - ExSel
0xF9 - ErEOF       0xFA - Play          0xFB - Zoom             0xFC
0xFD - PA1         0xFE - OemClear

-------------------- SwapChars -------------------------------------------------
Swap chars for selected text.
Specify swap rules in custom.h file before compiling.
All messages are composed of one symbol and specify in the custom.h file.
By default there are rules changes in layout between the Latin and Eastern European Cyrillic keyboard layout.

-------------------- VolumeControl ---------------------------------------------
Supported: WinVista+.
Control volume level for default sound card.

Messages:
+%%% - increase the volume level of %%% per cent of the maximum level;
-%%% - decrease the volume level of %%% per cent of the maximum level;
=%%% - set the volume to %%% per cent of the maximum level;
/mute - set the muting state of the audio stream;
/unmute - unset the muting state of the audio stream;
/toggle-mute - toggle set/unset the muting state of the audio stream.

%%% must be between 000 and 100;
-------------------- VolumeControlXP -------------------------------------------
Supported: WinXP.
Such as VolumeControl.

-------------------- WindowControl ---------------------------------------------
Control foreground window.

Messages:
/suspend - suspend all threads of process of foreground window;
/resume - resume all threads of process of foreground window;
/minimize - minimize foreground window;
/maximize - maximize foreground window;
/restore - restore foreground window;
/toggle-maximize - toggle maximize/restore foreground window;
/topmost - set always on top foreground window;
/notopmost - unset always on top foreground window;
/toggle-topmost - toggle topmost/notopmost mode foreground window;
/caption-off - hide caption window;
/caption-on - show caption window;
/toggle-caption - toggle hide/show caption window;
/resize - allows to change the window size;
/close - close foreground window;
/terminate - terminate owner windows process;
/opacity+%%% - increase opacity level of the foreground window by %%% units;
/opacity-%%% - decrease opacity level of the foreground window by %%% units;
/opacity=%&% - set opacity level of the foreground window in %%% units.

%%% must be between 000 and 255;
