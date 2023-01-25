#pragma once
#include "qcustomplot.h"
#include "Plot.h"
#include "PlotData.h"
#include <QtCore/QRegularExpression>
#include <memory>

class GraphWidget : public QCustomPlot
{
	Q_OBJECT

public:
	GraphWidget(std::shared_ptr<Plot> plot, QWidget* parent);
	void updateAllData();
	void restart();

signals:
	void logMessage(const QString& msg);

protected:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void dragLeaveEvent(QDragLeaveEvent* event) override;
	void dropEvent(QDropEvent* ev) override;
	void keyPressEvent(QKeyEvent* ev) override;

private:

	enum class AxisType
	{
		Position,
		Velocity,
		Acceleration,

		Curvature,

		Angle,
		AngleVelocity,

		Unique
	};

private:
	void insertNode(const QString& node);


	void setupTimeAxis();
	QCPAxis* createAxis(const QString& name);

	void clear();
	void prepareCustomMenu(const QPoint& pos);
	void removeAll();
	void removeOne(const QString& name);

	void updateYAxisRange(QCPAxis* yaxis);
	void prettyAxis(double& minv, double& maxv);

	AxisType getAxisType(const QString& name);

	void showAverage();
	void updateData(const QString& node);

private:
	bool time_axis_;
	QStringList node_list_;

	QMap<AxisType, QCPAxis*> y_axis_by_type_;
	QMap<QString, QCPAxis*> y_axis_by_name_;
	QMap<QString, QCPGraph*> graphs_;
	bool left_right_;
	int color_index_;

	std::shared_ptr<Plot> plot_;
	std::shared_ptr<PlotData> data_;

	QVector<QCPAbstractItem*> avg_items_;

	bool show_averages_;

	static QVector<QColor> node_colors_;
	static QRegularExpression node_expr_;
	static QMap<QString, AxisType> unit_mapper_data_;


};

