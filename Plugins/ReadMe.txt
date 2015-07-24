Add plugins to "appname_plugins" folder, e.g. HotKeys_plugins or HotKeys64_plugins.

CapsSwitch - Use CapsLock to switch keyboard layout around. If you want standard behavior - switching capitalized letter, press Alt+CapsLock. Any messages ignored.
ChangeLayout - Change layout keyboard for selected text between english and russian/ukrainian/belarussian/german characters.
SendInput - Input text string or emulate key press.
VolumeControl - Set/increase/decrease volume level or mute/unmute/toggle-mute volume for default soundcard.
VolumeControlXP - VolumeControl for Windows2000/XP.
WindowControl - Manage foreground window.

When writing your plugin:
- Use DllMain function if needed;
- Prototype export function for messages (if missing any messages ignored):
void fMsg(const wchar_t *wMsg)
- Not initialize COM. If need COM add empty export function:
void fNeedCom() {}
