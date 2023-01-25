#pragma once

#include "Plot.h"
#include <QtWidgets/QTreeWidget>
#include <memory>

class GraphNodeList : public QTreeWidget
{
public:
	GraphNodeList(std::shared_ptr<Plot> plot, QWidget* parent);

	void restarted();
	void columnsDefined(const QStringList& cols);
	void dataArrived(const QString& name);

protected:
	QMimeData* mimeData(const QList<QTreeWidgetItem*>& items) const;

private:
	std::shared_ptr<Plot> plot_;
};

