#pragma once

#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/qvector.h>

class PlotData
{
public:
	struct LevelAverage
	{
		int startIndex;
		double start;
		int endIndex;
		double end;
		double average;
		double minv;
		double maxv;

		LevelAverage() {
			startIndex = -1;
			endIndex = -1;
			start = 0.0;
			end = 0.0;
			average = 0.0;
			minv = 0.0;
			maxv = 0.0;
		}

		bool isValid() const {
			return startIndex != -1 && endIndex != -1;
		}
	};

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

	void getMinMax(int col, double& minv, double& maxv);
	QVector<LevelAverage> getAverages(int col);

private:
	void normalizeData();
	int getDir(int col, int index);
	QPair<int, int> trim(int col, int st, int en);
	LevelAverage makeLevelAverage(int col, int st, int end);

private:
	QString name_;
	QStringList columns_;
	QVector<QVector<double>> data_;
};

