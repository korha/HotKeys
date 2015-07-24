#ifndef HOTKEYSSETTINGS_H
#define HOTKEYSSETTINGS_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QToolButton>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QStandardItemModel>
#include <QtCore/QMimeData>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtCore/QTranslator>
#include <QtCore/qt_windows.h>

class ListViewEx : public QListView
{
    //Q_OBJECT not required
public:
    explicit ListViewEx(QWidget *parent);

private:
    explicit ListViewEx(const ListViewEx &);
    void operator=(const ListViewEx &);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
};

class HotKeysSettings : public QWidget
{
    Q_OBJECT
public:
    HotKeysSettings();
    ~HotKeysSettings();
    static HotKeysSettings *pApplication;
    void fOpenCfg(const QString &strPath);

public slots:
    void slotNewCfg();

private:
    struct TagEntry
    {
        int iModifiers,
        iVirtualKey;
        QString strKey;
        bool bTypeIsMsg;
        const QString *pCmdLine;
        union
        {
            const QString *pStrMessage,
            *pStrWorkDir;
        };
        union
        {
            int iPlugin,
            iShowCmd;
        };
    };
    enum
    {
        eRoleIsMsg = Qt::UserRole+1,
        eRoleModifiers,
        eRoleVirtualKey,
        eRolePlugin,
        eRoleMessage
    };
    enum
    {
        eRoleCmdLine = Qt::UserRole+4,
        eRoleWorkDir,
        eRoleShowCmd
    };

    typedef QPixmap (*PFromWinHICON)(HICON hIcon);
    PFromWinHICON fFromWinHICON;
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static QPixmap fNullPixmap(HICON);
    static const char *const cKeyToString[0xFF];
    QPushButton *pbSaveCfg,
    *pbSaveAsCfg;
    QLabel *lblInfo;
    ListViewEx *listView;
    QLineEdit *leKeyComb,
    *leNewKeyComb;
    QToolButton *tbKeyComb;
    QRadioButton *rbTypeFile,
    *rbTypeMsg;
    QLineEdit *leCmdLine,
    *leWorkDir;
    QComboBox *cbShowCmd,
    *cbPlugin;
    QLineEdit *leMsg;
    QStackedWidget *stackWgt;
    QSplitter *splitter;
    QStandardItemModel *const sitemModel;
    int iModifiersTemp,
    iModifiers,
    iVirtualKey,
    iNewModifiersTemp,
    iNewModifiers,
    iNewVirtualKey;
    QString strModifiers[16],
    strCurrentCfg;
    QStringList slistPlugins;
    bool bActive;
    const HHOOK hHook;
    bool fKeyboardProc(const bool bKeyDown, const DWORD dwKey);
    inline const char* fKeyToString(const DWORD dwKey) const
    {
        return dwKey < 0xFF ? cKeyToString[dwKey] : 0;
    }
    QString fGetFile(const QString &strCmdLine) const;
    QString fHintWorkDir(const QString &strCmdLine) const;
    void fSaveCfg(const QString &strPath);

private slots:
    void slotOpenCfg();
    void slotSaveCfg();
    void slotSaveAsCfg();
    void slotRestart();
    void slotStop() const;
    void slotChangeKeyComb();
    inline void slotChangeType(const bool bTypeIsMsg) const
    {
        stackWgt->setCurrentIndex(bTypeIsMsg);
    }
    inline void slotHintWorkDir(const QString &strCmdLine) const
    {
        leWorkDir->setPlaceholderText(fHintWorkDir(strCmdLine));
    }
    void slotChangeFile();
    void slotChangeWorkDir();
    void slotOk() const;
    void slotDel() const;
    void slotTimerActive();
    void slotCurrentChanged(const QModelIndex &modIndCurrent);
};

#endif // HOTKEYSSETTINGS_H
