#pragma once

#include <QtWidgets/QTreeWidget>
#include <QtCore/QThread>
#include <QtCore/QMimeData>
#include <networktables/NetworkTableInstance.h>

class ValueDisplayWidget : public QTreeWidget
{
	Q_OBJECT

public:
	ValueDisplayWidget(QWidget* parent);
	virtual ~ValueDisplayWidget() = default;

	void addNode(QString node);
	void removeNode(QString node);
	void updateNode(QString node, nt::Value value);

signals:
	void addNodeAsync(const QString& node);
	void removeNodeAsync(const QString& node);
	void updateNodeAsync(const QString& node, nt::Value value);

protected:
	QMimeData* mimeData(const QList<QTreeWidgetItem*>& items) const;

private:
	void addNodeInternal(QString node);
	void removeNodeInternal(QString node);
	void updateNodeInternal(QString node, nt::Value value);

	QTreeWidgetItem* getItemByName(const QString& name);

private:
	QThread* gui_thread_;
};

