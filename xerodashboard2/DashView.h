#pragma once

#include <QtCore/QJsonArray>
#include <QtWidgets/QWidget>
#include <networktables/NetworkTableInstance.h>
#include "XeroItemFrame.h"
#include "PlotManager.h"

class NetworkTableManager;
class PlotMgr;
class XeroItemFrame;
class GameFieldManager;

class DashView : public QWidget
{
	Q_OBJECT

public:
	DashView(nt::NetworkTableInstance& inst, PlotManager &pmgr, QWidget *parent = Q_NULLPTR);
	~DashView();

	void updateNode(const QString& node, const nt::Value &value);

	int tileMargin() const {
		return tile_margin_;
	}

	void setTileMargin(int m) {
		tile_margin_ = m;
	}

	QJsonArray getJSONDesc();
	void restoreLayout(const QJsonArray &arr);

	void alignLeft();
	void alignRight();
	void alignTop();
	void alignBottom();
	void alignHCenter();
	void alignVCentor();
	void alignTileH();
	void alignTileV();

	void sizeWidth();
	void sizeHeight();
	void sizeBoth();

	void addTab();
	void closeTab();

	void showPath();

	int selectedCount() const {
		return selected_.count();
	}

	XeroItemFrame* selectedWidget(int index) {
		return selected_.at(index);
	}

	bool isPlotSelected() const {
		if (selected_.count() != 1)
			return false;

		return selected_.at(0)->isPlot();
	}

	int count() const;

signals:
	void selectedCountChanged();

protected:
	void paintEvent(QPaintEvent* ev) override;
	void dragEnterEvent(QDragEnterEvent* ev) override;
	void dropEvent(QDropEvent* ev) override;
	void mousePressEvent(QMouseEvent* ev) override;
	void mouseReleaseEvent(QMouseEvent* ev) override;
	void mouseMoveEvent(QMouseEvent* ev) override;
	void resizeEvent(QResizeEvent* ev) override;

private:
	void createNTWidget(const QJsonObject& obj);
	void createPlot(const QJsonObject& obj);
	void frameWindowHeaderClicked(XeroItemFrame*, bool shift);
	void removeAllFrames();

	void addToSelectedSet(XeroItemFrame* frame);
	void removeFromSelectedSet(XeroItemFrame* frame);
	void clearSelectedSet();

	XeroItemFrame* createNewFrame();
	void frameClosing(XeroItemFrame* frame);

	void dragWindowsStart(const QPoint& global);
	void dragWindowsContinue(const QPoint& global);

private:
	nt::NetworkTableInstance& inst_;
	QList<XeroItemFrame*> selected_;
	int tile_margin_;

	bool selecting_;
	QPoint other_;
	QPoint mouse_;

	QPoint drag_mouse_start_;

	PlotManager& plot_mgr_;
};
