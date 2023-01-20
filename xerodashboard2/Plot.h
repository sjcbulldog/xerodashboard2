#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QMutex>
#include <networktables/StringArrayTopic.h>
#include <networktables/BooleanTopic.h>
#include <networktables/DoubleTopic.h>
#include <networktables/DoubleArrayTopic.h>
#include <networktables/IntegerTopic.h>

class PlotData;

class Plot : public QObject
{
	Q_OBJECT

public:
	Plot(nt::NetworkTableInstance& inst, const QString &basename, const QString& name);

	void queryData();
	std::shared_ptr<PlotData> getPlotData();
	int dataPoints();

signals:
	void restarted(const QString &name);
	void dataArrived(const QString &name);
	void complete(const QString &name);
	void logMessage(const QString& msg);

private:
	void initColumns(const std::vector<std::string>& colstrs);
	void reset();
	bool compareCols(const std::vector<std::string>& colstrs);

	bool readV3Data();
	bool readV4Data();

	int dataPointsNoLock();

private:
	QString name_;
	int version_;
	QStringList columns_;
	QVector<QVector<double>> data_;

	nt::StringArraySubscriber column_subscriber_;
	nt::BooleanSubscriber complete_subscriber_;
	nt::IntegerSubscriber count_subscriber_;
	nt::IntegerSubscriber points_subscriber_;
	nt::IntegerSubscriber version_subscriber_;
	QVector<nt::DoubleSubscriber *> data_subscribers_;

	nt::NetworkTableInstance& inst_;

	bool complete_;

	QString plot_base_name_;
	QMutex lock_;
};
