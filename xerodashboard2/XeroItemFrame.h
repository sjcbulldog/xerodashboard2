#pragma once

#include <QtWidgets/QWidget>
#include "TabEditName.h"

class XeroItemFrame : public QWidget
{
	Q_OBJECT

public:
	XeroItemFrame(QWidget *parent = Q_NULLPTR);
	~XeroItemFrame();

	void setTitle(const QString& title) {
		title_ = title;
	}

	const QString& title() const {
		return title_;
	}

	void setWidget(QWidget* child) {
		child_ = child;
		child_->setParent(this);
		child_->setVisible(true);
		layout();
	}

	void layout();

	void setSelected(bool b);

	bool isMaximized() const {
		return is_maximized_;
	}

	void maximize(bool storeprev);
	void restore();

	bool isPlot() const;
	bool isNetworkTableEntry() const;

	QWidget* child() {
		return child_;
	}

	void setSavePos(const QPoint& pos) {
		pos_ = pos;
	}

	const QPoint& getSavePos() {
		return pos_;
	}

signals:
	void headerClicked(XeroItemFrame * which, bool shift);
	void frameClosing();
	void startDragWindows(const QPoint& global);
	void continueDragWindows(const QPoint& global);

protected:
	void paintEvent(QPaintEvent* ev);
	void mousePressEvent(QMouseEvent* ev);
	void mouseMoveEvent(QMouseEvent* ev);
	void mouseReleaseEvent(QMouseEvent* ev);
	void mouseDoubleClickEvent(QMouseEvent* ev);
	void resizeEvent(QResizeEvent* ev);
	void changeEvent(QEvent* ev);
	void enterEvent(QEvent* ev);
	void leaveEvent(QEvent* ev);
	void closeEvent(QCloseEvent* ev);

private:
	QRect closeBoxRect();
	QRect maxBoxRect();
	QRect headerRect();
	void checkCursor();
	void checkButtons(const QPoint &pt);

	void titleEditInputRejected();
	void titleEditEditingFinished();

private:
	static constexpr const int BorderThickness = 1;

	enum class ResizeCursor
	{
		None,
		Right,
		Left,
		Bottom,
		BottomRight,
		BottomLeft
	};

private:
	QString title_;
	QWidget* child_;
	int header_height_;
	bool dragging_;
	ResizeCursor resize_cursor_;
	ResizeCursor resizing_;
	bool close_highlighted_;
	bool max_highlighted_;
	bool is_maximized_;
	QRect prev_location_;
	QPoint mouse_;
	QPoint window_;
	QSize winsize_;
	QPoint winpos_;

	TabEditName* editor_;

	QColor header_color_;
	QColor hilite_color_;

	QPoint pos_;
};
