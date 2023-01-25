#pragma once

#include <QtWidgets/QTabWidget>

class EditableTabWidget : public QTabWidget
{
	Q_OBJECT

public:
	EditableTabWidget(QWidget *parent = Q_NULLPTR);
	~EditableTabWidget();

protected:
	virtual void mouseDoubleClickEvent(QMouseEvent* ev) override;

private:
};
