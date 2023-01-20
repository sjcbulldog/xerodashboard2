#pragma once

#include "Plot.h"
#include <networktables/NetworkTableInstance.h>
#include <QtCore/QString>
#include <QtCore/QMap>

class PlotManager : public QObject
{
	Q_OBJECT

public:
	PlotManager(nt::NetworkTableInstance& inst);
	void addNode(const QString& name);

	void queryData();

	std::shared_ptr<Plot> getPlot(const QString& name) {
		if (!plots_.contains(name))
			return nullptr;

		return plots_.value(name);
	}

	const QString& getBaseName() const {
		return base_name_;
	}

signals:
	void addedNode(const QString& name);

private:
	QMap<QString, std::shared_ptr<Plot>> plots_;
	nt::NetworkTableInstance& inst_;
	QString base_name_;
};

