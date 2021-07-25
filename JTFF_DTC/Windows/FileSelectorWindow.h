#ifndef FILESELECTORWINDOW_H
#define FILESELECTORWINDOW_H

#include <QWidget>
#include <QFileDialog>
#include <QFile>
#include <QXmlStreamReader>

#include <fstream>
#include <string>
#include <libzippp/libzippp.h>

#include "../DTCLogger.h"
#include "ui_FileSelectorWindow.h"

class FileSelectorWindow : public QWidget
{
	Q_OBJECT

public:
	FileSelectorWindow(QWidget* parent, const std::string& fileType, DTCLogger* logger);
	~FileSelectorWindow();

	Ui::FileSelectorWindow m_ui;

public slots:
	void SelectFile();

private:
	const std::string m_fileType;
	DTCLogger* m_logger;
};

#endif