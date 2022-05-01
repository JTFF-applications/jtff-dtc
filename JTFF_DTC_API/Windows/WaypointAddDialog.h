#pragma once

#include <QWidget>

#include "../Logger.h"
#include "../Utilities/Waypoint.h"
#include "ui_WaypointAddDialog.h"

class JTFF_DTC_API_EXPORT WaypointAddDialog : public QWidget
{
	Q_OBJECT

public:
	WaypointAddDialog(std::function<void(void)> onAccept, std::function<void(void)> onReject, QWidget* parent = nullptr);
	~WaypointAddDialog();

	void Set(const Waypoint& wpt);

private:
	Ui::WaypointAddDialog m_ui;
	std::shared_ptr<spdlog::logger> m_logger;
	std::function<void(void)> m_onAccept, m_onReject;
};
