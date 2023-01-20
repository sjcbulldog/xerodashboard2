#include "XeroItemFrame.h"
#include "NTValueDisplayWidget.h"
// #include "PlotWidget.h"
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QFontMetrics>
#include <QtGui/QMouseEvent>

XeroItemFrame::XeroItemFrame(QWidget *parent) : QWidget(parent)
{
	setMinimumSize(10, 10);
	child_ = nullptr;
	dragging_ = false;
	resizing_ = ResizeCursor::None;
	resize_cursor_ = ResizeCursor::None;

	QFontMetrics fm(font());
	header_height_ = fm.height() + BorderThickness * 2 ;

	setMouseTracking(true);

	close_highlighted_ = false;
	max_highlighted_ = false;
	is_maximized_ = false;

	header_color_ = QColor(135, 206, 250);
	hilite_color_ = QColor(51, 102, 255);
}

XeroItemFrame::~XeroItemFrame()
{
	if (child_ != nullptr)
		delete child_;
}

//bool XeroItemFrame::isPlot() const
//{
//	return dynamic_cast<PlotWidget*>(child_) != nullptr;
//}

bool XeroItemFrame::isNetworkTableEntry() const
{
	return dynamic_cast<NTValueDisplayWidget*>(child_) != nullptr;
}

void XeroItemFrame::closeEvent(QCloseEvent* ev)
{
	emit frameClosing();
	QWidget::closeEvent(ev);
}

void XeroItemFrame::titleEditInputRejected()
{
	editor_->deleteLater();
	editor_ = nullptr;
}

void XeroItemFrame::titleEditEditingFinished()
{
	title_ = editor_->text();
	editor_->close();
	editor_->deleteLater();
	editor_ = nullptr;
	update();
}

void XeroItemFrame::mouseDoubleClickEvent(QMouseEvent* ev)
{
	if (!headerRect().contains(ev->pos()))
		return;

	editor_ = new TabEditName(this);
	editor_->setGeometry(headerRect());
	editor_->setText(title_);
	editor_->setVisible(true);
	editor_->setEnabled(true);
	editor_->setFocus();
	editor_->setSelection(0, title_.length());

	(void)connect(editor_, &QLineEdit::returnPressed, this, &XeroItemFrame::titleEditEditingFinished);
	(void)connect(editor_, &TabEditName::escapePressed, this, &XeroItemFrame::titleEditInputRejected);
}

void XeroItemFrame::setSelected(bool b)
{
	if (b)
		header_color_ = QColor(0, 191, 255);
	else
		header_color_ = QColor(135, 206, 250);

	update();
}

void XeroItemFrame::layout()
{
	if (child_ == nullptr)
		return;

	int w = width();
	int h = height();

	if (w < child_->minimumWidth() + BorderThickness * 2)
		w = child_->minimumWidth() + BorderThickness * 2;

	if (h < child_->minimumHeight() + BorderThickness * 4 + header_height_)
		h = child_->minimumHeight() + BorderThickness * 4 + header_height_;

	if (w != width() || h != height())
	{
		setGeometry(pos().x(), pos().y(), w, h);

		//
		// This will trigger a resize event which will recursively call back into this method but with a valid width and
		// height so that the else clause below will execute
		//
	}
	else
	{
		child_->setGeometry(BorderThickness, header_height_ + 3 * BorderThickness, width() - BorderThickness * 2, height() - header_height_ - 4 * BorderThickness);
	}
}

QRect XeroItemFrame::closeBoxRect()
{
	QRect r(width() - BorderThickness - header_height_, BorderThickness, header_height_, header_height_);
	return r;
}

QRect XeroItemFrame::maxBoxRect()
{
	int left = width() - BorderThickness - header_height_ - BorderThickness - header_height_;
	QRect r(left, BorderThickness, header_height_, header_height_);
	return r;
}

QRect XeroItemFrame::headerRect()
{
	QRect r(0, 0, width(), header_height_ + 2 * BorderThickness);
	return r;
}

void XeroItemFrame::changeEvent(QEvent* ev)
{
	QWidget::changeEvent(ev);

	if (ev->type() == QEvent::FontChange)
	{
		QFontMetrics fm(font());
		header_height_ = fm.height() + BorderThickness * 2;
		layout();
	}
}

void XeroItemFrame::resizeEvent(QResizeEvent* ev)
{
	if (child_ != nullptr)
	{
		layout();
	}
}

void XeroItemFrame::maximize(bool storeprev)
{
	if (storeprev)
		prev_location_ = geometry();

	setGeometry(0, 0, parentWidget()->width(), parentWidget()->height());
	is_maximized_ = true;
}

void XeroItemFrame::restore()
{
	is_maximized_ = false;
	setGeometry(prev_location_);
}

void XeroItemFrame::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		QRect r = closeBoxRect();
		if (r.contains(ev->pos()))
		{
			if (child_ != nullptr)
				child_->close();
			close();
			deleteLater();
			return;
		}

		r = maxBoxRect();
		if (r.contains(ev->pos()))
		{
			if (is_maximized_)
			{
				restore();
			}
			else
			{
				maximize(true);
			}
			return;
		}

		r = headerRect();
		if (r.contains(ev->pos()))
		{
			bool shift = false;
			if ((ev->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier)
				shift = true;

			emit headerClicked(this, shift);

			dragging_ = true;
			emit startDragWindows(ev->globalPos());
			return;
		}

		if (resize_cursor_ != ResizeCursor::None)
		{
			resizing_ = resize_cursor_;
			mouse_ = ev->globalPos();
			winsize_ = size();
			winpos_ = pos();
			return;
		}
	}
}

void XeroItemFrame::paintEvent(QPaintEvent* ev)
{
	QRect r = maxBoxRect();

	QPainter p(this);
	QFontMetricsF fm(p.font());

	p.setPen(QPen(QColor(0, 0, 0)));
	p.drawRect(0, 0, width(), height());

	p.setBrush(QBrush(header_color_));
	p.drawRect(0, 0, width(), header_height_ + BorderThickness * 2);

	int x = width() / 2 - fm.horizontalAdvance(title_) / 2;
	if (x + fm.horizontalAdvance(title_) > r.left())
		x = BorderThickness;
	QPoint pt(x, BorderThickness + fm.ascent());
	p.drawText(pt, title_);

	// Draw the max button
	if (max_highlighted_)
	{
		p.setBrush(QBrush(hilite_color_));
		p.drawRect(r);
	}
	p.setBrush(Qt::BrushStyle::NoBrush);
	r.adjust(4, 4, -4, -4);
	p.drawRect(r);
	if (is_maximized_)
	{
		r.adjust(-2, 2, -2, 2);
		p.drawRect(r);
	}

	// Draw the close button
	r = closeBoxRect();
	if (close_highlighted_)
	{
		p.setBrush(QBrush(hilite_color_));
		p.drawRect(r);
	}
	p.setBrush(Qt::BrushStyle::NoBrush);
	r.adjust(2, 2, -2, -2);

	p.drawLine(r.topLeft(), r.bottomRight());
	p.drawLine(r.topRight(), r.bottomLeft());
}

void XeroItemFrame::mouseMoveEvent(QMouseEvent* ev)
{
	if (dragging_)
	{
		emit continueDragWindows(ev->globalPos());
	}
	else if (resizing_ == ResizeCursor::BottomRight)
	{
		QPoint dist = ev->globalPos() - mouse_;
		int w = winsize_.width() + dist.x();
		int h = winsize_.height() + dist.y();
		setGeometry(pos().x(), pos().y(), w, h);
	}
	else if (resizing_ == ResizeCursor::BottomLeft)
	{
		QPoint dist = ev->globalPos() - mouse_;
		int w = winsize_.width() - dist.x();
		int h = winsize_.height() + dist.y();
		setGeometry(winpos_.x() + dist.x(), pos().y(), w, h);
	}
	else if (resizing_ == ResizeCursor::Right)
	{
		QPoint dist = ev->globalPos() - mouse_;
		int w = winsize_.width() + dist.x();
		setGeometry(pos().x(), pos().y(), w, winsize_.height());
	}
	else if (resizing_ == ResizeCursor::Left)
	{
		QPoint dist = ev->globalPos() - mouse_;
		int w = winsize_.width() - dist.x();
		setGeometry(winpos_.x() + dist.x(), pos().y(), w, winsize_.height());

		qDebug() << dist;
	}
	else if (resizing_ == ResizeCursor::Bottom)
	{
		QPoint dist = ev->globalPos() - mouse_;
		int h = winsize_.height() + dist.y();
		setGeometry(pos().x(), pos().y(), winsize_.width(), h);
	}
	else
	{
		checkCursor();
		checkButtons(ev->pos());
	}
}

void XeroItemFrame::checkButtons(const QPoint &pt)
{
	bool newvalue = false;

	QRect r = closeBoxRect();
	if (r.contains(pt))
	{
		newvalue = true;
	}
	else
	{
		newvalue = false;
	}

	if (newvalue != close_highlighted_)
	{
		close_highlighted_ = newvalue;
		update();
	}

	r = maxBoxRect();
	if (r.contains(pt))
	{
		newvalue = true;
	}
	else
	{
		newvalue = false;
	}

	if (newvalue != max_highlighted_)
	{
		max_highlighted_ = newvalue;
		update();
	}
}

void XeroItemFrame::checkCursor()
{
	QPoint gpos = QCursor::pos();
	QPoint wpos = mapFromGlobal(gpos);
	QPoint brdiff = wpos - QPoint(width(), height());
	QPoint bldiff = wpos - QPoint(0, height());

	if (std::abs(brdiff.x()) < 8 && std::abs(brdiff.y()) < 8)
	{
		setCursor(Qt::SizeFDiagCursor);
		resize_cursor_ = ResizeCursor::BottomRight;
		grabMouse();
	}
	else if (std::abs(bldiff.x()) < 8 && std::abs(bldiff.y()) < 8)
	{
		setCursor(Qt::SizeBDiagCursor);
		resize_cursor_ = ResizeCursor::BottomLeft;
		grabMouse();
	}
	else if (std::abs(brdiff.x()) < 8)
	{
		setCursor(Qt::SizeHorCursor);
		resize_cursor_ = ResizeCursor::Right;
		grabMouse();
	}
	else if (std::abs(bldiff.x()) < 8)
	{
		setCursor(Qt::SizeHorCursor);
		resize_cursor_ = ResizeCursor::Left;
		grabMouse();
	}
	else if (std::abs(brdiff.y()) < 8)
	{
		setCursor(Qt::SizeVerCursor);
		resize_cursor_ = ResizeCursor::Bottom; 
		grabMouse();
	}
	else
	{
		releaseMouse();
		setCursor(Qt::ArrowCursor);
		resize_cursor_ = ResizeCursor::None;
	}
}

void XeroItemFrame::mouseReleaseEvent(QMouseEvent* ev)
{
	dragging_ = false;
	resizing_ = ResizeCursor::None;;
}

void XeroItemFrame::enterEvent(QEvent* ev)
{
	checkCursor();
}

void XeroItemFrame::leaveEvent(QEvent* ev)
{
	setCursor(Qt::ArrowCursor);
	resize_cursor_ = ResizeCursor::None;;
}
