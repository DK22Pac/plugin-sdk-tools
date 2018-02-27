#include "projectwizard.h"
#include <QtWidgets/QApplication>
#include "BorderlessStyle.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NativeEventFilter filterObj;
    a.installNativeEventFilter(&filterObj);
    ProjectWizard w;
    w.show();
    return a.exec();
}
