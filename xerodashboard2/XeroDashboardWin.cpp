#include "XeroDashboardWin.h"
#include "build.h"
#include <QtCore/QProcessEnvironment>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>

XeroDashboardWin::XeroDashboardWin(QWidget* parent) : QMainWindow(parent)
{
	data_win_ = nullptr;
	data_win_dock_ = nullptr;
	plot_win_ = nullptr;
	plot_win_dock_ = nullptr;
	plots_ = nullptr;
	logwin_ = nullptr;
	log_win_dock_ = nullptr;

	connect(this, &XeroDashboardWin::logMessage, this, &XeroDashboardWin::putMessageInLogWindow, Qt::QueuedConnection);

	plots_ = new PlotManager(inst_);

	createWindows();
	createMenus();

	ipaddr_ = "127.0.0.1";
	name_ = "XeroDashBoard2";
	if (settings_.contains(NTServerIPSetting))
		ipaddr_ = settings_.value(NTServerIPSetting).toString();
	startNetworkTables(ipaddr_, name_);

	if (settings_.contains(GeometrySetting))
		restoreGeometry(settings_.value(GeometrySetting).toByteArray());

	if (settings_.contains(WindowStateSetting))
		restoreState(settings_.value(WindowStateSetting).toByteArray());
}

XeroDashboardWin::~XeroDashboardWin()
{
}

QString XeroDashboardWin::versionString()
{
	QString ver;

	ver = QString::number(XERO_MAJOR_VERSION);
	ver += "." + QString::number(XERO_MINOR_VERSION);
	ver += "." + QString::number(XERO_MICRO_VERSION);
	ver += "." + QString::number(XERO_BUILD_VERSION);

	return ver;
}

void XeroDashboardWin::showEvent(QShowEvent* ev)
{
	QString version;
	logwin_->appendPlainText("Xero Dashboard Version " + XeroDashboardWin::versionString());

	for (const QString& line : storedLogLines_) {
		logwin_->appendPlainText(line);
	}

	storedLogLines_.clear();
	QMainWindow::showEvent(ev);
}

void XeroDashboardWin::closeEvent(QCloseEvent* ev)
{
	nt::NetworkTableInstance::Destroy(inst_);

	settings_.setValue(GeometrySetting, saveGeometry());
	settings_.setValue(WindowStateSetting, saveState());

	QMainWindow::closeEvent(ev);
}

void XeroDashboardWin::createWindows()
{
	dashboard_view_ = new DashView(inst_, *plots_);
	setCentralWidget(dashboard_view_);

	data_win_ = new ValueDisplayWidget(nullptr);
	data_win_dock_ = new QDockWidget("NetworkTables");
	data_win_dock_->setObjectName("NetworkTables");
	data_win_dock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	data_win_dock_->setWidget(data_win_);
	addDockWidget(Qt::LeftDockWidgetArea, data_win_dock_);

	plot_win_ = new PlotSelectWidget(*plots_, nullptr);
	plot_win_dock_ = new QDockWidget("Plots");
	plot_win_dock_->setObjectName("Plots");
	plot_win_dock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	plot_win_dock_->setWidget(plot_win_);
	addDockWidget(Qt::LeftDockWidgetArea, plot_win_dock_);

	logwin_ = new QPlainTextEdit();
	log_win_dock_ = new QDockWidget("Log Window");
	log_win_dock_->setObjectName("LogWin");
	log_win_dock_->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	log_win_dock_->setWidget(logwin_);
	addDockWidget(Qt::BottomDockWidgetArea, log_win_dock_);
}

void XeroDashboardWin::createMenus()
{
	file_menu_ = new QMenu(tr("&File"));
	menuBar()->addMenu(file_menu_);

	window_menu_ = new QMenu(tr("&Windows"));
	menuBar()->addMenu(window_menu_);
	window_menu_->addAction(data_win_dock_->toggleViewAction());
	window_menu_->addAction(plot_win_dock_->toggleViewAction());
	window_menu_->addAction(log_win_dock_->toggleViewAction());
}

void XeroDashboardWin::startNetworkTables(const QString &ipaddr, const QString &name)
{
	bool v4 = false;
	inst_ = nt::NetworkTableInstance::GetDefault();

	if (!v4) {
		inst_.SetServer(ipaddr.toStdString().c_str(), nt::NetworkTableInstance::kDefaultPort3);
		inst_.StartClient3(name.toStdString());
	}
	else
	{
		inst_.SetServer(ipaddr.toStdString().c_str(), nt::NetworkTableInstance::kDefaultPort4);
		inst_.StartClient4(name.toStdString());

		QString prefix = plots_->getBaseName();
		if (prefix.endsWith("/")) {
			prefix = prefix.mid(0, prefix.length() - 1);
		}
		std::string p = prefix.toStdString();
		std::vector<std::string_view> prefixes;
		prefixes.push_back(p);
	}

	auto fn = std::bind(&XeroDashboardWin::listenNotify, this, std::placeholders::_1);
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	if (env.contains("XERO_DASHBOARD_LOG_NT_MESSAGES")) {		
		inst_.AddLogger(0, std::numeric_limits<int>::max(), fn);
	}

	int flags = 0xffffffff;
	std::vector<std::string_view> prefixes = { "" };
	listener_ = inst_.AddListener(prefixes, flags, fn);
}

void XeroDashboardWin::listenNotify(const nt::Event& ev)
{
	if ((ev.flags & NT_EVENT_PUBLISH) == NT_EVENT_PUBLISH)
	{
		QString name = QString::fromStdString(ev.GetTopicInfo()->name);
#ifdef DISPLAY_EVENTS
		qDebug() << "NT_EVENT_PUBLISH: " << name;
#endif
		data_win_->addNode(name);
		if (name.startsWith(plots_->getBaseName())) {
			plots_->addNode(name);
		}
	}
	else if ((ev.flags & NT_EVENT_UNPUBLISH) == NT_EVENT_UNPUBLISH)
	{
		QString name = QString::fromStdString(ev.GetTopicInfo()->name);
		data_win_->removeNode(name);
	}
	else if ((ev.flags & NT_EVENT_VALUE_REMOTE) == NT_EVENT_VALUE_REMOTE)
	{
		nt::Topic topic(ev.GetValueEventData()->topic);
		QString topicname = QString::fromStdString(topic.GetName());
		data_win_->updateNode(topicname, ev.GetValueEventData()->value);
		dashboard_view_->updateNode(topicname, ev.GetValueEventData()->value);
		if (topicname.startsWith(plots_->getBaseName())) {
			plots_->queryData();
		}
	}
	else if ((ev.flags & NT_EVENT_VALUE_LOCAL) == NT_EVENT_VALUE_LOCAL)
	{
		nt::Topic topic(ev.GetValueEventData()->topic);
		QString topicname = QString::fromStdString(topic.GetName());
		data_win_->updateNode(topicname, ev.GetValueEventData()->value);
		dashboard_view_->updateNode(topicname, ev.GetValueEventData()->value);

		if (topicname.startsWith(plots_->getBaseName())) {
			plots_->queryData();
		}
	}
	else if ((ev.flags & NT_EVENT_PROPERTIES) == NT_EVENT_PROPERTIES)
	{
	}
	else if ((ev.flags & NT_EVENT_IMMEDIATE) == NT_EVENT_IMMEDIATE)
	{
	}
	else if ((ev.flags & NT_EVENT_CONNECTED) == NT_EVENT_CONNECTED)
	{
	}
	else if ((ev.flags & NT_EVENT_DISCONNECTED) == NT_EVENT_DISCONNECTED)
	{
	}
	else if ((ev.flags & NT_EVENT_LOGMESSAGE) == NT_EVENT_LOGMESSAGE)
	{
		QString msg;

		msg += QString::fromStdString(ev.GetLogMessage()->filename);
		msg += ":" + QString::number(ev.GetLogMessage()->line);
		msg += ":" + QString::number(ev.GetLogMessage()->level);
		msg += ":" + QString::fromStdString(ev.GetLogMessage()->message);
		if (logwin_ != nullptr) {
			emit logMessage(msg);
		}
		else {
			storedLogLines_.push_back(msg);
		}
	}
	else 
	{
		qDebug() << "LISTEN NOTIFY - UNKNOWN";
	}
}
