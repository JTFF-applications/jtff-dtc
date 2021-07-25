#include "Windows/MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    DTCLogger log("log.txt");
    DTCSettings settings(&log);

    QApplication a(argc, argv);
    MainWindow w(nullptr, &log, &settings);
    w.show();
    return a.exec();
}
