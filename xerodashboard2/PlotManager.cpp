#include "PlotManager.h"
#include <QtCore/QDebug>

PlotManager::PlotManager(nt::NetworkTableInstance& inst) : inst_(inst)
{
	base_name_ = "/XeroPlot/";
}

void PlotManager::addNode(const QString& name)
{
	QStringList list = name.split("/");
	if (list.size() > 2) {
		QString nodename = list[2];
		if (!plots_.contains(nodename))
		{
			auto plot = std::make_shared<Plot>(inst_, base_name_, nodename);
			plots_.insert(nodename, plot);
			emit addedNode(nodename);
		}
	}
}

void PlotManager::queryData()
{
	for (auto plot : plots_.values()) {
		plot->queryData();
	}
}
