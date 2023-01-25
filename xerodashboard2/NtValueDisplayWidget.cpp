#include "NTValueDisplayWidget.h"
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtWidgets/QToolTip>
#include <QtWidgets/QMenu>
#include <QtGui/QHelpEvent>
#include <QtGui/QPainter>
#include "JsonFieldNames.h"
#include "NTValueFormatting.h"

NTValueDisplayWidget::NTValueDisplayWidget(const QString& node, QWidget *parent): QWidget(parent)
{
	(void)connect(this, &QWidget::customContextMenuRequested, this, &NTValueDisplayWidget::customMenuRequested);

	setMinimumSize(40, 10);
	setContextMenuPolicy(Qt::CustomContextMenu);

	display_type_ = "text";

	node_ = node;
	active_ = true;
}

NTValueDisplayWidget::~NTValueDisplayWidget()
{
}

void NTValueDisplayWidget::displayAs(QString v)
{
	display_type_ = v;
}

void NTValueDisplayWidget::customMenuRequested(const QPoint& pos)
{
	QMenu menu;

	QAction textDisplay("Display As Text", this);
	auto fn = std::bind(&NTValueDisplayWidget::displayAs, this, QString("text"));
	connect(&textDisplay, &QAction::triggered, fn);
	menu.addAction(&textDisplay);

	QAction colorDisplay("Display As Color", this);
	fn = std::bind(&NTValueDisplayWidget::displayAs, this, QString("color"));
	connect(&colorDisplay, &QAction::triggered, fn);
	menu.addAction(&colorDisplay);

	QAction barDisplay("Display As Bar", this);
	fn = std::bind(&NTValueDisplayWidget::displayAs, this, QString("bar"));
	connect(&barDisplay, &QAction::triggered, fn);
	menu.addAction(&barDisplay);

	menu.exec(mapToGlobal(pos));
	update();
}

bool NTValueDisplayWidget::event(QEvent* ev)
{
	if (ev->type() == QEvent::ToolTip)
	{
		QHelpEvent* hev = static_cast<QHelpEvent*>(ev);
		QToolTip::showText(hev->globalPos(), node_);
	}
	return QWidget::event(ev);
}

QJsonObject NTValueDisplayWidget::getJSONDesc() const
{
	QJsonObject obj;

	obj[JsonFieldNames::Display] = display_type_;
	obj[JsonFieldNames::Path] = node_;
	obj[JsonFieldNames::X] = pos().x();
	obj[JsonFieldNames::Y] = pos().y();
	obj[JsonFieldNames::Width] = width();
	obj[JsonFieldNames::Height] = height();

	return obj;
}

void NTValueDisplayWidget::paintEvent(QPaintEvent* ev)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	QBrush back(QColor(255, 255, 255));
	p.setBrush(back);
	p.drawRect(0, 0, width(), height());

	if (value_.type() == NT_Type::NT_BOOLEAN)
	{
		if (display_type_ == "color")
			drawContentsBoolean(p);
		else
			drawContentsText(p);
	}
	else if (value_.type() == NT_FLOAT || value_.type() == NT_DOUBLE || value_.type() == NT_INTEGER)
	{
		if (display_type_ == "bar")
			drawContentsBar(p);
		else
			drawContentsText(p);
	}
	else
	{
		drawContentsText(p);
	}
}

void NTValueDisplayWidget::drawContentsBoolean(QPainter& p)
{
	QColor color;
	if (!active_)
	{
		color = QColor(128, 128, 128);
	}
	else if (NTValueFormatting::getValueString(value_) == "true")
	{
		color = QColor(0, 255, 0);
	}
	else
	{
		color = QColor(255, 0, 0);
	}

	QBrush brush(color);
	p.setBrush(brush);
	p.drawRect(0, 0, width(), height());
}

void NTValueDisplayWidget::drawContentsBar(QPainter& p)
{
}

void NTValueDisplayWidget::drawContentsText(QPainter &p)
{
	QFontMetricsF fm(p.font());

	QColor txtcolor;

	if (active_)
		txtcolor = QColor(0, 0, 0);
	else
		txtcolor = QColor(128, 128, 128);

	QString txt = NTValueFormatting::getValueString(value_);
	QPoint pt(width() / 2 - fm.horizontalAdvance(txt) / 2, height() / 2 - fm.height() / 2 + fm.ascent());
	p.setPen(QPen(txtcolor));
	p.drawText(pt, txt);
}
