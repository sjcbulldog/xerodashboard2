#pragma once

#include <QtCore/QVector>

class MovingAverage
{
public:
	MovingAverage(int count) {
		count_ = count;
	}

	void reset() {
		values_.clear();
	}

	void addValue(double v) {
		values_.push_back(v);
		while (values_.size() > count_)
		{
			values_.removeAt(0);
		}
	}

	double average() const {
		return std::accumulate(values_.begin(), values_.end(), 0) / static_cast<double>(values_.size());
	}

private:
	QVector<double> values_;
	int count_;
};

