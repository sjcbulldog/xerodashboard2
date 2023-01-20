#pragma once

#include <QtWidgets/QLineEdit>

class TabEditName : public QLineEdit
{
	Q_OBJECT

public:
	TabEditName(QWidget *parent = Q_NULLPTR);
	~TabEditName();

signals:
	void escapePressed();

protected:
	virtual void keyPressEvent(QKeyEvent* ev) override;

private:
	void emitEscapePressed();

private:
};
