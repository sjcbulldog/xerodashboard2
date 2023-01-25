#include "PlotData.h"
#include "MovingAverage.h"
#include "LinearRegression.h"
#include <QtCore/QDebug>

void PlotData::normalizeData()
{
	int minval = std::numeric_limits<int>::max();

	for (int i = 0; i < data_.size(); i++) {
		if (data_[i].size() < minval) {
			minval = data_[i].size();
		}
	}

	for (int i = 0; i < data_.size(); i++) {
		if (data_[i].size() != minval) {
			data_[i].resize(minval);
		}
	}
}

void PlotData::getMinMax(int col, double& minv, double& maxv)
{
	minv = std::numeric_limits<double>::max();
	maxv = std::numeric_limits<double>::min();

	QVector<double>& rowdata = data_[col];
	for (int row = 0; row < rowdata.size(); row++) {
		if (rowdata[row] < minv) {
			minv = rowdata[row];
		}

		if (rowdata[row] > maxv) {
			maxv = rowdata[row];
		}
	}
}

int PlotData::getDir(int col, int index)
{
	int dir;

	if (data_[col][index] < data_[col][index + 1]) {
		dir = 1;
	}
	else {
		dir = -1;
	}

	return dir;
}

PlotData::LevelAverage PlotData::makeLevelAverage(int col, int st, int en)
{
	LevelAverage avg;
	double sum = 0.0;

	avg.minv = data_[col][st];
	avg.maxv = data_[col][st];

	for (int i = st; i <= en; i++) {
		double v = data_[col][i];
		if (v > avg.maxv) {
			avg.maxv = v;
		}

		if (v < avg.minv) {
			avg.minv = v;
		}

		sum += v;
	}

	avg.average = sum / static_cast<double>(en - st + 1);
	avg.end = data_[0][en];
	avg.endIndex = en;
	avg.start = data_[0][st];
	avg.startIndex = st;

	return avg;
}

QPair<int, int> PlotData::trim(int col, int st, int en)
{
	double minv = data_[col][st];
	double maxv = data_[col][st];

	for (int i = st; i <= en; i++)
	{
		double v = data_[col][i];
		if (v < minv) {
			minv = v;
		}

		if (v > maxv) {
			maxv = v;
		}
	}

	return QPair<int, int>(st, en);
}

QVector<PlotData::LevelAverage> PlotData::getAverages(int col)
{
	int st = 0;
	int index = 1;
	const double avgthreshold = 0.05;
	const double valuethreshold = 0.10;
	double minv, maxv;
	double range;
	double sum;
	double last;
	QVector<QPair<int, int>> regions;
	QVector<LevelAverage> result;

	if (data_[0].size() == 0) {
		return result;
	}

	getMinMax(col, minv, maxv);
	range = maxv - minv;
	sum = data_[col][0];
	last = data_[col][0];

	while (index < data_[col].size()) 
	{
		double value = data_[col][index];
		double avgdiff = std::abs(value - sum / static_cast<double>(index - st));
		double valuediff = std::abs(value - last);
		if (valuediff > range * valuethreshold && avgdiff > range * avgthreshold)
		{
			if (index - st > 2) {
				regions.push_back(QPair<int, int>(st, index - 1));
			}

			st = index;
			sum = value;
		}
		else
		{
			sum += value;
		}

		index++;
		last = value;
	}

	if (index - st > 2) {
		regions.push_back(QPair<int, int>(st, index - 1));
	}

	for (int i = 0; i < regions.size(); i++) {
		QPair<int, int> trimmed = trim(col, regions[i].first, regions[i].second);
		result.push_back(makeLevelAverage(col, trimmed.first, trimmed.second));
	}

	return result;
}
