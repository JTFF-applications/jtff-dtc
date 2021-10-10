#include "Windows/TomcatWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("./icon.png"));

    DTCLogger* logger = new DTCLogger("log.log");
    DTCSettings* settings = new DTCSettings(logger);
    ConnectorCreator connector = { nullptr, "127.0.0.1", 7778, logger, settings };

    TomcatWindow* w = new TomcatWindow(nullptr, connector, logger, settings);
    w->show();
    
    a.exec();
    delete w, settings, logger;
    return EXIT_SUCCESS;
}
