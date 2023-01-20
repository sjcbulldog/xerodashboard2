#include "TabEditName.h"
#include <QtGui/QKeyEvent>

TabEditName::TabEditName(QWidget *parent) : QLineEdit(parent)
{
}

TabEditName::~TabEditName()
{
}

void TabEditName::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key_Escape)
	{
		emitEscapePressed();
		return;
	}

	QLineEdit::keyPressEvent(ev);
}

void TabEditName::emitEscapePressed()
{
	emit escapePressed();
}