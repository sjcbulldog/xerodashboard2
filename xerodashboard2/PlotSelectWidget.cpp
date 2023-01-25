#include "PlotSelectWidget.h"
#include "PlotManager.h"
#include <QtCore/QMimeData>

PlotSelectWidget::PlotSelectWidget(PlotManager &mgr, QWidget* parent) : QTreeWidget(parent), mgr_(mgr)
{
	setColumnCount(2);

	setDragDropMode(DragDropMode::DragOnly);
	setDragEnabled(true);
	setSelectionMode(SelectionMode::SingleSelection);

	setHeaderLabels({ "Name", "Points" });

	connect(&mgr_, &PlotManager::addedNode, this, &PlotSelectWidget::addNewPlot);

	loading_ = QBrush(QColor(0, 0, 255));
	complete_ = QBrush(QColor(0, 0, 0));
}

QMimeData* PlotSelectWidget::mimeData(const QList<QTreeWidgetItem*>& items) const
{
	QMimeData* data = nullptr;

	if (items.size() == 1)
	{
		QString path;
		QTreeWidgetItem* item = items.at(0);

		while (item)
		{
			if (path.length() > 0)
				path = item->text(0) + "/" + path;
			else
				path = item->text(0);

			item = item->parent();
		}

		data = new QMimeData();
		data->setText("PLOT:/" + path);
	}

	return data;
}

void PlotSelectWidget::addNewPlot(const QString& name)
{
	QTreeWidgetItem* item = getItemByName(name);
	if (item == nullptr) {
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setForeground(0, loading_);
		item->setText(0, name);
		item->setText(1, "-");
		addTopLevelItem(item);

		auto plot = mgr_.getPlot(name);
		if (plot != nullptr) {
			connect(plot.get(), &Plot::dataArrived, this, &PlotSelectWidget::dataArrived);
			connect(plot.get(), &Plot::complete, this, &PlotSelectWidget::plotComplete);
			connect(plot.get(), &Plot::restarted, this, &PlotSelectWidget::plotRestarted);
		}

		plot->setReady();
		plot->queryData();
		resizeColumnToContents(0);
	}
}

void PlotSelectWidget::dataArrived(const QString& name)
{
	auto plot = mgr_.getPlot(name);
	if (plot != nullptr) {
		int count = plot->dataPoints();
		QTreeWidgetItem* item = getItemByName(name);
		item->setText(1, QString::number(count));
	}
}
void PlotSelectWidget::plotComplete(const QString& name)
{
	QTreeWidgetItem* item = getItemByName(name);
	item->setForeground(0, complete_);
}

void PlotSelectWidget::plotRestarted(const QString& name)
{
	QTreeWidgetItem* item = getItemByName(name);
	item->setForeground(0, loading_);
}