#include "GraphNodeList.h"
#include "PlotData.h"
#include <QtCore/QMimeData>

GraphNodeList::GraphNodeList(std::shared_ptr<Plot> plot, QWidget* parent) : QTreeWidget(parent)
{
	plot_ = plot;

	setColumnCount(1);
	setHeaderHidden(true);
	setDragDropMode(DragDropMode::DragOnly);
	setDragEnabled(true);
	setSelectionMode(SelectionMode::SingleSelection);
	columnsDefined(plot->getPlotData()->columns());
}

QMimeData* GraphNodeList::mimeData(const QList<QTreeWidgetItem*>& items) const
{
	QMimeData* data = nullptr;

	if (items.size() > 0)
	{
		QString text;
		for (auto item : items) {
			if (text.length() > 0) {
				text += ",";
			}
			text += item->text(0);
		}

		data = new QMimeData();
		data->setText(text);
	}

	return data;
}

void GraphNodeList::restarted()
{
	clear();
}

void GraphNodeList::columnsDefined(const QStringList &columns)
{
	clear();

	for (const QString& str : columns) 
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0, str);
		addTopLevelItem(item);
	}
}

void GraphNodeList::dataArrived(const QString &name)
{
}