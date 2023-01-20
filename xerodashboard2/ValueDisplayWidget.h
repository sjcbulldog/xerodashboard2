#pragma once

#include <QtWidgets/QTreeWidget>
#include <QtCore/QThread>
#include <QtCore/QMimeData>
#include <networktables/NetworkTableInstance.h>

class ValueDisplayWidget : public QTreeWidget
{
	Q_OBJECT

	template <class T>
	struct ItemAndSubscriber
	{
		QTreeWidgetItem* item_;
		T sub_;
	};

public:
	ValueDisplayWidget(nt::NetworkTableInstance &inst, QWidget* parent);
	virtual ~ValueDisplayWidget() = default;

	void addNode(const QString& node);
	void removeNode(const QString& node);
	void updateNodes();

signals:
	void addNodeAsync(const QString& node);
	void removeNodeAsync(const QString& node);

protected:
	QMimeData* mimeData(const QList<QTreeWidgetItem*>& items) const;

private:
	void addNodeInternal(const QString& node);
	void removeNodeInternal(const QString& node);
	void descendItem(QTreeWidgetItem* item);
	void updateItem(QTreeWidgetItem* item);
	void removeSubscriber(QTreeWidgetItem* item);

private:
	QThread* gui_thread_;
	nt::NetworkTableInstance &inst_;
	int counter_;
	QMap<int, void*> maps_;
};

