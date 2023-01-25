#include "GraphWidget.h"
#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>

QRegularExpression GraphWidget::node_expr_(".*\\((.*)\\)");
QMap<QString, GraphWidget::AxisType> GraphWidget::unit_mapper_data_;

GraphWidget::GraphWidget(std::shared_ptr<Plot> plot, QWidget *parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &GraphWidget::customContextMenuRequested, this, &GraphWidget::prepareCustomMenu);

	setAcceptDrops(true);

	plot_ = plot;
	left_right_ = true;
	color_index_ = 0;
	time_axis_ = false;
	show_averages_ = false;

	axisRect(0)->removeAxis(yAxis);
	axisRect(0)->removeAxis(yAxis2);

	setAutoAddPlottableToLegend(true);
	legend->setVisible(true);

	setInteraction(QCP::iRangeDrag);
	setInteraction(QCP::iRangeZoom);

	if (unit_mapper_data_.size() == 0) {
		unit_mapper_data_.insert("m", AxisType::Position);
		unit_mapper_data_.insert("m/s", AxisType::Velocity);
		unit_mapper_data_.insert("m/s/s", AxisType::Acceleration);
		unit_mapper_data_.insert("deg", AxisType::Angle);
		unit_mapper_data_.insert("deg/s", AxisType::AngleVelocity);
	}
}

void GraphWidget::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key_A) {
		if (show_averages_) {
			show_averages_ = false;
			for (auto item : avg_items_) {
				removeItem(item);
			}
			avg_items_.clear();
			replot();
		}
		else {
			show_averages_ = true;
			showAverage();
		}
	}
	else if (ev->key() == Qt::Key_L) {
		if (legend->visible())
			legend->setVisible(false);
		else
			legend->setVisible(true);
		replot();
	}
}

void GraphWidget::showAverage()
{
	for (auto item : avg_items_) {
		removeItem(item);
	}
	avg_items_.clear();

	if (node_list_.size() == 0) {
		QMessageBox::warning(this, "No Nodes", "There are no values being graphed, so cannot calculate averages");
		return;
	}

	for (const QString& node : node_list_) {
		int col = data_->columns().indexOf(node);
		if (col != -1 && graphs_.contains(node)) {
			QCPGraph* gr = graphs_.value(node);

			double minv, maxv;
			data_->getMinMax(col, minv, maxv);

			QVector<PlotData::LevelAverage> results = data_->getAverages(col);
			for(int i = 0 ; i < results.size() ; i++) {

				QCPAxis* axis = y_axis_by_name_.value(node);

				double bottom = results[i].maxv + 0.01 * (maxv - minv);

				QCPItemLine* line = new QCPItemLine(this);
				line->start->setAxes(xAxis, axis);
				line->end->setAxes(xAxis, axis);
				line->setHead(QCPLineEnding::esSpikeArrow);
				line->setTail(QCPLineEnding::esSpikeArrow);
				QPen pen(QColor(0, 0, 255));
				pen.setWidth(1.0);
				line->setPen(pen);
				line->start->setCoords(results[i].start, bottom);
				line->end->setCoords(results[i].end, bottom);
				avg_items_.push_back(line);

				line = new QCPItemLine(this);
				line->start->setAxes(xAxis, axis);
				line->end->setAxes(xAxis, axis);
				line->setHead(QCPLineEnding::esNone);
				line->setTail(QCPLineEnding::esNone);
				pen = QPen(QColor(0, 0, 0));
				line->setPen(pen);
				line->start->setCoords(results[i].start, bottom);
				line->end->setCoords(results[i].start, results[i].average);
				avg_items_.push_back(line);

				line = new QCPItemLine(this);
				line->start->setAxes(xAxis, axis);
				line->end->setAxes(xAxis, axis);
				line->setHead(QCPLineEnding::esNone);
				line->setTail(QCPLineEnding::esNone);
				line->setPen(pen);
				line->start->setCoords(results[i].end, bottom);
				line->end->setCoords(results[i].end, results[i].average);
				avg_items_.push_back(line);

				QCPItemText* text = new QCPItemText(this);
				text->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
				text->position->setAxes(xAxis, axis);
				text->position->setCoords((results[i].start + results[i].end) / 2.0, bottom);
				text->setText(QString::number(results[i].average, 'f', 4));
				QFont font = text->font();
				font.setPointSizeF(8.0);
				font.setBold(false);
				text->setFont(font);
				QString label = node + " " + QString::number(results[i].average, 'f', 4);
				text->setText(label);
				avg_items_.push_back(text);
			}
		}
	}

	replot();
}

void GraphWidget::clear()
{
	clearGraphs();
	graphs_.clear();

	for (auto axis : y_axis_by_name_.values())
	{
		axisRect(0)->removeAxis(axis);
	}
	y_axis_by_name_.clear();
	y_axis_by_type_.clear();

	color_index_ = 0;
}

GraphWidget::AxisType GraphWidget::getAxisType(const QString& name)
{
	AxisType ret = AxisType::Unique;

	QRegularExpressionMatch m = node_expr_.match(name);
	if (m.hasMatch()) {
		QString units = m.captured(1);
		if (unit_mapper_data_.contains(units)) {
			ret = unit_mapper_data_.value(units);
		}
		else {
			QString msg = "no axis type for units '" + units + "'";
			logMessage(msg);
		}
	}
	return ret;
}

QCPAxis* GraphWidget::createAxis(const QString& name)
{
	QCPAxis* axis = nullptr;

	AxisType type = getAxisType(name);
	if (y_axis_by_type_.contains(type)) {
		axis = y_axis_by_type_.value(type);
	}
	else {
		if (left_right_) {
			axis = axisRect()->addAxis(QCPAxis::AxisType::atLeft);
		}
		else {
			axis = axisRect()->addAxis(QCPAxis::AxisType::atRight);
		}
		QPen pen = axis->basePen();
		pen.setWidth(1.5);
		axis->setBasePen(pen);

		switch (type) {
		case AxisType::Acceleration:
			axis->setLabel("acceleration (m/s/s)");
			break; 
		case AxisType::Angle:
			axis->setLabel("angle (deg)");
			axis->setRange(-180.0, 180.0);
			break;
		case AxisType::AngleVelocity:
			axis->setLabel("angular velocity (deg/s)");
			break;
		case AxisType::Curvature:
			axis->setLabel("curvature");
			break;
		case AxisType::Position:
			axis->setLabel("distance (m)");
			break;
		case AxisType::Velocity:
			axis->setLabel("velocity (m/s/");
			break;
		}

		left_right_ = !left_right_;

		if (type != AxisType::Unique) {
			y_axis_by_type_.insert(type, axis);
		}
	}

	return axis;
}

void GraphWidget::setupTimeAxis()
{
	QPen pen = xAxis->basePen();
	pen.setWidth(1.5);
	xAxis->setBasePen(pen);
	xAxis->setLabel("time (s)");
	time_axis_ = true;
}

void GraphWidget::restart()
{
	for (auto item : avg_items_) {
		removeItem(item);
	}
	avg_items_.clear();
	updateAllData();

}

void GraphWidget::updateAllData()
{
	data_ = plot_->getPlotData();
	for (const QString& node : node_list_) {
		updateData(node);
	}

	if (show_averages_) {
		showAverage();
	}
}

void GraphWidget::updateYAxisRange(QCPAxis* yaxis)
{
	double tminv, tmaxv;
	bool first = true;

	for (const QString& name : y_axis_by_name_.keys()) {
		QCPAxis* ax = y_axis_by_name_[name];
		if (ax == yaxis)
		{
			int col = data_->columns().indexOf(name);
			if (col != -1) {
				double minv, maxv;
				data_->getMinMax(col, minv, maxv);
				if (first) {
					tminv = minv;
					tmaxv = maxv;
					first = false;
				}
				else {
					if (minv < tminv) {
						tminv = minv;
					}

					if (maxv > tmaxv) {
						tmaxv = maxv;
					}
				}
			}
		}
	}

	prettyAxis(tminv, tmaxv);
	yaxis->setRange(tminv, tmaxv);
}

void GraphWidget::updateData(const QString &node)
{
	assert(y_axis_by_name_.contains(node));
	assert(graphs_.contains(node));

	//
	// Get fresh data for the plot
	//
	data_ = plot_->getPlotData();

	//
	// Set the range for the time axis
	//
	double minv, maxv;
	data_->getMinMax(0, minv, maxv);
	xAxis->setRange(minv, maxv);

	//
	// Get the column number for the node of interest
	//
	int col = data_->columns().indexOf(node);
	if (col == -1) {
		return;
	}

	QCPAxis* yaxis = y_axis_by_name_.value(node);
	updateYAxisRange(yAxis);

	QCPGraph* gr = graphs_.value(node);
	gr->setData(data_->getCol(0), data_->getCol(col));
	replot();
}

void GraphWidget::prettyAxis(double& minv, double& maxv)
{
	double range = maxv - minv;

	int minvi = (int)minv;
	int maxvi = (int)maxv;

	if (range < 10.0) {
		//
		// Do nothing
		//
	}
	else if (range < 100.0) {
		minvi /= 10;
		maxvi /= 10;
	}
	else if (range <= 1000) {
		minvi /= 100;
		maxvi /= 100;
	}
	maxvi++;

	if (range < 10.0) {
		minv = minvi;
		maxv = maxvi;
	}
	else if (range < 100.0) {
		minv = minvi * 10.0;
		maxv = maxvi * 10.0;
	}
	else if (range <= 1000) {
		minv = minvi * 100.0;
		maxv = maxvi * 100.0;
	}
}

void GraphWidget::insertNode(const QString& node)
{
	if (!node_list_.contains(node))
	{
		//
		// Initialize the time axis (column 0 in the data)
		//
		if (!time_axis_)
		{
			setupTimeAxis();
		}

		//
		// Create the Y axis and store it
		//
		QCPAxis* myyaxis = createAxis(node);
		myyaxis->setRange(0.0, 1.0);
		y_axis_by_name_.insert(node, myyaxis);

		//
		// Create the graph and store it
		//
		QCPGraph* gr = addGraph(xAxis, myyaxis);
		graphs_.insert(node, gr);
		QPen pen(node_colors_.at(color_index_));
		pen.setWidth(2.0);
		gr->setPen(pen);
		gr->setName(node);

		//
		// Set the color for this node
		//
		color_index_++;
		if (color_index_ == node_colors_.size())
			color_index_ = 0;

		node_list_.push_back(node);
	}

	updateData(node);
}

void GraphWidget::dragEnterEvent(QDragEnterEvent* ev)
{
	setBackgroundRole(QPalette::Highlight);
	ev->setDropAction(Qt::DropAction::CopyAction);
	ev->acceptProposedAction();
}

void GraphWidget::dragMoveEvent(QDragMoveEvent* ev)
{
	ev->acceptProposedAction();
}

void GraphWidget::dragLeaveEvent(QDragLeaveEvent* ev)
{
	setBackgroundRole(QPalette::Window);
}

void GraphWidget::dropEvent(QDropEvent* ev)
{
	const char* fmttext = "text/plain";
	QString text;

	const QMimeData* data = ev->mimeData();
	QStringList fmts = data->formats();
	if (data->hasFormat(fmttext))
	{
		QByteArray encoded = data->data(fmttext);
		QString text = QString::fromUtf8(encoded);
		QStringList list = text.split(",");
		for (const QString& node : list) {
			insertNode(node.trimmed());
		}
	}
	setBackgroundRole(QPalette::Window);
}

void GraphWidget::removeOne(const QString& node)
{
	node_list_.removeAll(node);
	if (graphs_.contains(node))
	{
		auto gr = graphs_[node];
		graphs_.remove(node);
		removeGraph(gr);
	}
	replot();
}

void GraphWidget::removeAll()
{
	clear();
	node_list_.clear();
	replot();
}

void GraphWidget::prepareCustomMenu(const QPoint& pos)
{
	QMenu menu(this);
	QAction* act;

	//
	// This is a group item: delete group, add path
	//
	act = new QAction(tr("Remove All"));
	connect(act, &QAction::triggered, this, &GraphWidget::removeAll);
	menu.addAction(act);

	for (const QString& node : node_list_) {
		act = new QAction("Remove '" + node + "'");
		connect(act, &QAction::triggered, [this, node]() { this->removeOne(node); });
		menu.addAction(act);
	}

	menu.exec(this->mapToGlobal(pos));
}


QVector<QColor> GraphWidget::node_colors_ =
{
	QColor(0, 0, 0),
	QColor(255, 0, 0),
	QColor(0, 255, 0),
	QColor(0, 0, 255),
	QColor(255, 165, 0),
	QColor(0, 255, 255),
	QColor(255, 0, 255)
};