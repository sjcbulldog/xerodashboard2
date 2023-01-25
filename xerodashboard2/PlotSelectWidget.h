#pragma once

#include <QtWidgets/QTreeWidget>

class PlotManager;

class PlotSelectWidget : public QTreeWidget
{
public:
	PlotSelectWidget(PlotManager &mgr, QWidget* parent);
	virtual ~PlotSelectWidget() = default;

protected:
	QMimeData* mimeData(const QList<QTreeWidgetItem*>& items) const;

private:
	void addNewPlot(const QString& name);
	void dataArrived(const QString& name);
	void plotComplete(const QString& name);
	void plotRestarted(const QString& name);

	QTreeWidgetItem* getItemByName(const QString& name) {
		for (int i = 0; i < topLevelItemCount(); i++) {
			QTreeWidgetItem* item = topLevelItem(i);
			if (item->text(0) == name)
				return item;
		}

		return nullptr;
	}

private:
	PlotManager& mgr_;
	QBrush complete_;
	QBrush loading_;
};

