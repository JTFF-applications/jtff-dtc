#pragma once

#include <QWidget>
#include "ui_HornetWindow.h"

class HornetWindow : public QWidget
{
	Q_OBJECT

public:
	HornetWindow(QWidget *parent = Q_NULLPTR);
	~HornetWindow();

private:
	Ui::HornetWindow ui;
};
