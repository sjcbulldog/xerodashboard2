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

ValueDisplayWidget::ValueDisplayWidget(nt::NetworkTableInstance &inst, QWidget* parent) : QTreeWidget(parent), inst_(inst)
{
	counter_ = 0;
	inst_ = inst;
	setColumnCount(2);

	setDragDropMode(DragDropMode::DragOnly);
	setDragEnabled(true);
	setSelectionMode(SelectionMode::SingleSelection);

	setHeaderLabels({ "Name", "Value" });

	gui_thread_ = QThread::currentThread();

	connect(this, &ValueDisplayWidget::addNodeAsync, this, &ValueDisplayWidget::addNodeInternal, Qt::QueuedConnection);
	connect(this, &ValueDisplayWidget::removeNodeAsync, this, &ValueDisplayWidget::removeNodeInternal, Qt::QueuedConnection);
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


void ValueDisplayWidget::addNode(const QString& node)
{
	if (QThread::currentThread() != gui_thread_) {
		emit addNodeAsync(node);
	}
	else {
		addNodeInternal(node);
	}
}

void ValueDisplayWidget::addNodeInternal(const QString& node)
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

	if (current) 
	{
		auto info = inst_.GetTopicInfo(node.toStdString());
		current->setData(1, Qt::UserRole, -1);
		switch (info.front().type)
		{
			case NT_BOOLEAN:
			{
				nt::BooleanTopic topic = inst_.GetBooleanTopic(node.toStdString());
				ItemAndSubscriber<nt::BooleanSubscriber>* t = new ItemAndSubscriber<nt::BooleanSubscriber>();
				t->sub_ = topic.Subscribe(false);
				int cnt = counter_++;
				maps_.insert(cnt, (void*)t);
				current->setData(1, Qt::UserRole, cnt);
			}
			break;

		case NT_BOOLEAN_ARRAY:
			{
				nt::BooleanArrayTopic topic = inst_.GetBooleanArrayTopic(node.toStdString());
				ItemAndSubscriber<nt::BooleanArraySubscriber>* t = new ItemAndSubscriber<nt::BooleanArraySubscriber>();
				t->sub_ = topic.Subscribe({});
				int cnt = counter_++;
				maps_.insert(cnt, (void*)t);
				current->setData(1, Qt::UserRole, cnt);
			}
			break;

		case NT_DOUBLE:
			{
				nt::DoubleTopic topic = inst_.GetDoubleTopic(node.toStdString());
				ItemAndSubscriber<nt::DoubleSubscriber>* t = new ItemAndSubscriber<nt::DoubleSubscriber>();
				t->sub_ = topic.Subscribe(0.0);
				int cnt = counter_++;
				maps_.insert(cnt, (void*)t);
				current->setData(1, Qt::UserRole, cnt);
			}
			break;

		case NT_DOUBLE_ARRAY:
			{
				nt::DoubleArrayTopic topic = inst_.GetDoubleArrayTopic(node.toStdString());
				ItemAndSubscriber<nt::DoubleArraySubscriber>* t = new ItemAndSubscriber<nt::DoubleArraySubscriber>();
				t->sub_ = topic.Subscribe({});
				int cnt = counter_++;
				maps_.insert(cnt, (void*)t);
				current->setData(1, Qt::UserRole, cnt);
			}
			break;

		case NT_FLOAT:
			{
				nt::FloatTopic topic = inst_.GetFloatTopic(node.toStdString());
				ItemAndSubscriber<nt::FloatSubscriber>* t = new ItemAndSubscriber<nt::FloatSubscriber>();
				t->sub_ = topic.Subscribe(0.0f);
				int cnt = counter_++;
				maps_.insert(cnt, (void*)t);
				current->setData(1, Qt::UserRole, cnt);
			}
			break;

		case NT_FLOAT_ARRAY:
			{
				nt::FloatArrayTopic topic = inst_.GetFloatArrayTopic(node.toStdString());
				ItemAndSubscriber<nt::FloatArraySubscriber>* t = new ItemAndSubscriber<nt::FloatArraySubscriber>();
				t->sub_ = topic.Subscribe({});
				int cnt = counter_++;
				maps_.insert(cnt, (void*)t);
				current->setData(1, Qt::UserRole, cnt);
			}
			break;

		case NT_INTEGER:
			{
				nt::IntegerTopic topic = inst_.GetIntegerTopic(node.toStdString());
				ItemAndSubscriber<nt::IntegerSubscriber>* t = new ItemAndSubscriber<nt::IntegerSubscriber>();
				t->sub_ = topic.Subscribe(0);
				int cnt = counter_++;
				maps_.insert(cnt, (void*)t);
				current->setData(1, Qt::UserRole, cnt);
			}
			break;

		case NT_INTEGER_ARRAY:
			{
				nt::IntegerArrayTopic topic = inst_.GetIntegerArrayTopic(node.toStdString());
				ItemAndSubscriber<nt::IntegerArraySubscriber>* t = new ItemAndSubscriber<nt::IntegerArraySubscriber>();
				t->sub_ = topic.Subscribe({});
				int cnt = counter_++;
				maps_.insert(cnt, (void*)t);
				current->setData(1, Qt::UserRole, cnt);
			}
			break;

		case NT_STRING:
			{
				nt::StringTopic topic = inst_.GetStringTopic(node.toStdString());
				ItemAndSubscriber<nt::StringSubscriber>* t = new ItemAndSubscriber<nt::StringSubscriber>();
				t->sub_ = topic.Subscribe("");
				int cnt = counter_++;
				maps_.insert(cnt, (void*)t);
				current->setData(1, Qt::UserRole, cnt);
			}
			break;

		case NT_STRING_ARRAY:
			{
				nt::StringArrayTopic topic = inst_.GetStringArrayTopic(node.toStdString());
				ItemAndSubscriber<nt::StringArraySubscriber>* t = new ItemAndSubscriber<nt::StringArraySubscriber>();
				t->sub_ = topic.Subscribe({});
				int cnt = counter_++;
				maps_.insert(cnt, (void*)t);
				current->setData(1, Qt::UserRole, cnt);
			}
			break;
		}
		current->setData(0, Qt::UserRole, node);
		current->setText(1, NTValueFormatting::getValueString(inst_, node));
	}
}

void ValueDisplayWidget::removeNode(const QString& node)
{
	if (QThread::currentThread() != gui_thread_) {
		emit removeNodeAsync(node);
	}
	else {
		removeNodeInternal(node);
	}
}
void ValueDisplayWidget::removeNodeInternal(const QString& node)
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

	if (current) {
		if (itemnames.length() == 0) {
			removeSubscriber(current);
			auto parent = current->parent();
			delete current;
			current = parent;

		}

		//
		// Now, delete the tree all of the way up if there are no entries
		//
		while (current) {
			QString str = current->text(0);
			auto parent = current->parent();

			if (current->childCount() > 1) {
				break;
			}
			else
			{
				if (parent == nullptr) {
					int which = -1;
					for(int i = 0; i < topLevelItemCount(); i++) {
						if (topLevelItem(i) == current) {
							which = i;
							break;
						}
					}

					if (which != -1) {
						delete takeTopLevelItem(which);
					}
				}
				else {
					parent->removeChild(current);
					delete current;
				}
			}
			current = parent;
		}
	}
}

void ValueDisplayWidget::removeSubscriber(QTreeWidgetItem* item)
{
	QString node = item->data(0, Qt::UserRole).toString();
	nt::Topic tinfo = inst_.GetTopic(node.toStdString());
	int index = item->data(1, Qt::UserRole).toInt();

	if (!maps_.contains(index))
		return;

	switch (tinfo.GetType())
	{
	case nt::NetworkTableType::kBoolean:
		{
			ItemAndSubscriber<nt::BooleanSubscriber>* item = static_cast<ItemAndSubscriber<nt::BooleanSubscriber>*>(maps_.value(index));
			delete item;
		}
		break;
		case nt::NetworkTableType::kBooleanArray:
		{
			ItemAndSubscriber<nt::BooleanArraySubscriber>* item = static_cast<ItemAndSubscriber<nt::BooleanArraySubscriber>*>(maps_.value(index));
			delete item;
		}
		break;
	case nt::NetworkTableType::kDouble:
		{
			ItemAndSubscriber<nt::DoubleSubscriber>* item = static_cast<ItemAndSubscriber<nt::DoubleSubscriber>*>(maps_.value(index));
			delete item;
		}
		break;
	case nt::NetworkTableType::kDoubleArray:
		{
			ItemAndSubscriber<nt::DoubleArraySubscriber>* item = static_cast<ItemAndSubscriber<nt::DoubleArraySubscriber>*>(maps_.value(index));
			delete item;
		}
		break;
	case nt::NetworkTableType::kFloat:
		{
			ItemAndSubscriber<nt::FloatSubscriber>* item = static_cast<ItemAndSubscriber<nt::FloatSubscriber>*>(maps_.value(index));
			delete item;
		}
		break;
	case nt::NetworkTableType::kFloatArray:
		{
			ItemAndSubscriber<nt::FloatArraySubscriber>* item = static_cast<ItemAndSubscriber<nt::FloatArraySubscriber>*>(maps_.value(index));
			delete item;
		}
		break;
	case nt::NetworkTableType::kInteger:
		{
			ItemAndSubscriber<nt::IntegerSubscriber>* item = static_cast<ItemAndSubscriber<nt::IntegerSubscriber>*>(maps_.value(index));
			delete item;
		}
		break;
	case nt::NetworkTableType::kIntegerArray:
		{
			ItemAndSubscriber<nt::IntegerArraySubscriber>* item = static_cast<ItemAndSubscriber<nt::IntegerArraySubscriber>*>(maps_.value(index));
			delete item;
		}
		break;
	case nt::NetworkTableType::kString:
		{
			ItemAndSubscriber<nt::StringSubscriber>* item = static_cast<ItemAndSubscriber<nt::StringSubscriber>*>(maps_.value(index));
			delete item;
		}
		break;
	case nt::NetworkTableType::kStringArray:
		{
			ItemAndSubscriber<nt::StringArraySubscriber>* item = static_cast<ItemAndSubscriber<nt::StringArraySubscriber>*>(maps_.value(index));
			delete item;
		}
		break;
	}

	maps_.remove(index);
}

void ValueDisplayWidget::updateItem(QTreeWidgetItem* item)
{
	int n = item->data(1, Qt::UserRole).toInt();
	if (maps_.contains(n)) {
		QString node = item->data(0, Qt::UserRole).toString();
		item->setText(1, NTValueFormatting::getValueString(inst_, node));
	}
}

void ValueDisplayWidget::descendItem(QTreeWidgetItem* item)
{
	for (int i = 0; i < item->childCount(); i++) {
		QTreeWidgetItem* child = item->child(i);
		if (child->childCount() == 0) {
			updateItem(child);
		}
		else {
			descendItem(child);
		}
	}
}

void ValueDisplayWidget::updateNodes()
{
	for (int i = 0; i < topLevelItemCount(); i++) {
		QTreeWidgetItem* child = topLevelItem(i);
		if (child->childCount() == 0) {
			updateItem(child);
		}
		else {
			descendItem(child);
		}
	}
}