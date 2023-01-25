#include "EditableTabWidget.h"

EditableTabWidget::EditableTabWidget(QWidget *parent) : QTabWidget(parent)
{
	setTabsClosable(true);
	setMovable(true);
}

EditableTabWidget::~EditableTabWidget()
{
}

void EditableTabWidget::mouseDoubleClickEvent(QMouseEvent* ev)
{
}
