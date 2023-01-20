#pragma once

#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/qvector.h>

class PlotData
{
public:
	PlotData(const QString& name, const QStringList &columns, const QVector<QVector<double>> &data) {
		name_ = name;
		columns_ = columns;
		data_ = data;
		normalizeData();
	}

	const QString& name() {
		return name_;
	}

	const QStringList& columns() {
		return columns_;
	}

	double get(int row, int col) {
		return (data_[col])[row];
	}

	const QVector<double> &getCol(int col) {
		return data_[col];
	}

private:
	void normalizeData();

private:
	QString name_;
	QStringList columns_;
	QVector<QVector<double>> data_;
};

