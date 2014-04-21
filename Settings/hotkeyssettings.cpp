#include "hotkeyssettings.h"

#define MAIN_APP_NAME "HotKeys"
#define GUID_CLASS L"b6c9d088-c4a5-45f2-8481-87b29bcaec50"

#ifdef QT_DEBUG
#define LIB_GUI L"Qt5Guid"
#else
#define LIB_GUI L"Qt5Gui"
#endif

#define FUNC_FROM_WIN_HICON "_Z21qt_pixmapFromWinHICONP7HICON__"

#define ROLE_TYPE_ISMSG (Qt::UserRole+1)
#define ROLE_MODIFIERS  (Qt::UserRole+2)
#define ROLE_VIRTUALKEY (Qt::UserRole+3)
#define ROLE_FILE       (Qt::UserRole+4)
#define ROLE_PLUGIN     (Qt::UserRole+4)
#define ROLE_PARAM      (Qt::UserRole+5)
#define ROLE_MESSAGE    (Qt::UserRole+5)
#define ROLE_WORKDIR    (Qt::UserRole+6)
#define ROLE_SHOWMODE   (Qt::UserRole+7)

//-------------------------------------------------------------------------------------------------
ListViewEx::ListViewEx(HotKeysSettings *parent) : QListView(parent)
{
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setDefaultDropAction(Qt::IgnoreAction);
}

//-------------------------------------------------------------------------------------------------
void ListViewEx::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        if (QFileInfo(event->mimeData()->urls().first().toLocalFile()).isFile())
            event->acceptProposedAction();
    }
    else
        this->QListView::dragEnterEvent(event);
}

//-------------------------------------------------------------------------------------------------
void ListViewEx::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        Q_ASSERT(QFileInfo(event->mimeData()->urls().first().toLocalFile()).isFile());
        HotKeysSettings::pApplication->slotNewCfg();
        HotKeysSettings::pApplication->fOpenCfg(event->mimeData()->urls().first().toLocalFile());
    }
    else
        this->QListView::dropEvent(event);
}

//-------------------------------------------------------------------------------------------------
HotKeysSettings::HotKeysSettings() : QWidget(),
    fFromWinHICON(0),
    leNewKeyComb(0),
    leMsg(0),
    sitemModel(new QStandardItemModel(this)),
    iModifiersTemp(0),
    iModifiers(0),
    iVirtualKey(0),
    bActive(::FindWindowEx(HWND_MESSAGE, 0, GUID_CLASS, 0)),
    hHook(::SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, qWinAppInst(), 0))
{
    Q_ASSERT((MOD_WIN | MOD_CONTROL | MOD_ALT | MOD_SHIFT) == 0xF);
    Q_ASSERT(SW_HIDE == 0 &&
             SW_SHOWNORMAL == 1 &&
             SW_SHOWMINIMIZED == 2 &&
             SW_MAXIMIZE == 3 && SW_SHOWMAXIMIZED == 3 &&
             SW_SHOWNOACTIVATE == 4 &&
             SW_SHOW == 5 &&
             SW_MINIMIZE == 6 &&
             SW_SHOWMINNOACTIVE == 7 &&
             SW_SHOWNA == 8 &&
             SW_RESTORE == 9 &&
             SW_SHOWDEFAULT == 10);

    if (const HMODULE hMod = ::GetModuleHandle(LIB_GUI))
        fFromWinHICON = reinterpret_cast<PFromWinHICON>(::GetProcAddress(hMod, FUNC_FROM_WIN_HICON));
    if (!fFromWinHICON)
    {
        Q_ASSERT(false);
        fFromWinHICON = &fNullPixmap;
    }

    //strModifiers[0] = "";
    strModifiers[1] = "Alt+";
    strModifiers[2] = "Ctrl+";
    strModifiers[3] = "Ctrl+Alt+";
    strModifiers[4] = "Shift+";
    strModifiers[5] = "Alt+Shift+";
    strModifiers[6] = "Ctrl+Shift+";
    strModifiers[7] = "Ctrl+Alt+Shift+";
    strModifiers[8] = "Win+";
    strModifiers[9] = "Win+Alt+";
    strModifiers[10] = "Win+Ctrl+";
    strModifiers[11] = "Win+Ctrl+Alt+";
    strModifiers[12] = "Win+Shift+";
    strModifiers[13] = "Win+Alt+Shift+";
    strModifiers[14] = "Win+Ctrl+Shift+";
    strModifiers[15] = "Win+Ctrl+Alt+Shift+";

    QPushButton *pbNewCfg = new QPushButton(QIcon(":/img/new.png"), 0, this);
    pbNewCfg->setToolTip(tr("Create New File"));
    QPushButton *pbOpenCfg = new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton), tr("Open"), this);
    pbSaveCfg = new QPushButton(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("Save"), this);
    pbSaveCfg->setEnabled(false);
    pbSaveAsCfg = new QPushButton(style()->standardIcon(QStyle::SP_DialogSaveButton), 0, this);
    pbSaveAsCfg->setToolTip(tr("Save As..."));
    pbSaveAsCfg->setEnabled(false);
    lblInfo = new QLabel(this);
    QFont fontSaved = lblInfo->font();
    fontSaved.setItalic(true);
    lblInfo->setFont(fontSaved);
    QPushButton *pbRestart = new QPushButton(style()->standardIcon(QStyle::SP_CommandLink), 0, this);
    pbRestart->setToolTip(tr("Start (Restart)"));
    QPushButton *pbStop = new QPushButton(style()->standardIcon(QStyle::SP_BrowserStop), 0, this);
    pbStop->setToolTip(tr("Stop"));
    QHBoxLayout *hblTop = new QHBoxLayout;
    hblTop->setSpacing(2);
    hblTop->addWidget(pbNewCfg);
    hblTop->addWidget(pbOpenCfg);
    hblTop->addWidget(pbSaveCfg);
    hblTop->addWidget(pbSaveAsCfg);
    hblTop->addWidget(lblInfo);
    hblTop->addStretch();
    hblTop->addWidget(pbRestart);
    hblTop->addWidget(pbStop);

    listView = new ListViewEx(this);
    listView->setModel(sitemModel);

    QLabel *lblKeyComb = new QLabel(tr("Key Combination:"), this);
    leKeyComb = new QLineEdit(this);
    leKeyComb->setContextMenuPolicy(Qt::NoContextMenu);
    leKeyComb->setAcceptDrops(false);
    tbKeyComb = new QToolButton(this);
    tbKeyComb->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    tbKeyComb->setToolTip(tr("Change key combination"));
    tbKeyComb->setEnabled(false);
    QFrame *frmKeyComb = new QFrame(this);
    frmKeyComb->setFrameStyle(QFrame::StyledPanel);
    QHBoxLayout *hblKeyComb = new QHBoxLayout(frmKeyComb);
    hblKeyComb->setContentsMargins(9, 7, 9, 7);
    hblKeyComb->setSpacing(3);
    hblKeyComb->addStretch();
    hblKeyComb->addWidget(lblKeyComb);
    hblKeyComb->addWidget(leKeyComb);
    hblKeyComb->addWidget(tbKeyComb);
    hblKeyComb->addStretch();

    rbTypeFile = new QRadioButton(tr("Run File"), this);
    rbTypeFile->setChecked(true);
    rbTypeMsg = new QRadioButton(tr("Send Message"), this);
    QHBoxLayout *hblType = new QHBoxLayout;
    hblType->addWidget(rbTypeFile, 0, Qt::AlignRight);
    hblType->addWidget(rbTypeMsg, 0, Qt::AlignLeft);

    QLabel *lblFile = new QLabel(tr("File:"), this);
    leFile = new QLineEdit(this);
    QToolButton *tbFile = new QToolButton(this);
    tbFile->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    QHBoxLayout *hblFile = new QHBoxLayout;
    hblFile->setSpacing(3);
    hblFile->addWidget(leFile);
    hblFile->addWidget(tbFile);

    QLabel *lblParam = new QLabel(tr("Parameters:"), this);
    leParam = new QLineEdit(this);

    QLabel *lblWorkDir = new QLabel(tr("Working Directory:"), this);
    leWorkDir = new QLineEdit(this);
    QToolButton *tbWorkDir = new QToolButton(this);
    tbWorkDir->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    QHBoxLayout *hblWorkDir = new QHBoxLayout;
    hblWorkDir->setSpacing(3);
    hblWorkDir->addWidget(leWorkDir);
    hblWorkDir->addWidget(tbWorkDir);

    QLabel *lblShowMode = new QLabel(tr("Show Mode:"), this);
    cbShowMode = new QComboBox(this);
    cbShowMode->setMaxVisibleItems(11);
    cbShowMode->addItems(QStringList()
                         << tr("Hide")
                         << tr("Normal state")
                         << tr("Minimize")
                         << tr("Maximize")
                         << tr("Most recent state without activation")
                         << tr("Current state")
                         << tr("Minimize and activates next window")
                         << tr("Minimize without activation")
                         << tr("Current state without activation")
                         << tr("Restore")
                         << tr("Default"));
    cbShowMode->setCurrentIndex(SW_SHOWNORMAL);

    QWidget *wgtTypeFile = new QWidget(this);
    QGridLayout *glTypeFile = new QGridLayout(wgtTypeFile);
    glTypeFile->setContentsMargins(0, 0, 0, 0);
    glTypeFile->addWidget(lblFile, 0, 0);
    glTypeFile->addLayout(hblFile, 0, 1);
    glTypeFile->addWidget(lblParam, 1, 0);
    glTypeFile->addWidget(leParam, 1, 1);
    glTypeFile->addWidget(lblWorkDir, 2, 0);
    glTypeFile->addLayout(hblWorkDir, 2, 1);
    glTypeFile->addWidget(lblShowMode, 3, 0);
    glTypeFile->addWidget(cbShowMode, 3, 1, Qt::AlignLeft);

    slistPlugins = QDir(MAIN_APP_NAME "_plugins").entryList(QStringList("*.dll"), QDir::Files);

    QFrame *frmEntry = new QFrame(this);
    frmEntry->setFrameStyle(QFrame::StyledPanel);
    QVBoxLayout *vblEntry = new QVBoxLayout(frmEntry);
    vblEntry->addLayout(hblType);

    if (slistPlugins.isEmpty())
    {
        rbTypeMsg->setEnabled(false);
        vblEntry->addWidget(wgtTypeFile);
    }
    else
    {
        QLabel *lblPlugin = new QLabel(tr("Plugin:"), this);
        cbPlugin = new QComboBox(this);
        cbPlugin->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        cbPlugin->addItems(slistPlugins);
        cbPlugin->setMaximumWidth(cbPlugin->minimumSizeHint().width());
        QLabel *lblMsg = new QLabel(tr("Message:"), this);
        leMsg = new QLineEdit(this);
        QWidget *wgtTypeMsg = new QWidget(this);
        QGridLayout *glTypeMsg = new QGridLayout(wgtTypeMsg);
        glTypeMsg->setContentsMargins(0, 0, 0, 0);
        glTypeMsg->addWidget(lblPlugin, 0, 0);
        glTypeMsg->addWidget(cbPlugin, 0, 1, Qt::AlignHCenter);
        glTypeMsg->addWidget(lblMsg, 1, 0);
        glTypeMsg->addWidget(leMsg, 1, 1);

        stackWgt = new QStackedWidget(this);
        stackWgt->addWidget(wgtTypeFile);
        stackWgt->addWidget(wgtTypeMsg);
        vblEntry->addWidget(stackWgt);

        connect(rbTypeMsg, SIGNAL(toggled(bool)), this, SLOT(slotChangeType(bool)));
    }

    QPushButton *pbOk = new QPushButton(style()->standardIcon(QStyle::SP_DialogApplyButton), "OK", this);
    QPushButton *pbDel = new QPushButton(style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Delete"), this);
    QHBoxLayout *hblOkDel = new QHBoxLayout;
    hblOkDel->addStretch();
    hblOkDel->addWidget(pbOk);
    hblOkDel->addWidget(pbDel);

    vblEntry->addLayout(hblOkDel);

    QWidget *wgtRight = new QWidget(this);
    QVBoxLayout *vblRight = new QVBoxLayout(wgtRight);
    vblRight->setContentsMargins(0, 0, 0, 0);
    vblRight->addWidget(frmKeyComb);
    vblRight->addWidget(frmEntry, 0, Qt::AlignTop);
    vblRight->addStretch();

    splitter = new QSplitter(this);
    splitter->addWidget(listView);
    splitter->addWidget(wgtRight);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    QVBoxLayout *vblMain = new QVBoxLayout(this);
    vblMain->setContentsMargins(5, 5, 5, 5);
    vblMain->addLayout(hblTop);
    vblMain->addWidget(splitter);

    QTimer *timer = new QTimer(this);
    timer->start(3000);

    const QString strAppName = qAppName();
    leKeyComb->setFixedWidth(leKeyComb->fontMetrics().width("Win+Ctrl+Alt+Shift+BrowserFavorites") + 10);        //max width
    this->setWindowTitle(strAppName + " [?]");
    this->setWindowIcon(QIcon(bActive ? ":/img/on.png" : ":/img/off.png"));
    pApplication = this;

    //connects
    connect(pbNewCfg, SIGNAL(clicked()), this, SLOT(slotNewCfg()));
    connect(pbOpenCfg, SIGNAL(clicked()), this, SLOT(slotOpenCfg()));
    connect(pbSaveCfg, SIGNAL(clicked()), this, SLOT(slotSaveCfg()));
    connect(pbSaveAsCfg, SIGNAL(clicked()), this, SLOT(slotSaveAsCfg()));
    if (QFileInfo(MAIN_APP_NAME ".exe").exists())
        connect(pbRestart, SIGNAL(clicked()), this, SLOT(slotRestart()));
    else
        pbRestart->setEnabled(false);
    connect(pbStop, SIGNAL(clicked()), this, SLOT(slotStop()));
    connect(tbFile, SIGNAL(clicked()), this, SLOT(slotChangeFile()));
    connect(tbWorkDir, SIGNAL(clicked()), this, SLOT(slotChangeWorkDir()));
    connect(pbOk, SIGNAL(clicked()), this, SLOT(slotOk()));
    connect(pbDel, SIGNAL(clicked()), this, SLOT(slotDel()));
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerActive()));
    connect(listView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentChanged(QModelIndex)));

    //settings
    const QSettings stg("UserPrograms", strAppName);
    if (this->restoreGeometry(stg.value("Geometry").toByteArray()))
        splitter->restoreState(stg.value("State").toByteArray());

    if (hHook)
        connect(tbKeyComb, SIGNAL(clicked()), this, SLOT(slotChangeKeyComb()));
    else
    {
        leKeyComb->setEnabled(false);
        tbKeyComb->setVisible(false);
        QMessageBox::warning(0, 0, tr("Error:") + " SetWindowsHookEx");
    }

    //arguments
    const QStringList slistArgs = qApp->arguments();
    if (slistArgs.size() > 1)
    {
        if (QFileInfo(slistArgs.at(1)).isFile())
            fOpenCfg(slistArgs.at(1));
    }
    else if (QFileInfo(MAIN_APP_NAME ".cfg").isFile())
        fOpenCfg(MAIN_APP_NAME ".cfg");
}

//-------------------------------------------------------------------------------------------------
HotKeysSettings::~HotKeysSettings()
{
    if (hHook)
        ::UnhookWindowsHookEx(hHook);
    QSettings stg("UserPrograms", qAppName());
    stg.setValue("Geometry", this->saveGeometry());
    stg.setValue("State", splitter->saveState());
}

//-------------------------------------------------------------------------------------------------
HotKeysSettings *HotKeysSettings::pApplication;

//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK HotKeysSettings::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    Q_ASSERT(wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN || wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
    return ((nCode == HC_ACTION &&
             pApplication->fKeyboardProc(wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN, reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam)->vkCode)
             ) ? 1 : ::CallNextHookEx(0, nCode, wParam, lParam));
}

//-------------------------------------------------------------------------------------------------
QPixmap HotKeysSettings::fNullPixmap(HICON)
{
    return QPixmap();
}

//-------------------------------------------------------------------------------------------------
bool HotKeysSettings::fKeyboardProc(const bool bKeyDown, const DWORD dwKey)
{
    if (leKeyComb->hasFocus())
    {
        if (bKeyDown)
            switch (dwKey)
            {
            case VK_LWIN:     case VK_RWIN:                      iModifiersTemp |= MOD_WIN;     break;
            case VK_LCONTROL: case VK_RCONTROL: case VK_CONTROL: iModifiersTemp |= MOD_CONTROL; break;
            case VK_LMENU:    case VK_RMENU:    case VK_MENU:    iModifiersTemp |= MOD_ALT;     break;
            case VK_LSHIFT:   case VK_RSHIFT:   case VK_SHIFT:   iModifiersTemp |= MOD_SHIFT;   break;
            default:
            {
                const QString strKey = fKeyToString(dwKey);
                if (!strKey.isEmpty())
                {
                    iVirtualKey = dwKey;
                    iModifiers = iModifiersTemp;
                    leKeyComb->setText(strModifiers[iModifiers] + strKey);
                    for (int i = 0, iRowCount = sitemModel->rowCount(); i < iRowCount; ++i)
                    {
                        const QModelIndex modInd = sitemModel->index(i, 0);
                        if (modInd.data(ROLE_VIRTUALKEY).toInt() == iVirtualKey && modInd.data(ROLE_MODIFIERS).toInt() == iModifiers)
                        {
                            listView->setCurrentIndex(modInd);
                            tbKeyComb->setEnabled(true);
                            return true;
                        }
                    }
                    tbKeyComb->setEnabled(false);
                    leFile->clear();
                    leParam->clear();
                    leWorkDir->clear();
                    cbShowMode->setCurrentIndex(SW_SHOWNORMAL);
                    if (leMsg)
                        leMsg->clear();
                }
            }
            }
        else
            switch (dwKey)
            {
            case VK_LWIN:     case VK_RWIN:                      iModifiersTemp &= ~MOD_WIN;     break;
            case VK_LCONTROL: case VK_RCONTROL: case VK_CONTROL: iModifiersTemp &= ~MOD_CONTROL; break;
            case VK_LMENU:    case VK_RMENU:    case VK_MENU:    iModifiersTemp &= ~MOD_ALT;     break;
            case VK_LSHIFT:   case VK_RSHIFT:   case VK_SHIFT:   iModifiersTemp &= ~MOD_SHIFT;   break;
            }
        return true;
    }
    else if (leNewKeyComb && leNewKeyComb->hasFocus())
    {
        if (bKeyDown)
            switch (dwKey)
            {
            case VK_LWIN:     case VK_RWIN:                      iNewModifiersTemp |= MOD_WIN;     break;
            case VK_LCONTROL: case VK_RCONTROL: case VK_CONTROL: iNewModifiersTemp |= MOD_CONTROL; break;
            case VK_LMENU:    case VK_RMENU:    case VK_MENU:    iNewModifiersTemp |= MOD_ALT;     break;
            case VK_LSHIFT:   case VK_RSHIFT:   case VK_SHIFT:   iNewModifiersTemp |= MOD_SHIFT;   break;
            default:
            {
                const QString strKey = fKeyToString(dwKey);
                if (!strKey.isEmpty())
                {
                    iNewVirtualKey = dwKey;
                    iNewModifiers = iNewModifiersTemp;
                    leNewKeyComb->setText(strModifiers[iNewModifiers] + strKey);
                }
            }
            }
        else
            switch (dwKey)
            {
            case VK_LWIN:     case VK_RWIN:                      iNewModifiersTemp &= ~MOD_WIN;     break;
            case VK_LCONTROL: case VK_RCONTROL: case VK_CONTROL: iNewModifiersTemp &= ~MOD_CONTROL; break;
            case VK_LMENU:    case VK_RMENU:    case VK_MENU:    iNewModifiersTemp &= ~MOD_ALT;     break;
            case VK_LSHIFT:   case VK_RSHIFT:   case VK_SHIFT:   iNewModifiersTemp &= ~MOD_SHIFT;   break;
            }
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------------------------------
const char* HotKeysSettings::fKeyToString(const DWORD dwKey) const
{
    switch (dwKey)
    {
    case 0x01: return "LeftButton";
    case 0x02: return "RightButton";
    case 0x03: return "Control-Break";
    case 0x04: return "MiddleButton";
    case 0x05: return "XButton1";
    case 0x06: return "XButton2";
    case 0x07: return "0x07";
    case 0x08: return "BackSpace";
    case 0x09: return "Tab";
    case 0x0A: return "0x0A";
    case 0x0B: return "0x0B";
    case 0x0C: return "Clear";
    case 0x0D: return "Enter";
    case 0x0E: return "0x0E";
    case 0x0F: return "0x0F";
        //case 0x10: return "Shift";        //modifier
        //case 0x11: return "Ctrl";        //modifier
        //case 0x12: return "Alt";        //modifier
    case 0x13: return "Pause";
    case 0x14: return "CapsLock";
    case 0x15: return "Kana";
    case 0x16: return "0x16";
    case 0x17: return "Junja";
    case 0x18: return "Final";
    case 0x19: return "Hanja";
    case 0x1A: return "0x1A";
    case 0x1B: return "Esc";
    case 0x1C: return "Convert";
    case 0x1D: return "NonConvert";
    case 0x1E: return "Accept";
    case 0x1F: return "ModeChange";
    case 0x20: return "Space";
    case 0x21: return "PgUp";
    case 0x22: return "PgDn";
    case 0x23: return "End";
    case 0x24: return "Home";
    case 0x25: return "Left";
    case 0x26: return "Up";
    case 0x27: return "Right";
    case 0x28: return "Down";
    case 0x29: return "Select";
    case 0x2A: return "Print";
    case 0x2B: return "Execute";
    case 0x2C: return "PrtScr";
    case 0x2D: return "Ins";
    case 0x2E: return "Del";
    case 0x2F: return "Help";
    case 0x30: return "0";
    case 0x31: return "1";
    case 0x32: return "2";
    case 0x33: return "3";
    case 0x34: return "4";
    case 0x35: return "5";
    case 0x36: return "6";
    case 0x37: return "7";
    case 0x38: return "8";
    case 0x39: return "9";
    case 0x3A: return "0x3A";
    case 0x3B: return "0x3B";
    case 0x3C: return "0x3C";
    case 0x3D: return "0x3D";
    case 0x3E: return "0x3E";
    case 0x3F: return "0x3F";
    case 0x40: return "0x40";
    case 0x41: return "A";
    case 0x42: return "B";
    case 0x43: return "C";
    case 0x44: return "D";
    case 0x45: return "E";
    case 0x46: return "F";
    case 0x47: return "G";
    case 0x48: return "H";
    case 0x49: return "I";
    case 0x4A: return "J";
    case 0x4B: return "K";
    case 0x4C: return "L";
    case 0x4D: return "M";
    case 0x4E: return "N";
    case 0x4F: return "O";
    case 0x50: return "P";
    case 0x51: return "Q";
    case 0x52: return "R";
    case 0x53: return "S";
    case 0x54: return "T";
    case 0x55: return "U";
    case 0x56: return "V";
    case 0x57: return "W";
    case 0x58: return "X";
    case 0x59: return "Y";
    case 0x5A: return "Z";
        //case 0x5B: return "LWin";        //modifier
        //case 0x5C: return "RWin";        //modifier
    case 0x5D: return "Apps";
    case 0x5E: return "0x5E";
    case 0x5F: return "Sleep";
    case 0x60: return "Num0";
    case 0x61: return "Num1";
    case 0x62: return "Num2";
    case 0x63: return "Num3";
    case 0x64: return "Num4";
    case 0x65: return "Num5";
    case 0x66: return "Num6";
    case 0x67: return "Num7";
    case 0x68: return "Num8";
    case 0x69: return "Num9";
    case 0x6A: return "Num*";
    case 0x6B: return "Num+";
    case 0x6C: return "Separator";
    case 0x6D: return "Num-";
    case 0x6E: return "Num.";
    case 0x6F: return "Num/";
    case 0x70: return "F1";
    case 0x71: return "F2";
    case 0x72: return "F3";
    case 0x73: return "F4";
    case 0x74: return "F5";
    case 0x75: return "F6";
    case 0x76: return "F7";
    case 0x77: return "F8";
    case 0x78: return "F9";
    case 0x79: return "F10";
    case 0x7A: return "F11";
    case 0x7B: return "F12";
    case 0x7C: return "F13";
    case 0x7D: return "F14";
    case 0x7E: return "F15";
    case 0x7F: return "F16";
    case 0x80: return "F17";
    case 0x81: return "F18";
    case 0x82: return "F19";
    case 0x83: return "F20";
    case 0x84: return "F21";
    case 0x85: return "F22";
    case 0x86: return "F23";
    case 0x87: return "F24";
    case 0x88: return "0x88";
    case 0x89: return "0x89";
    case 0x8A: return "0x8A";
    case 0x8B: return "0x8B";
    case 0x8C: return "0x8C";
    case 0x8D: return "0x8D";
    case 0x8E: return "0x8E";
    case 0x8F: return "0x8F";
    case 0x90: return "NumLock";
    case 0x91: return "ScrollLock";
    case 0x92: return "0x92";
    case 0x93: return "0x93";
    case 0x94: return "0x94";
    case 0x95: return "0x95";
    case 0x96: return "0x96";
    case 0x97: return "0x97";
    case 0x98: return "0x98";
    case 0x99: return "0x99";
    case 0x9A: return "0x9A";
    case 0x9B: return "0x9B";
    case 0x9C: return "0x9C";
    case 0x9D: return "0x9D";
    case 0x9E: return "0x9E";
    case 0x9F: return "0x9F";
        //case 0xA0: return "LShift";        //modifier
        //case 0xA1: return "RShift";        //modifier
        //case 0xA2: return "LCtrl";        //modifier
        //case 0xA3: return "RCtrl";        //modifier
        //case 0xA4: return "LAlt";        //modifier
        //case 0xA5: return "RAlt";        //modifier
    case 0xA6: return "BrowserBack";
    case 0xA7: return "BrowserForward";
    case 0xA8: return "BrowserRefresh";
    case 0xA9: return "BrowserStop";
    case 0xAA: return "BrowserSearch";
    case 0xAB: return "BrowserFavorites";
    case 0xAC: return "BrowserHome";
    case 0xAD: return "VolumeMute";
    case 0xAE: return "VolumeDown";
    case 0xAF: return "VolumeUp";
    case 0xB0: return "MediaNext";
    case 0xB1: return "MediaPrev";
    case 0xB2: return "MediaStop";
    case 0xB3: return "MediaPlayPause";
    case 0xB4: return "StartMail";
    case 0xB5: return "MediaSelect";
    case 0xB6: return "StartApp1";
    case 0xB7: return "StartApp2";
    case 0xB8: return "0xB8";
    case 0xB9: return "0xB9";
    case 0xBA: return ";";
    case 0xBB: return "=";
    case 0xBC: return ",";
    case 0xBD: return "-";
    case 0xBE: return ".";
    case 0xBF: return "/";
    case 0xC0: return "`";
    case 0xC1: return "0xC1";
    case 0xC2: return "0xC2";
    case 0xC3: return "0xC3";
    case 0xC4: return "0xC4";
    case 0xC5: return "0xC5";
    case 0xC6: return "0xC6";
    case 0xC7: return "0xC7";
    case 0xC8: return "0xC8";
    case 0xC9: return "0xC9";
    case 0xCA: return "0xCA";
    case 0xCB: return "0xCB";
    case 0xCC: return "0xCC";
    case 0xCD: return "0xCD";
    case 0xCE: return "0xCE";
    case 0xCF: return "0xCF";
    case 0xD0: return "0xD0";
    case 0xD1: return "0xD1";
    case 0xD2: return "0xD2";
    case 0xD3: return "0xD3";
    case 0xD4: return "0xD4";
    case 0xD5: return "0xD5";
    case 0xD6: return "0xD6";
    case 0xD7: return "0xD7";
    case 0xD8: return "0xD8";
    case 0xD9: return "0xD9";
    case 0xDA: return "0xDA";
    case 0xDB: return "[";
    case 0xDC: return "\\";
    case 0xDD: return "]";
    case 0xDE: return "'";
    case 0xDF: return "0xDF";
    case 0xE0: return "0xE0";
    case 0xE1: return "0xE1";
    case 0xE2: return "0xE2";
    case 0xE3: return "0xE3";
    case 0xE4: return "0xE4";
    case 0xE5: return "Process";
    case 0xE6: return "0xE6";
        //case 0xE7: return "Packet";        //used to pass unicode characters
    case 0xE8: return "0xE8";
    case 0xE9: return "0xE9";
    case 0xEA: return "0xEA";
    case 0xEB: return "0xEB";
    case 0xEC: return "0xEC";
    case 0xED: return "0xED";
    case 0xEE: return "0xEE";
    case 0xEF: return "0xEF";
    case 0xF0: return "0xF0";
    case 0xF1: return "0xF1";
    case 0xF2: return "0xF2";
    case 0xF3: return "0xF3";
    case 0xF4: return "0xF4";
    case 0xF5: return "0xF5";
    case 0xF6: return "Attn";
    case 0xF7: return "CrSel";
    case 0xF8: return "ExSel";
    case 0xF9: return "ErEOF";
    case 0xFA: return "Play";
    case 0xFB: return "Zoom";
    case 0xFC: return "0xFC";
    case 0xFD: return "PA1";
    case 0xFE: return "OemClear";
    default: return 0;
    }
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::fOpenCfg(const QString &strPath)
{
    Q_ASSERT(QFileInfo(strPath).isFile());
    QFile file(strPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, 0, tr("Can't open file"));
        return;
    }
    const QStringList slistEntries = QString(file.readAll()).split('\n');
    if (file.error())
    {
        QMessageBox::warning(this, 0, file.errorString());
        return;
    }
    file.close();

    const int iSize = slistEntries.size();
    if (!iSize || iSize%8)        //[8 = 7 lines and empty line]
    {
        QMessageBox::warning(this, 0, tr("Incorrect file size"));
        return;
    }
    const int iRules = iSize/8;

    TagEntry *tgEntry = new TagEntry[iRules];
    for (int i = 0; i < slistEntries.size(); ++i)
    {
        TagEntry &reftgEntry = tgEntry[i/8];
        bool bOk;

        reftgEntry.iModifiers = slistEntries.at(i).toInt(&bOk, 16);
        if (!(bOk && reftgEntry.iModifiers >= 0 && reftgEntry.iModifiers <= 16))
        {
            delete[] tgEntry;
            QMessageBox::warning(this, 0, tr("Incorrect line:") + " #" + QString::number(i+1));
            return;
        }

        reftgEntry.iVirtualKey = slistEntries.at(++i).toInt(0, 16);

        reftgEntry.strKey = fKeyToString(reftgEntry.iVirtualKey);
        if (reftgEntry.strKey.isEmpty())
        {
            delete[] tgEntry;
            QMessageBox::warning(this, 0, tr("Incorrect line:") + " #" + QString::number(i+1));
            return;
        }

        //check repeats
        for (int j = 0; j < i/8; ++j)
            if (tgEntry[j].iVirtualKey == reftgEntry.iVirtualKey && tgEntry[j].iModifiers == reftgEntry.iModifiers)
            {
                delete[] tgEntry;
                QMessageBox::warning(this, 0, tr("Key combination met twice in line:") + " #" + QString::number(i));
                return;
            }

        if (slistEntries.at(++i) == "M")        //send message
        {
            reftgEntry.bTypeIsMsg = true;

            reftgEntry.iPlugin = slistPlugins.indexOf(slistEntries.at(++i));
            if (reftgEntry.iPlugin < 0)
            {
                delete[] tgEntry;
                QMessageBox::warning(this, 0, tr("Plugin not found:") + '\n' + slistEntries.at(i));
                return;
            }

            reftgEntry.pStrMessage = &slistEntries.at(++i);
            if (reftgEntry.pStrMessage->isEmpty())
            {
                delete[] tgEntry;
                QMessageBox::warning(this, 0, tr("Empty line:") + " #" + QString::number(i+1));
                return;
            }

            if (!(slistEntries.at(++i) == "-" && slistEntries.at(++i) == "-" && slistEntries.at(++i).isEmpty()))
            {
                delete[] tgEntry;
                QMessageBox::warning(this, 0, tr("Incorrect line:") + " #" + QString::number(i+1));
                return;
            }
        }
        else if (slistEntries.at(i) == "F")        //run file
        {
            reftgEntry.bTypeIsMsg = false;

            reftgEntry.pStrFile = &slistEntries.at(++i);
            if (reftgEntry.pStrFile->isEmpty())
            {
                delete[] tgEntry;
                QMessageBox::warning(this, 0, tr("Empty line:") + " #" + QString::number(i+1));
                return;
            }

            reftgEntry.pStrParam = &slistEntries.at(++i);

            reftgEntry.pStrWorkDir = &slistEntries.at(++i);

            reftgEntry.iShowCmd = slistEntries.at(++i).toInt(&bOk);
            if (!(bOk && reftgEntry.iShowCmd >= 0 && reftgEntry.iShowCmd <= 10 && slistEntries.at(++i).isEmpty()))
            {
                delete[] tgEntry;
                QMessageBox::warning(this, 0, tr("Incorrect line:") + " #" + QString::number(i+1));
                return;
            }
        }
        else
        {
            delete[] tgEntry;
            QMessageBox::warning(this, 0, tr("Incorrect line:") + " #" + QString::number(i+1));
            return;
        }
        reftgEntry.strKey.prepend(strModifiers[reftgEntry.iModifiers]);
    }

    SHFILEINFO shFileInfo;
    for (int i = 0; i < iRules; ++i)
    {
        QStandardItem *sitem = new QStandardItem(tgEntry[i].strKey);
        sitem->setFlags(sitem->flags() & ~Qt::ItemIsDropEnabled);
        sitem->setData(tgEntry[i].iModifiers, ROLE_MODIFIERS);
        sitem->setData(tgEntry[i].iVirtualKey, ROLE_VIRTUALKEY);
        sitem->setData(tgEntry[i].bTypeIsMsg, ROLE_TYPE_ISMSG);
        if (tgEntry[i].bTypeIsMsg)
        {
            sitem->setData(tgEntry[i].iPlugin, ROLE_PLUGIN);
            sitem->setData(*tgEntry[i].pStrMessage, ROLE_MESSAGE);
            sitem->setIcon(QIcon(":/img/plugin.png"));
        }
        else
        {
            const QString strTemp = *tgEntry[i].pStrFile;
            sitem->setData(strTemp, ROLE_FILE);
            sitem->setData(*tgEntry[i].pStrParam, ROLE_PARAM);
            sitem->setData(*tgEntry[i].pStrWorkDir, ROLE_WORKDIR);
            sitem->setData(tgEntry[i].iShowCmd, ROLE_SHOWMODE);
            if (::SHGetFileInfo(static_cast<const wchar_t*>(static_cast<const void*>(strTemp.utf16())),
                                0,
                                &shFileInfo,
                                sizeof(SHFILEINFO),
                                SHGFI_ICON | SHGFI_SMALLICON) &&
                    shFileInfo.hIcon)
            {
                const QPixmap pixmap = fFromWinHICON(shFileInfo.hIcon);
                DestroyIcon(shFileInfo.hIcon);
                sitem->setIcon(pixmap.isNull() ? QIcon(":/img/empty.png") : pixmap);
            }
            else
                sitem->setIcon(QIcon(":/img/empty.png"));
        }
        sitem->setEditable(false);
        sitemModel->appendRow(sitem);
    }

    delete[] tgEntry;
    strCurrentCfg = QDir::toNativeSeparators(strPath);
    this->setWindowTitle(qAppName() + " [" + strCurrentCfg + ']');
    pbSaveCfg->setEnabled(true);
    pbSaveAsCfg->setEnabled(true);
    Q_ASSERT(iRules == sitemModel->rowCount());
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::fSaveCfg(const QString &strPath)
{
    Q_ASSERT(sitemModel->hasChildren());
    QString strEntries;
    for (int i = 0, iRowCount = sitemModel->rowCount(); i < iRowCount; ++i)
    {
        const QModelIndex modInd = sitemModel->index(i, 0);
        strEntries += "0x" + QString::number(modInd.data(ROLE_MODIFIERS).toInt(), 16) + "\n0x" +
                QString::number(modInd.data(ROLE_VIRTUALKEY).toInt(), 16) +
                (
                    modInd.data(ROLE_TYPE_ISMSG).toBool() ?
                        ("\nM\n" +
                         slistPlugins.at(modInd.data(ROLE_PLUGIN).toInt()) + '\n' +
                         modInd.data(ROLE_MESSAGE).toString() + "\n-\n-\n\n") :
                        ("\nF\n" +
                         modInd.data(ROLE_FILE).toString() + '\n' +
                         modInd.data(ROLE_PARAM).toString() + '\n' +
                         modInd.data(ROLE_WORKDIR).toString() + '\n' +
                         QString::number(modInd.data(ROLE_SHOWMODE).toInt()) + "\n\n")
                        );
    }
    strEntries.chop(1);        //unwanted last line

    QFile file(strPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, 0, tr("Can't create file"));
        return;
    }

    file.write(strEntries.toUtf8());
    if (file.error())
        QMessageBox::warning(this, 0, file.errorString());
    else
    {
        strCurrentCfg = QDir::toNativeSeparators(strPath);
        this->setWindowTitle(qAppName() + " [" + strCurrentCfg + ']');
        lblInfo->setText(tr("Saved successfully"));
        QTimer::singleShot(1000, lblInfo, SLOT(clear()));
    }
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotNewCfg()
{
    sitemModel->clear();
    strCurrentCfg.clear();
    this->setWindowTitle(qAppName() + " [?]");
    iModifiersTemp = iModifiers = iVirtualKey = 0;
    leKeyComb->clear();
    tbKeyComb->setEnabled(false);
    leFile->clear();
    leParam->clear();
    leWorkDir->clear();
    cbShowMode->setCurrentIndex(SW_SHOWNORMAL);
    rbTypeFile->setChecked(true);
    if (leMsg)
        leMsg->clear();
    pbSaveCfg->setEnabled(false);
    pbSaveAsCfg->setEnabled(false);
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotOpenCfg()
{
    const QString strPath = QFileDialog::getOpenFileName(this, 0, 0, "Config (*.cfg);;All files (*.*)");
    if (!strPath.isEmpty())
    {
        slotNewCfg();
        fOpenCfg(strPath);
    }
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotSaveCfg()
{
    Q_ASSERT(sitemModel->hasChildren());
    if (strCurrentCfg.isEmpty())
        slotSaveAsCfg();
    else
        fSaveCfg(strCurrentCfg);
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotSaveAsCfg()
{
    Q_ASSERT(sitemModel->hasChildren());
    const QString strPath = QFileDialog::getSaveFileName(this, 0, 0, "Config (*.cfg);;All files (*.*)");
    if (!strPath.isEmpty())
        fSaveCfg(strPath);
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotRestart()
{
    slotStop();
    if (strCurrentCfg.isEmpty() ?
            QProcess::startDetached(MAIN_APP_NAME ".exe") :
            QProcess::startDetached(MAIN_APP_NAME ".exe", QStringList(strCurrentCfg)))
    {
        lblInfo->setText(tr("Running successfully"));
        QTimer::singleShot(1000, lblInfo, SLOT(clear()));
    }
    else
        QMessageBox::warning(this, 0, tr("Error: can't run application"));
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotStop() const
{
    for (HWND hWndPrev = 0;
         (hWndPrev = FindWindowEx(HWND_MESSAGE, hWndPrev, GUID_CLASS, 0));
         PostMessage(hWndPrev, WM_CLOSE, 0, 0));
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotChangeKeyComb()
{
    Q_ASSERT(!leNewKeyComb);
    Q_ASSERT(listView->currentIndex().isValid());
    Q_ASSERT(leKeyComb->text() == strModifiers[iModifiers] + fKeyToString(iVirtualKey));

    QDialog dialNewKeyComb(this, Qt::WindowCloseButtonHint);
    QLabel *lblNewKeyComb = new QLabel(tr("Enter new key combination:"), &dialNewKeyComb);
    leNewKeyComb = new QLineEdit(&dialNewKeyComb);
    leNewKeyComb->setContextMenuPolicy(Qt::NoContextMenu);
    leNewKeyComb->setFixedWidth(leKeyComb->width());
    QDialogButtonBox *dialogBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialNewKeyComb);
    QVBoxLayout *vblNewMain = new QVBoxLayout(&dialNewKeyComb);
    vblNewMain->addWidget(lblNewKeyComb);
    vblNewMain->addWidget(leNewKeyComb);
    vblNewMain->addWidget(dialogBox);
    dialNewKeyComb.setFixedSize(dialNewKeyComb.minimumSizeHint());

    connect(dialogBox, SIGNAL(accepted()), &dialNewKeyComb, SLOT(accept()));
    connect(dialogBox, SIGNAL(rejected()), &dialNewKeyComb, SLOT(reject()));

    iNewModifiersTemp = iNewModifiers = iNewVirtualKey = 0;

    if (dialNewKeyComb.exec() == QDialog::Accepted && iNewVirtualKey && !(iNewVirtualKey == iVirtualKey && iNewModifiers == iModifiers))
    {
        bool bReplace = true;
        for (int i = 0, iRowCount = sitemModel->rowCount(); i < iRowCount; ++i)
        {
            const QModelIndex modInd = sitemModel->index(i, 0);
            if (modInd.data(ROLE_VIRTUALKEY).toInt() == iNewVirtualKey && modInd.data(ROLE_MODIFIERS).toInt() == iNewModifiers)
            {
                Q_ASSERT(modInd.data() == leNewKeyComb->text());
                if (QMessageBox::question(this, 0, tr("This key combination already exists, remove the previous entry?")) == QMessageBox::Yes)
                    sitemModel->removeRow(i);
                else
                    bReplace = false;
                break;
            }
        }

        if (bReplace)
        {
            const QString strText = leNewKeyComb->text();
            Q_ASSERT(strText == strModifiers[iNewModifiers] + fKeyToString(iNewVirtualKey));
            QStandardItem *sitem = sitemModel->itemFromIndex(listView->currentIndex());
            sitem->setText(strText);
            sitem->setData(iNewModifiers, ROLE_MODIFIERS);
            sitem->setData(iNewVirtualKey, ROLE_VIRTUALKEY);
            iModifiers = iNewModifiers;
            iVirtualKey = iNewVirtualKey;
            leKeyComb->setText(strText);
        }
    }

    iModifiersTemp = 0;
    leNewKeyComb = 0;        //necessarily
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotChangeFile()
{
    const QString strPath = QFileDialog::getOpenFileName(this);
    if (!strPath.isEmpty())
        leFile->setText(QDir::toNativeSeparators(strPath));
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotChangeWorkDir()
{
    const QString strPath = QFileDialog::getExistingDirectory(this);
    if (!strPath.isEmpty())
        leWorkDir->setText(QDir::toNativeSeparators(strPath));
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotOk() const
{
    if (!iVirtualKey)
    {
        Q_ASSERT(leKeyComb->text().isEmpty());
        leKeyComb->setFocus();
        return;
    }
    Q_ASSERT(leKeyComb->text() == strModifiers[iModifiers] + fKeyToString(iVirtualKey));

    QString strTemp;
    const bool bTypeIsMsg = rbTypeMsg->isChecked();
    if (bTypeIsMsg)
    {
        strTemp = leMsg->text();
        if (strTemp.isEmpty())
        {
            leMsg->setFocus();
            return;
        }
    }
    else
    {
        strTemp = leFile->text();
        if (strTemp.isEmpty())
        {
            leFile->setFocus();
            return;
        }
    }

    for (int i = 0, iRowCount = sitemModel->rowCount(); i < iRowCount; ++i)
    {
        const QModelIndex modInd = sitemModel->index(i, 0);
        if (modInd.data(ROLE_VIRTUALKEY).toInt() == iVirtualKey && modInd.data(ROLE_MODIFIERS).toInt() == iModifiers)
        {
            QStandardItem *sitem = sitemModel->itemFromIndex(modInd);
            sitem->setFlags(sitem->flags() & ~Qt::ItemIsDropEnabled);
            sitem->setData(iModifiers, ROLE_MODIFIERS);
            sitem->setData(iVirtualKey, ROLE_VIRTUALKEY);
            if (bTypeIsMsg)
            {
                sitem->setData(true, ROLE_TYPE_ISMSG);
                sitem->setData(cbPlugin->currentIndex(), ROLE_PLUGIN);
                sitem->setData(strTemp, ROLE_MESSAGE);
                sitem->setData(QVariant(), ROLE_WORKDIR);
                sitem->setData(QVariant(), ROLE_SHOWMODE);
                sitem->setIcon(QIcon(":/img/plugin.png"));
            }
            else
            {
                sitem->setData(false, ROLE_TYPE_ISMSG);
                sitem->setData(strTemp, ROLE_FILE);
                sitem->setData(leParam->text(), ROLE_PARAM);
                sitem->setData(leWorkDir->text(), ROLE_WORKDIR);
                sitem->setData(cbShowMode->currentIndex(), ROLE_SHOWMODE);
                SHFILEINFO shFileInfo;
                if (::SHGetFileInfo(static_cast<const wchar_t*>(static_cast<const void*>(strTemp.utf16())),
                                    0,
                                    &shFileInfo,
                                    sizeof(SHFILEINFO),
                                    SHGFI_ICON | SHGFI_SMALLICON) &&
                        shFileInfo.hIcon)
                {
                    const QPixmap pixmap = fFromWinHICON(shFileInfo.hIcon);
                    DestroyIcon(shFileInfo.hIcon);
                    sitem->setIcon(pixmap.isNull() ? QIcon(":/img/empty.png") : pixmap);
                }
                else
                    sitem->setIcon(QIcon(":/img/empty.png"));
            }
            listView->setCurrentIndex(modInd);
            return;
        }
    }

    QStandardItem *sitem = new QStandardItem(leKeyComb->text());
    sitem->setFlags(sitem->flags() & ~Qt::ItemIsDropEnabled);
    sitem->setData(iModifiers, ROLE_MODIFIERS);
    sitem->setData(iVirtualKey, ROLE_VIRTUALKEY);
    if (bTypeIsMsg)
    {
        sitem->setData(true, ROLE_TYPE_ISMSG);
        sitem->setData(cbPlugin->currentIndex(), ROLE_PLUGIN);
        sitem->setData(strTemp, ROLE_MESSAGE);
        sitem->setIcon(QIcon(":/img/plugin.png"));
    }
    else
    {
        sitem->setData(false, ROLE_TYPE_ISMSG);
        sitem->setData(strTemp, ROLE_FILE);
        sitem->setData(leParam->text(), ROLE_PARAM);
        sitem->setData(leWorkDir->text(), ROLE_WORKDIR);
        sitem->setData(cbShowMode->currentIndex(), ROLE_SHOWMODE);
        SHFILEINFO shFileInfo;
        if (::SHGetFileInfo(static_cast<const wchar_t*>(static_cast<const void*>(strTemp.utf16())),
                            0,
                            &shFileInfo,
                            sizeof(SHFILEINFO),
                            SHGFI_ICON | SHGFI_SMALLICON) &&
                shFileInfo.hIcon)
        {
            const QPixmap pixmap = fFromWinHICON(shFileInfo.hIcon);
            DestroyIcon(shFileInfo.hIcon);
            sitem->setIcon(pixmap.isNull() ? QIcon(":/img/empty.png") : pixmap);
        }
        else
            sitem->setIcon(QIcon(":/img/empty.png"));
    }
    sitem->setEditable(false);
    sitemModel->appendRow(sitem);
    listView->setCurrentIndex(sitemModel->indexFromItem(sitem));

    pbSaveCfg->setEnabled(true);
    pbSaveAsCfg->setEnabled(true);
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotDel() const
{
    sitemModel->removeRow(listView->currentIndex().row());        //it's safe, cleaning in currentChanged
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotTimerActive()
{
    const bool bHwnd = ::FindWindowEx(HWND_MESSAGE, 0, GUID_CLASS, 0);
    if (bHwnd != bActive)
    {
        bActive = bHwnd;
        this->setWindowIcon(QIcon(bActive ? ":/img/on.png" : ":/img/off.png"));
    }
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotCurrentChanged(const QModelIndex &modIndCurrent)
{
    iModifiersTemp = 0;
    if (modIndCurrent.isValid())
    {
        iModifiers = modIndCurrent.data(ROLE_MODIFIERS).toInt();
        iVirtualKey = modIndCurrent.data(ROLE_VIRTUALKEY).toInt();
        leKeyComb->setText(modIndCurrent.data().toString());
        if (modIndCurrent.data(ROLE_TYPE_ISMSG).toBool())
        {
            cbPlugin->setCurrentIndex(modIndCurrent.data(ROLE_PLUGIN).toInt());
            leMsg->setText(modIndCurrent.data(ROLE_MESSAGE).toString());
            rbTypeMsg->setChecked(true);
            leFile->clear();
            leParam->clear();
            leWorkDir->clear();
            cbShowMode->setCurrentIndex(SW_SHOWNORMAL);
        }
        else
        {
            leFile->setText(modIndCurrent.data(ROLE_FILE).toString());
            leParam->setText(modIndCurrent.data(ROLE_PARAM).toString());
            leWorkDir->setText(modIndCurrent.data(ROLE_WORKDIR).toString());
            cbShowMode->setCurrentIndex(modIndCurrent.data(ROLE_SHOWMODE).toInt());
            rbTypeFile->setChecked(true);
            if (leMsg)
                leMsg->clear();
        }
        tbKeyComb->setEnabled(true);
        Q_ASSERT(leKeyComb->text() == strModifiers[iModifiers] + fKeyToString(iVirtualKey));
    }
    else
    {
        iModifiers = iVirtualKey = 0;
        leKeyComb->clear();
        leFile->clear();
        leParam->clear();
        leWorkDir->clear();
        cbShowMode->setCurrentIndex(SW_SHOWNORMAL);
        rbTypeFile->setChecked(true);
        if (leMsg)
            leMsg->clear();
        tbKeyComb->setEnabled(false);
        pbSaveCfg->setEnabled(false);
        pbSaveAsCfg->setEnabled(false);
    }
}
