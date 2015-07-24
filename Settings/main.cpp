#include "hotkeyssettings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HotKeysSettings w;
    w.show();
    return a.exec();
}
