#pragma once
#include "Plot.h"
#include "GraphWidget.h"
#include "GraphNodeList.h"
#include "TabEditName.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QLayout>
#include <QtWidgets/QTabWidget>
#include <memory>

class PlotWidget : public QWidget
{
public:
	PlotWidget(std::shared_ptr<Plot> plot, QWidget* parrent);

protected:
	void resizeEvent(QResizeEvent *ev) override;
	void paintEvent(QPaintEvent* ev) override;
	void keyPressEvent(QKeyEvent* ev) override;

private:
	void splitterMoved(int pos, int index);

	void editTabText(int which);
	void editTabDone();
	void editTabAborted();
	void closeTab(int index = -1);

	void columnsDefined(QStringList columns);
	void restarted(const QString& name);
	void dataArrived(const QString& name);
	void complete(const QString& name);

private:
	std::shared_ptr<Plot> plot_;
	std::shared_ptr<PlotData> data_;
	QHBoxLayout* layout_;
	QSplitter* splitter_;
	GraphNodeList* node_list_;
	QTabWidget* tabs_;
	int leftWidth_;

	int which_tab_;
	TabEditName* editor_;
};

