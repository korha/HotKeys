#include "hotkeyssettings.h"

#ifdef _WIN64
#define MAIN_APP "HotKeys64"
#else
#define MAIN_APP "HotKeys32"
#endif
static const wchar_t *const g_wGuidClass = L"App::b6c9d088-c4a5-45f2-8481-87b29bcaec50",
#ifdef QT_DEBUG
*const g_wLibGui = L"Qt5Guid.dll";
#else
*const g_wLibGui = L"Qt5Gui.dll";
#endif
static const char *const g_cFuncFromWinHicon = "_Z21qt_pixmapFromWinHICONP7HICON__";

//-------------------------------------------------------------------------------------------------
ListViewEx::ListViewEx(QWidget *parent) : QListView(parent)
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
    bActive(::FindWindowEx(HWND_MESSAGE, 0, g_wGuidClass, 0)),
    hHook(::SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0))
{
    Q_ASSERT_X(!QString('a').utf16()[1], "Error", "UTF-16 must be LE");
    Q_ASSERT((MOD_WIN | MOD_CONTROL | MOD_ALT | MOD_SHIFT) == 0xF);
    Q_ASSERT(SW_HIDE == 0 && SW_SHOWNORMAL == 1 && SW_SHOWMINIMIZED == 2 && SW_MAXIMIZE == 3 &&
             SW_SHOWNOACTIVATE == 4 && SW_MINIMIZE == 6 && SW_SHOWMINNOACTIVE == 7);
    Q_ASSERT((WM_KEYDOWN & 1) == 0 && (WM_SYSKEYDOWN & 1) == 0);
    Q_ASSERT((WM_KEYUP & 1) == 1 && (WM_SYSKEYUP & 1) == 1);

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

    const QString strAppName = qAppName();
    QString strAppDir = qApp->applicationDirPath();
    QTranslator *translator = new QTranslator(this);
    if (translator->load(strAppName, strAppDir) || translator->load(strAppName + '_' + QLocale::system().name(), strAppDir))
        qApp->installTranslator(translator);

    if (const HMODULE hMod = ::GetModuleHandle(g_wLibGui))
        fFromWinHICON = reinterpret_cast<PFromWinHICON>(::GetProcAddress(hMod, g_cFuncFromWinHicon));
    if (!fFromWinHICON)
    {
        Q_ASSERT(false);
        fFromWinHICON = &fNullPixmap;
    }

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
    QPushButton *pbStop = new QPushButton(style()->standardIcon(QStyle::SP_LineEditClearButton), 0, this);
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

    QLabel *lblFile = new QLabel(tr("Command Line:"), this);
    leCmdLine = new QLineEdit(this);
    QToolButton *tbCmdLine = new QToolButton(this);
    tbCmdLine->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    QHBoxLayout *hblFile = new QHBoxLayout;
    hblFile->setSpacing(3);
    hblFile->addWidget(leCmdLine);
    hblFile->addWidget(tbCmdLine);

    QLabel *lblWorkDir = new QLabel(tr("Working Directory:"), this);
    leWorkDir = new QLineEdit(this);
    QToolButton *tbWorkDir = new QToolButton(this);
    tbWorkDir->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    QHBoxLayout *hblWorkDir = new QHBoxLayout;
    hblWorkDir->setSpacing(3);
    hblWorkDir->addWidget(leWorkDir);
    hblWorkDir->addWidget(tbWorkDir);

    QLabel *lblShowMode = new QLabel(tr("Show Mode:"), this);
    cbShowCmd = new QComboBox(this);
    cbShowCmd->setMaxVisibleItems(11);
    cbShowCmd->addItems(QStringList(tr("Normal")) << tr("Minimized") << tr("Maximized") << tr("Hidden")
                        << tr("Normal (Not Activate)") << tr("Minimized (Activate)") << tr("Minimized (Activate Next)"));

    QWidget *wgtTypeFile = new QWidget(this);
    QGridLayout *glTypeFile = new QGridLayout(wgtTypeFile);
    glTypeFile->setContentsMargins(0, 0, 0, 0);
    glTypeFile->addWidget(lblFile, 0, 0);
    glTypeFile->addLayout(hblFile, 0, 1);
    glTypeFile->addWidget(lblWorkDir, 1, 0);
    glTypeFile->addLayout(hblWorkDir, 1, 1);
    glTypeFile->addWidget(lblShowMode, 2, 0);
    glTypeFile->addWidget(cbShowCmd, 2, 1, Qt::AlignLeft);

    slistPlugins = QDir(strAppDir + ("/" MAIN_APP "_plugins")).entryList(QStringList("*.dll"), QDir::Files);

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
        Q_ASSERT(cbPlugin->sizePolicy().verticalPolicy() == QSizePolicy::Fixed);
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

    leKeyComb->setFixedWidth(leKeyComb->fontMetrics().width("Win+Ctrl+Alt+Shift+BrowserFavorites") + 10);        //max width
    this->setWindowTitle(strAppName + " [?]");
    this->setWindowIcon(QIcon(bActive ? ":/img/on.png" : ":/img/off.png"));
    pApplication = this;

    //connects
    connect(pbNewCfg, SIGNAL(clicked()), this, SLOT(slotNewCfg()));
    connect(pbOpenCfg, SIGNAL(clicked()), this, SLOT(slotOpenCfg()));
    connect(pbSaveCfg, SIGNAL(clicked()), this, SLOT(slotSaveCfg()));
    connect(pbSaveAsCfg, SIGNAL(clicked()), this, SLOT(slotSaveAsCfg()));
    if (QFileInfo(strAppDir + ("/" MAIN_APP ".exe")).exists())
        connect(pbRestart, SIGNAL(clicked()), this, SLOT(slotRestart()));
    else
        pbRestart->setEnabled(false);
    connect(pbStop, SIGNAL(clicked()), this, SLOT(slotStop()));
    connect(leCmdLine, SIGNAL(textChanged(QString)), this, SLOT(slotHintWorkDir(QString)));
    connect(tbCmdLine, SIGNAL(clicked()), this, SLOT(slotChangeFile()));
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
    else
    {
        strAppDir += ("/" MAIN_APP ".cfg");
        if (QFileInfo(strAppDir).isFile())
            fOpenCfg(strAppDir);
    }
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
    return ((nCode == HC_ACTION &&
             pApplication->fKeyboardProc(!(wParam & 1), reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam)->vkCode)
             ) ? 1 : ::CallNextHookEx(0, nCode, wParam, lParam));
}

//-------------------------------------------------------------------------------------------------
QPixmap HotKeysSettings::fNullPixmap(HICON)
{
    return QPixmap();
}

//-------------------------------------------------------------------------------------------------
const char *const HotKeysSettings::cKeyToString[0xFF] = {
    0, "LeftButton", "RightButton", "Control-Break", "MiddleButton", "XButton1", "XButton2", "0x07", "BackSpace", "Tab",
    "0x0A", "0x0B", "Clear", "Enter", "0x0E", "0x0F", 0/*Shift (modifier)*/, 0/*Ctrl (modifier)*/, 0/*Alt (modifier)*/,
    "Pause", "CapsLock", "Kana", "0x16", "Junja", "Final", "Hanja", "0x1A", "Esc", "Convert", "NonConvert", "Accept",
    "ModeChange", "Space", "PgUp", "PgDn", "End", "Home", "Left", "Up", "Right", "Down", "Select", "Print", "Execute",
    "PrtScr", "Ins", "Del", "Help", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0x3A", "0x3B", "0x3C", "0x3D",
    "0x3E", "0x3F", "0x40", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R",
    "S", "T", "U", "V", "W", "X", "Y", "Z", 0/*LWin (modifier)*/, 0/*RWin (modifier)*/, "Apps", "0x5E", "Sleep", "Num0",
    "Num1", "Num2", "Num3", "Num4", "Num5", "Num6", "Num7", "Num8", "Num9", "Num*", "Num+", "Separator", "Num-", "Num.",
    "Num/", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16",
    "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24", "0x88", "0x89", "0x8A", "0x8B", "0x8C", "0x8D", "0x8E",
    "0x8F", "NumLock", "ScrollLock", "0x92", "0x93", "0x94", "0x95", "0x96", "0x97", "0x98", "0x99", "0x9A", "0x9B",
    "0x9C", "0x9D", "0x9E", "0x9F", 0/*LShift (modifier)*/, 0/*RShift (modifier)*/, 0/*LCtrl (modifier)*/,
    0/*RCtrl (modifier)*/, 0/*LAlt (modifier)*/, 0/*RAlt (modifier)*/, "BrowserBack", "BrowserForward",
    "BrowserRefresh", "BrowserStop", "BrowserSearch", "BrowserFavorites", "BrowserHome", "VolumeMute", "VolumeDown",
    "VolumeUp", "MediaNext", "MediaPrev", "MediaStop", "MediaPlayPause", "StartMail", "MediaSelect", "StartApp1",
    "StartApp2", "0xB8", "0xB9", ";", "=", ",", "-", ".", "/", "`", "0xC1", "0xC2", "0xC3", "0xC4", "0xC5", "0xC6",
    "0xC7", "0xC8", "0xC9", "0xCA", "0xCB", "0xCC", "0xCD", "0xCE", "0xCF", "0xD0", "0xD1", "0xD2", "0xD3", "0xD4",
    "0xD5", "0xD6", "0xD7", "0xD8", "0xD9", "0xDA", "[", "\\", "]", "'", "0xDF", "0xE0", "0xE1", "0xE2", "0xE3", "0xE4",
    "Process", "0xE6", 0/*Packet (unicode characters)*/, "0xE8", "0xE9", "0xEA", "0xEB", "0xEC", "0xED", "0xEE", "0xEF",
    "0xF0", "0xF1", "0xF2", "0xF3", "0xF4", "0xF5", "Attn", "CrSel", "ExSel", "ErEOF", "Play", "Zoom", "0xFC", "PA1",
    "OemClear"};

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
                if (const char *const cStrKey = fKeyToString(dwKey))
                {
                    const QString strKey = cStrKey;
                    iVirtualKey = dwKey;
                    iModifiers = iModifiersTemp;
                    leKeyComb->setText(strModifiers[iModifiers] + strKey);
                    for (int i = 0, iRowCount = sitemModel->rowCount(); i < iRowCount; ++i)
                    {
                        const QModelIndex modInd = sitemModel->index(i, 0);
                        if (modInd.data(eRoleVirtualKey).toInt() == iVirtualKey && modInd.data(eRoleModifiers).toInt() == iModifiers)
                        {
                            listView->setCurrentIndex(modInd);
                            tbKeyComb->setEnabled(true);
                            return true;
                        }
                    }
                    tbKeyComb->setEnabled(false);
                    leCmdLine->clear();
                    leWorkDir->clear();
                    cbShowCmd->setCurrentIndex(0);
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
QString HotKeysSettings::fGetFile(const QString &strCmdLine) const
{
    Q_ASSERT(!strCmdLine.isEmpty());
    if (strCmdLine.at(0) == '"')
    {
        int iPos = strCmdLine.indexOf('"', 1);
        if (iPos > 0)
            return strCmdLine.mid(1, iPos-1);
    }
    else
        for (int i = 0; i < strCmdLine.length(); ++i)
            if (strCmdLine.at(i) == ' ' || strCmdLine.at(i) == '\t')
                return strCmdLine.left(i);
    return strCmdLine;
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::fOpenCfg(const QString &strPath)
{
    Q_ASSERT(QFileInfo(strPath).isFile());
    QFile file(strPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, 0, tr("Can't open file"));
        return;
    }

    const QByteArray baFile = file.readAll();
    if (file.error())
    {
        QMessageBox::warning(this, 0, file.errorString());
        return;
    }
    file.close();
    if (!baFile.size())
    {
        strCurrentCfg = QDir::toNativeSeparators(strPath);
        this->setWindowTitle(qAppName() + " [" + strCurrentCfg + ']');
        return;
    }

    const QStringList slistEntries = QString::fromWCharArray(static_cast<const wchar_t*>(static_cast<const void*>(baFile.constData())), baFile.size()/sizeof(wchar_t)).split('\n');
    const int iRules = slistEntries.size()/2;
    if (slistEntries.isEmpty() || iRules*2 != slistEntries.size())
    {
        QMessageBox::warning(this, 0, tr("Incorrect file size"));
        return;
    }

    QVector<TagEntry> vectEntry(iRules);
    for (int i = 0; i < slistEntries.size(); ++i)
    {
        TagEntry &reftgEntry = vectEntry[i/2];
        const QString &strLine = slistEntries.at(i);
        const wchar_t *const pStrLine = static_cast<const wchar_t*>(static_cast<const void*>(strLine.utf16()));

        reftgEntry.iModifiers = *pStrLine - L'a';
        if (!(reftgEntry.iModifiers >= 0 && reftgEntry.iModifiers <= 16))
        {
            QMessageBox::warning(this, 0, tr("Incorrect line:") + " #" + QString::number(i+1));
            return;
        }

        reftgEntry.iVirtualKey = pStrLine[1] ? ((pStrLine[1] - L'a') << 4 | (pStrLine[2] - L'a')) : 0;
        reftgEntry.strKey = fKeyToString(reftgEntry.iVirtualKey);
        if (reftgEntry.strKey.isEmpty())
        {
            QMessageBox::warning(this, 0, tr("Incorrect line:") + " #" + QString::number(i+1));
            return;
        }
        Q_ASSERT(pStrLine[2]);

        //check repeats
        for (int j = 0; j < i/2; ++j)
            if (vectEntry.at(j).iVirtualKey == reftgEntry.iVirtualKey && vectEntry.at(j).iModifiers == reftgEntry.iModifiers)
            {
                QMessageBox::warning(this, 0, tr("Key combination met twice in line:") + " #" + QString::number(i+1));
                return;
            }

        if ((reftgEntry.bTypeIsMsg = (pStrLine[3] == L'5')))        //send message
        {
            reftgEntry.iPlugin = slistPlugins.indexOf(slistEntries.at(i).mid(4));
            if (reftgEntry.iPlugin < 0)
            {
                QMessageBox::warning(this, 0, tr("Plugin not found:") + '\n' + slistEntries.at(i).mid(4));
                return;
            }

            reftgEntry.pStrMessage = &slistEntries.at(++i);
            if (reftgEntry.pStrMessage->isEmpty())
            {
                QMessageBox::warning(this, 0, tr("Empty line:") + " #" + QString::number(i+1));
                return;
            }
        }
        else        //run process
        {
            switch (pStrLine[3])
            {
            case L'1': reftgEntry.iShowCmd = 0; break;
            case L'7': reftgEntry.iShowCmd = 1; break;
            case L'3': reftgEntry.iShowCmd = 2; break;
            case L'0': reftgEntry.iShowCmd = 3; break;
            case L'4': reftgEntry.iShowCmd = 4; break;
            case L'2': reftgEntry.iShowCmd = 5; break;
            case L'6': reftgEntry.iShowCmd = 6; break;
            default  :
                QMessageBox::warning(this, 0, tr("Incorrect line:") + " #" + QString::number(i+1));
                return;
            }
            if (!pStrLine[4])        //CmdLine non empty
            {
                QMessageBox::warning(this, 0, tr("Incorrect line:") + " #" + QString::number(i+1));
                return;
            }
            reftgEntry.pCmdLine = &slistEntries.at(i);
            reftgEntry.pStrWorkDir = &slistEntries.at(++i);
        }
        reftgEntry.strKey.prepend(strModifiers[reftgEntry.iModifiers]);
    }

    SHFILEINFO shFileInfo;
    for (int i = 0; i < iRules; ++i)
    {
        QStandardItem *sitem = new QStandardItem(vectEntry[i].strKey);
        sitem->setFlags(sitem->flags() & ~Qt::ItemIsDropEnabled);
        sitem->setData(vectEntry.at(i).iModifiers, eRoleModifiers);
        sitem->setData(vectEntry.at(i).iVirtualKey, eRoleVirtualKey);
        sitem->setData(vectEntry.at(i).bTypeIsMsg, eRoleIsMsg);
        if (vectEntry.at(i).bTypeIsMsg)
        {
            sitem->setData(vectEntry.at(i).iPlugin, eRolePlugin);
            sitem->setData(*vectEntry.at(i).pStrMessage, eRoleMessage);
            sitem->setIcon(QIcon(":/img/plugin.png"));
        }
        else
        {
            const QString strCmdLine = vectEntry.at(i).pCmdLine->mid(4);
            sitem->setData(strCmdLine, eRoleCmdLine);
            sitem->setData(*vectEntry.at(i).pStrWorkDir == fHintWorkDir(strCmdLine) ? 0 : *vectEntry.at(i).pStrWorkDir, eRoleWorkDir);
            sitem->setData(vectEntry.at(i).iShowCmd, eRoleShowCmd);
            if (::SHGetFileInfo(static_cast<const wchar_t*>(static_cast<const void*>(fGetFile(strCmdLine).utf16())),
                                0, &shFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON) && shFileInfo.hIcon)
            {
                const QPixmap pixmap = fFromWinHICON(shFileInfo.hIcon);
                ::DestroyIcon(shFileInfo.hIcon);
                sitem->setIcon(pixmap.isNull() ? QIcon(":/img/empty.png") : pixmap);
            }
            else
                sitem->setIcon(QIcon(":/img/empty.png"));
        }
        sitem->setEditable(false);
        sitemModel->appendRow(sitem);
    }

    strCurrentCfg = QDir::toNativeSeparators(strPath);
    this->setWindowTitle(qAppName() + " [" + strCurrentCfg + ']');
    pbSaveCfg->setEnabled(true);
    pbSaveAsCfg->setEnabled(true);
    Q_ASSERT(iRules == sitemModel->rowCount());
}

//-------------------------------------------------------------------------------------------------
QString HotKeysSettings::fHintWorkDir(const QString &strCmdLine) const
{
    if (!strCmdLine.isEmpty())
    {
        if (strCmdLine.at(0) == '"')
        {
            int iPos = strCmdLine.indexOf('"', 1);
            if (iPos > 0)
            {
                iPos = strCmdLine.lastIndexOf('\\', iPos);
                if (iPos > 0)
                    return strCmdLine.mid(1, iPos);
            }
        }
        else
        {
            int iPos = strCmdLine.length()-1;
            for (int i = 0; i < strCmdLine.length(); ++i)
                if (strCmdLine.at(i) == ' ' || strCmdLine.at(i) == '\t')
                {
                    iPos = i;
                    break;
                }
            iPos = strCmdLine.lastIndexOf('\\', iPos);
            if (iPos > 0)
                return strCmdLine.left(iPos+1);
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::fSaveCfg(const QString &strPath)
{
    Q_ASSERT(sitemModel->hasChildren());
    QString strEntries;
    for (int i = 0, iRowCount = sitemModel->rowCount(); i < iRowCount; ++i)
    {
        const QModelIndex modInd = sitemModel->index(i, 0);
        strEntries += 'a' + modInd.data(eRoleModifiers).toInt();
        const int iVirtualKey = modInd.data(eRoleVirtualKey).toInt();
        Q_ASSERT(*fKeyToString(iVirtualKey));
        strEntries += 'a' + (iVirtualKey >> 4);
        strEntries += 'a' + (iVirtualKey & 0xF);
        if (modInd.data(eRoleIsMsg).toBool())
            strEntries += '5' + slistPlugins.at(modInd.data(eRolePlugin).toInt()) + '\n' +
                    modInd.data(eRoleMessage).toString();
        else
        {
            const QString strCmdLine = modInd.data(eRoleCmdLine).toString(),
                    strWorkDir = modInd.data(eRoleWorkDir).toString();
            int iNum = modInd.data(eRoleShowCmd).toInt();
            Q_ASSERT(iNum >= 0 && iNum <= 6);
            switch (iNum)
            {
            //case 0: iNum = '1'; break;
            case 1: iNum = '7'; break;
            case 2: iNum = '3'; break;
            case 3: iNum = '0'; break;
            case 4: iNum = '4'; break;
            case 5: iNum = '2'; break;
            case 6: iNum = '6'; break;
            default:iNum = '1';
            }
            strEntries += iNum + strCmdLine + '\n' + (strWorkDir.isEmpty() ? fHintWorkDir(strCmdLine) : strWorkDir);
        }
        strEntries += '\n';
    }
    strEntries.chop(1);        //unwanted last line

    QFile file(strPath);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, 0, tr("Can't create file"));
        return;
    }

    file.write(static_cast<const char*>(static_cast<const void*>(strEntries.utf16())), strEntries.length()*sizeof(ushort));
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
    leCmdLine->clear();
    leWorkDir->clear();
    cbShowCmd->setCurrentIndex(0);
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
    const QString strPath = qApp->applicationDirPath() + ("/" MAIN_APP ".exe");
    if (strCurrentCfg.isEmpty() ?
            QProcess::startDetached(strPath) :
            QProcess::startDetached(strPath, QStringList(strCurrentCfg)))
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
         (hWndPrev = FindWindowEx(HWND_MESSAGE, hWndPrev, g_wGuidClass, 0));
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
            if (modInd.data(eRoleVirtualKey).toInt() == iNewVirtualKey && modInd.data(eRoleModifiers).toInt() == iNewModifiers)
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
            sitem->setData(iNewModifiers, eRoleModifiers);
            sitem->setData(iNewVirtualKey, eRoleVirtualKey);
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
    QString strPath = QFileDialog::getOpenFileName(this, 0, fHintWorkDir(leCmdLine->text()));
    if (!strPath.isEmpty())
    {
        if (strPath.contains(' '))
        {
            strPath.prepend('"');
            strPath += '"';
        }
        leCmdLine->setText(QDir::toNativeSeparators(strPath));
    }
}

//-------------------------------------------------------------------------------------------------
void HotKeysSettings::slotChangeWorkDir()
{
    const QString strPath = QFileDialog::getExistingDirectory(this, 0, fHintWorkDir(leCmdLine->text()));
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
        strTemp = leCmdLine->text();
        if (strTemp.isEmpty())
        {
            leCmdLine->setFocus();
            return;
        }
        if (leWorkDir->placeholderText().isEmpty() && leWorkDir->text().isEmpty())
        {
            leWorkDir->setFocus();
            return;
        }
    }

    for (int i = 0, iRowCount = sitemModel->rowCount(); i < iRowCount; ++i)
    {
        const QModelIndex modInd = sitemModel->index(i, 0);
        if (modInd.data(eRoleVirtualKey).toInt() == iVirtualKey && modInd.data(eRoleModifiers).toInt() == iModifiers)
        {
            QStandardItem *sitem = sitemModel->itemFromIndex(modInd);
            sitem->setFlags(sitem->flags() & ~Qt::ItemIsDropEnabled);
            sitem->setData(iModifiers, eRoleModifiers);
            sitem->setData(iVirtualKey, eRoleVirtualKey);
            if (bTypeIsMsg)
            {
                sitem->setData(true, eRoleIsMsg);
                sitem->setData(cbPlugin->currentIndex(), eRolePlugin);
                sitem->setData(strTemp, eRoleMessage);
                sitem->setIcon(QIcon(":/img/plugin.png"));
            }
            else
            {
                sitem->setData(false, eRoleIsMsg);
                sitem->setData(strTemp, eRoleCmdLine);
                sitem->setData(leWorkDir->text(), eRoleWorkDir);
                sitem->setData(cbShowCmd->currentIndex(), eRoleShowCmd);
                SHFILEINFO shFileInfo;
                if (::SHGetFileInfo(static_cast<const wchar_t*>(static_cast<const void*>(fGetFile(strTemp).utf16())),
                                    0, &shFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON) && shFileInfo.hIcon)
                {
                    const QPixmap pixmap = fFromWinHICON(shFileInfo.hIcon);
                    ::DestroyIcon(shFileInfo.hIcon);
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
    sitem->setData(iModifiers, eRoleModifiers);
    sitem->setData(iVirtualKey, eRoleVirtualKey);
    if (bTypeIsMsg)
    {
        sitem->setData(true, eRoleIsMsg);
        sitem->setData(cbPlugin->currentIndex(), eRolePlugin);
        sitem->setData(strTemp, eRoleMessage);
        sitem->setIcon(QIcon(":/img/plugin.png"));
    }
    else
    {
        sitem->setData(false, eRoleIsMsg);
        sitem->setData(strTemp, eRoleCmdLine);
        sitem->setData(leWorkDir->text(), eRoleWorkDir);
        sitem->setData(cbShowCmd->currentIndex(), eRoleShowCmd);
        SHFILEINFO shFileInfo;
        if (::SHGetFileInfo(static_cast<const wchar_t*>(static_cast<const void*>(fGetFile(strTemp).utf16())),
                            0, &shFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON) && shFileInfo.hIcon)
        {
            const QPixmap pixmap = fFromWinHICON(shFileInfo.hIcon);
            ::DestroyIcon(shFileInfo.hIcon);
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
    const bool bHwnd = ::FindWindowEx(HWND_MESSAGE, 0, g_wGuidClass, 0);
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
        iModifiers = modIndCurrent.data(eRoleModifiers).toInt();
        iVirtualKey = modIndCurrent.data(eRoleVirtualKey).toInt();
        leKeyComb->setText(modIndCurrent.data().toString());
        if (modIndCurrent.data(eRoleIsMsg).toBool())
        {
            cbPlugin->setCurrentIndex(modIndCurrent.data(eRolePlugin).toInt());
            leMsg->setText(modIndCurrent.data(eRoleMessage).toString());
            rbTypeMsg->setChecked(true);
            leCmdLine->clear();
            leWorkDir->clear();
            cbShowCmd->setCurrentIndex(0);
        }
        else
        {
            leCmdLine->setText(modIndCurrent.data(eRoleCmdLine).toString());
            leWorkDir->setText(modIndCurrent.data(eRoleWorkDir).toString());
            cbShowCmd->setCurrentIndex(modIndCurrent.data(eRoleShowCmd).toInt());
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
        leCmdLine->clear();
        leWorkDir->clear();
        cbShowCmd->setCurrentIndex(0);
        rbTypeFile->setChecked(true);
        if (leMsg)
            leMsg->clear();
        tbKeyComb->setEnabled(false);
        pbSaveCfg->setEnabled(false);
        pbSaveAsCfg->setEnabled(false);
    }
}
