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
#include <QtCore/qt_windows.h>

class HotKeysSettings;

class ListViewEx : public QListView
{
    //Q_OBJECT not required
public:
    explicit ListViewEx(HotKeysSettings *parent);

private:
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
        const QString *pStrParam,
        *pStrWorkDir;
        union
        {
            const QString *pStrMessage,
            *pStrFile;
        };
        union
        {
            int iPlugin,
            iShowCmd;
        };
    };

    typedef QPixmap (*PFromWinHICON)(HICON hIcon);
    PFromWinHICON fFromWinHICON;

    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static QPixmap fNullPixmap(HICON);
    QPushButton *pbSaveCfg,
    *pbSaveAsCfg;
    QLabel *lblInfo;
    ListViewEx *listView;
    QLineEdit *leKeyComb,
    *leNewKeyComb;
    QToolButton *tbKeyComb;
    QRadioButton *rbTypeFile,
    *rbTypeMsg;
    QLineEdit *leFile,
    *leParam,
    *leWorkDir;
    QComboBox *cbShowMode,
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
    const char* fKeyToString(const DWORD dwKey) const;
    void fSaveCfg(const QString &strPath);

private slots:
    void slotOpenCfg();
    void slotSaveCfg();
    void slotSaveAsCfg();
    void slotRestart();
    void slotStop() const;
    void slotChangeKeyComb();
    inline void slotChangeType(const bool bTypeIsMsg)
    {
        stackWgt->setCurrentIndex(bTypeIsMsg);
    }
    void slotChangeFile();
    void slotChangeWorkDir();
    void slotOk() const;
    void slotDel() const;
    void slotTimerActive();
    void slotCurrentChanged(const QModelIndex &modIndCurrent);
};

#endif // HOTKEYSSETTINGS_H
