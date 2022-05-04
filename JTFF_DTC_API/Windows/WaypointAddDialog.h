#pragma once

#include <QWidget>

#include "../Logger.h"
#include "../Utilities/Waypoint.h"
#include "ui_WaypointAddDialog.h"

class JTFF_DTC_API_EXPORT WaypointAddDialog : public QWidget
{
	Q_OBJECT

public:
	WaypointAddDialog(QWidget* parent = nullptr);
	~WaypointAddDialog();

	void SetFucntions(std::function<void(void)> onAccept, std::function<void(void)> onReject);
	std::pair<std::string, Waypoint> GetData();
	void SetData(const Waypoint& wpt);
	void hide();

private:
	Ui::WaypointAddDialog m_ui;
	std::shared_ptr<spdlog::logger> m_logger;
	std::function<void(void)> m_onAccept, m_onReject;
};
