#include "PlotData.h"


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
