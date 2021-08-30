#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QMessageLogger>

#include "TomcatWindow.h"
#include "../Connectors/BaseConnector.h"
#include "../Connectors/TomcatConnector.h"
#include "../DTCUtils.h"
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent, DTCLogger* log, DTCSettings* settings);
    ~MainWindow();

private slots:
    void ShowTomcatWindow() { m_tomcatWindow->show(); };
    //void ShowViperWindow() { m_viperWindow->show(); };
    //void ShowHornetWindow() { m_hornetWindow->show(); };
    //void ShowWarthogWindow() { m_warthogWindow->show(); };

    void CloseTomcatWindow() { m_tomcatWindow->hide(); };
    //void CloseViperWindow() { m_viperWindow->hide(); };
    //void CloseHornetWindow() { m_hornetWindow->hide(); };
    //void CloseWarthogWindow() { m_warthogWindow->hide(); };

private:
    Ui::MainWindowClass m_ui;

    TomcatWindow* m_tomcatWindow;
    DTCLogger* m_logger;
    DTCSettings* m_settings;
};

#endif //!MAINWINDOW_H