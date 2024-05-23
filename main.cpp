#include "mainwindow.h"
#include <stdafx.h>



static const char *c_org_str = "natshch";
static const char *c_app_str = "FirstYear";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName(c_org_str);
    QCoreApplication::setApplicationName(c_app_str);
    MainWindow w;
    w.show();
    return a.exec();
}
