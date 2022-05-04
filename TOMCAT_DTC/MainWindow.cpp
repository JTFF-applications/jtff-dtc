#include <QMessageBox>

#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);

	QObject::connect(m_ui.add_wpt_btn, &QPushButton::clicked, this, &MainWindow::on_add_wpt_clicked);
	QObject::connect(m_ui.rm_wpt_btn, &QPushButton::clicked, this, &MainWindow::on_rm_wpt_clicked);
	QObject::connect(m_ui.modify_wpy_btn, &QPushButton::clicked, this, &MainWindow::on_modify_wpt_clicked);
	QObject::connect(m_ui.import_file_btn, &QPushButton::clicked, this, &MainWindow::on_import_file_clicked);
	QObject::connect(m_ui.export_file_btn, &QPushButton::clicked, this, &MainWindow::on_export_file_clicked);
	QObject::connect(m_ui.import_cf_btn, &QPushButton::clicked, this, &MainWindow::on_import_cf_clicked);
	QObject::connect(m_ui.export_ac_btn, &QPushButton::clicked, this, &MainWindow::on_export_ac_clicked);
}

void MainWindow::on_add_wpt_clicked()
{
	if (m_waypoints.size() + 1 > m_maxWaypoints)
	{
		QMessageBox(QMessageBox::Icon::Warning, "Tomcat Error", "You can't enter more waypoint for the tomcat !", QMessageBox::Ok).exec();
		return;
	}

	m_wpt_dialog.show();
	m_wpt_dialog.SetFucntions([&] {
		auto data = m_wpt_dialog.GetData();
		if (data.first.empty())
			return;
		m_waypoints.push_back(data);
		m_wpt_dialog.hide();
		},
		[&] {
			m_wpt_dialog.hide();
		});
}

void MainWindow::on_rm_wpt_clicked()
{
}

void MainWindow::on_modify_wpt_clicked()
{
}

void MainWindow::on_import_file_clicked()
{
}

void MainWindow::on_export_file_clicked()
{
}

void MainWindow::on_import_cf_clicked()
{
}

void MainWindow::on_export_ac_clicked()
{
}
