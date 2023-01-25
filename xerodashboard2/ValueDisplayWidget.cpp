#include "ValueDisplayWidget.h"
#include "NTValueFormatting.h"
#include <networktables/BooleanTopic.h>
#include <networktables/BooleanArrayTopic.h>
#include <networktables/DoubleTopic.h>
#include <networktables/DoubleArrayTopic.h>
#include <networktables/FloatTopic.h>
#include <networktables/FloatArrayTopic.h>
#include <networktables/IntegerTopic.h>
#include <networktables/IntegerArrayTopic.h>
#include <networktables/StringTopic.h>
#include <networktables/StringArrayTopic.h>
#include <QtCore/QThread>

ValueDisplayWidget::ValueDisplayWidget(QWidget* parent) : QTreeWidget(parent)
{
	setColumnCount(2);

	setDragDropMode(DragDropMode::DragOnly);
	setDragEnabled(true);
	setSelectionMode(SelectionMode::SingleSelection);

	setHeaderLabels({ "Name", "Value" });

	gui_thread_ = QThread::currentThread();

	connect(this, &ValueDisplayWidget::addNodeAsync, this, &ValueDisplayWidget::addNodeInternal, Qt::QueuedConnection);
	connect(this, &ValueDisplayWidget::removeNodeAsync, this, &ValueDisplayWidget::removeNodeInternal, Qt::QueuedConnection);
	connect(this, &ValueDisplayWidget::updateNodeAsync, this, &ValueDisplayWidget::updateNodeInternal, Qt::QueuedConnection);
}

QMimeData* ValueDisplayWidget::mimeData(const QList<QTreeWidgetItem*>& items) const
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
		data->setText("NT:/" + path);
	}

	return data;
}

QTreeWidgetItem* ValueDisplayWidget::getItemByName(const QString& name)
{
	QStringList names = name.split("/");
	if (names.length() > 0 && names.at(0).length() == 0) {
		names.removeAt(0);
	}

	QTreeWidgetItem* searching = nullptr;

	while (names.length()) {
		QTreeWidgetItem* item = nullptr;

		if (searching == nullptr) {
			for (int i = 0; i < topLevelItemCount(); i++) {
				if (topLevelItem(i)->text(0) == names.at(0)) {
					item = topLevelItem(i);
					break;
				}
			}
		}
		else {
			for (int i = 0; i < searching->childCount(); i++) {
				if (searching->child(i)->text(0) == names.at(0)) {
					item = searching->child(i);
					break;
				}
			}
		}

		if (item == nullptr) {
			return nullptr;
		}

		names.removeAt(0);
		searching = item;
	}

	return searching;
}

void ValueDisplayWidget::updateNode(QString node, nt::Value value)
{
	if (QThread::currentThread() != gui_thread_) {
		emit updateNodeAsync(node, value);
	}
	else {
		updateNodeInternal(node, value);
	}
}

void ValueDisplayWidget::updateNodeInternal(QString node, nt::Value value)
{
	QTreeWidgetItem* item = getItemByName(node);
	item->setText(1, NTValueFormatting::getValueString(value));
}

void ValueDisplayWidget::addNode(QString node)
{
	if (QThread::currentThread() != gui_thread_) {
		emit addNodeAsync(node);
	}
	else {
		addNodeInternal(node);
	}
}

void ValueDisplayWidget::addNodeInternal(QString node)
{
	assert(QThread::currentThread() == gui_thread_);

	QStringList items = node.split("/");
	QTreeWidgetItem* item, * current;

	if (node.startsWith("/"))
		items.pop_front();

	current = nullptr;

	while (items.length() > 0)
	{
		QTreeWidgetItem* item = nullptr;

		QString itemname = items.front();
		items.pop_front();

		if (current == nullptr) {
			for (int i = 0; i < topLevelItemCount(); i++) {
				if (topLevelItem(i)->text(0) == itemname) {
					item = topLevelItem(i);
					break;
				}
			}

			if (item == nullptr) {
				item = new QTreeWidgetItem();
				item->setText(0, itemname);
				addTopLevelItem(item);
			}

		} 
		else {
			for (int i = 0; i < current->childCount(); i++) {
				if (current->child(i)->text(0) == itemname) {
					item = current->child(i);
					break;
				}
			}

			if (item == nullptr) {
				item = new QTreeWidgetItem();
				item->setText(0, itemname);
				current->addChild(item);
			}

		}

		current = item;
	}

	current->setData(0, Qt::UserRole, node);
}

void ValueDisplayWidget::removeNode(QString node)
{
	if (QThread::currentThread() != gui_thread_) {
		emit removeNodeAsync(node);
	}
	else {
		removeNodeInternal(node);
	}
}
void ValueDisplayWidget::removeNodeInternal(QString node)
{
	assert(QThread::currentThread() == gui_thread_);

	QStringList itemnames = node.split("/");
	QTreeWidgetItem* current = 0;
	int index = 0;

	if (node.startsWith("/"))
		itemnames.pop_front();

	while (itemnames.length() > 0)
	{
		QTreeWidgetItem* item = nullptr;

		QString itemname = itemnames.front();
		itemnames.pop_front();

		if (current == nullptr) {
			for (int i = 0; i < topLevelItemCount(); i++) {
				QString str = topLevelItem(i)->text(0);
				if (topLevelItem(i)->text(0) == itemname) {
					item = topLevelItem(i);
					break;
				}
			}

			if (item == nullptr)
				break;
		}
		else {
			for (int i = 0; i < current->childCount(); i++) {
				if (current->child(i)->text(0) == itemname) {
					item = current->child(i);
					break;
				}
			}

			if (item == nullptr)
				break;

		}

		current = item;
	}
}
