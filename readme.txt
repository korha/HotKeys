Setting global hotkeys to launch applications and perform actions (Windows).
Launch applications based on RegisterHotkey WinAPI function, perform actions by plugins, and GUI frontend to create configuration file.

Run parameters:
Core:
[%filepath%] - select configuration file or "/q" to close the running app.
    If not specified, the file is selected by default (identical filepath with cfg extension).
Settings:
[%filepath%] - select configuration file.
    If not specified, an attempt to open a <apppath>.cfg (HotKeys.exe.cfg) file in the same folder.

Remove unwanted plugins before running.
