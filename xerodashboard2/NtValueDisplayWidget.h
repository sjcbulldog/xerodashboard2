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
	NTValueDisplayWidget(const QString &node, QWidget * parent = Q_NULLPTR);
	~NTValueDisplayWidget();

	QJsonObject getJSONDesc() const;

	void setDisplayType(const QString& type) {
		display_type_ = type;
		update();
	}

	void updateData(const nt::Value& value) {
		value_ = value;
		update();
	}

	const QString& node() const {
		return node_;
	}

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
	nt::Value value_;

	QString node_;
	bool active_;
};
