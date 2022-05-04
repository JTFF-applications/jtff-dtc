#pragma once

#include <QDialog>

#include "../Logger.h"
#include "../Utilities/Waypoint.h"
#include "ui_WaypointAddDialog.h"

class JTFF_DTC_API_EXPORT WaypointAddDialog : public QDialog
{
	Q_OBJECT

public:
	WaypointAddDialog(const QStringList& available_wpts = { "WP1", "WP2", "WP3", "FP", "IP", "HB", "DP", "HA" }, QWidget* parent = nullptr);
	~WaypointAddDialog();

	void SetFunctions(std::function<void(void)> onAccept, std::function<void(void)> onReject);
	void SetCurrentWpt(const std::string& wpt);

	void SetData(const Waypoint& wpt);
	std::pair<const std::string, const Waypoint> GetData();

private:
	Ui::WaypointAddDialog m_ui;
	std::shared_ptr<spdlog::logger> m_logger;
	std::function<void(void)> m_onAccept, m_onReject;
};
