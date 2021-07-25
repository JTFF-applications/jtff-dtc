#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent, DTCLogger* log, DTCSettings* settings)
    : QMainWindow(parent), m_logger(log), m_settings(settings)
{
    m_ui.setupUi(this);

    ConnectorCreator connector = { nullptr, "127.0.0.1", 7778, m_logger, m_settings };

    m_logger->debug("Creating airplanes windows...");
    m_tomcatWindow = new TomcatWindow(nullptr, connector, m_logger, m_settings);

    m_logger->debug("Linking MainWindow UI...");
    connect(m_ui.Quit, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(m_ui.F14, SIGNAL(clicked()), this, SLOT(ShowTomcatWindow()));
    //connect(m_ui.F16, SIGNAL(clicked()), this, SLOT(ShowViperWindow()));
    //connect(m_ui.F18, SIGNAL(clicked()), this, SLOT(ShowHornetWindow()));
    //connect(m_ui.A10C, SIGNAL(clicked()), this, SLOT(ShowWarthogWindow()));
}

MainWindow::~MainWindow()
{
    m_logger->debug("Deleting MainWindow...");
    delete m_tomcatWindow;
}

/*TomcatConnector tomcat(cc);

    std::unordered_map<Tomcat::UsableWaypoints, std::string> wpts = {
        {Tomcat::UsableWaypoints::WP1, "N24.32.6 E54.21.7 A1225 TITITUTU"},
        {Tomcat::UsableWaypoints::IP, "N25.32.6 E54.21.7 A12 TOTO"},
        {Tomcat::UsableWaypoints::ST, "N28.32.6 E57.21.7 A17530 ALPHA"}
    };

    Tomcat::NavgridParams navgrid = { "N24.32.6 E54.21.7 A1225 TITITUTU", 5, 180, 90 };

    tomcat.enterNavgrid(navgrid);*/