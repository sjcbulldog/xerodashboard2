#pragma once

#include <QtCore/QJsonOBject>
#include <QtWidgets/QWidget>
#include <networktables/NetworkTableInstance.h>
#include <memory>

class NetworkTableManager;

class NTValueDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	NTValueDisplayWidget(nt::NetworkTableInstance& inst, const QString &node, QWidget * parent = Q_NULLPTR);
	~NTValueDisplayWidget();

	QJsonObject getJSONDesc() const;

	void setDisplayType(const QString& type) {
		display_type_ = type;
		update();
	}

	void updateData();

protected:
	void paintEvent(QPaintEvent* ev) override;
	bool event(QEvent* ev) override;

private:

	void drawContentsBoolean(QPainter& p);
	void drawContentsText(QPainter& p);
	void drawContentsBar(QPainter& p);

	void customMenuRequested(const QPoint& pos);

	void displayAs(QString value);

private:
	QString display_type_;
	NT_Type data_type_;

	QString node_;
	nt::NetworkTableInstance& inst_;
	bool active_;
};
