#include "Plot.h"
#include "PlotData.h"
#include <QtCore/QDebug>

Plot::Plot(nt::NetworkTableInstance &inst, const QString &basename, const QString& name) : inst_(inst) {
	name_ = name;
	plot_base_name_ = basename;
	complete_ = false;
	version_ = -1;

	QString topicname = plot_base_name_ + name + "/columns";
	auto column_topic = inst_.GetStringArrayTopic(topicname.toStdString());
	column_subscriber_ = column_topic.Subscribe({});

	topicname = plot_base_name_ + name + "/complete";
	auto complete_topic = inst_.GetBooleanTopic(topicname.toStdString());
	complete_subscriber_ = complete_topic.Subscribe(false);

	topicname = plot_base_name_ + name + "/count";
	auto count_topic = inst_.GetIntegerTopic(topicname.toStdString());
	count_subscriber_ = count_topic.Subscribe(-1);

	topicname = plot_base_name_ + name + "/points";
	auto points_topic = inst_.GetIntegerTopic(topicname.toStdString());
	points_subscriber_ = points_topic.Subscribe(-1);

	topicname = plot_base_name_ + name + "/version";
	auto version_topic = inst_.GetIntegerTopic(topicname.toStdString());
	version_subscriber_ = version_topic.Subscribe(-1);
}

std::shared_ptr<PlotData> Plot::getPlotData()
{
	lock_.lock();
	std::shared_ptr<PlotData> pd = std::make_shared<PlotData>(name_, columns_, data_);
	lock_.unlock();

	return pd;
}

int Plot::dataPoints()
{
	lock_.lock();
	int ret = dataPointsNoLock();
	lock_.unlock();
	return ret;
}

int Plot::dataPointsNoLock()
{
	int minval = std::numeric_limits<int>::max();

	if (version_ == 3) {
		if (data_.size() == 0) {
			minval = 0;
		}
		else {
			minval = data_[0].size();
		}
	}
	else {
		for (int i = 0; i < data_.size(); i++) {
			if (data_[i].size() < minval)
				minval = data_[i].size();
		}
	}
	return minval;
}


void Plot::reset()
{
	columns_.clear();

	for (int i = 0; i < data_subscribers_.size(); i++) {
		delete data_subscribers_[i];
	}
	data_subscribers_.clear();
	data_.clear();
}

void Plot::initColumns(const std::vector<std::string>& colstrs)
{
	QVector<double> empty;
	data_.clear();

	columns_.clear();
	for (const std::string& str : colstrs) {
		columns_.push_back(QString::fromStdString(str));
		data_.push_back(empty);
	}
}

bool Plot::compareCols(const std::vector<std::string>& colstrs)
{
	if (columns_.size() != colstrs.size())
		return false;

	for (int i = 0; i < columns_.size(); i++) {
		if (columns_[i].toStdString() != colstrs[i]) {
			return false;
		}
	}

	return true;
}

bool Plot::readV3Data()
{
	bool newData = false;

	int count = points_subscriber_.Get();
	int currentSize = dataPointsNoLock();
	if (count > currentSize) {
		for (int row = currentSize; row < count; row++) {
			QString topicName = plot_base_name_ + name_ + "/data/" + QString::number(row);
			nt::DoubleArrayTopic topic = inst_.GetDoubleArrayTopic(topicName.toStdString());
			auto sub = topic.Subscribe({});
			std::vector<double> value = sub.Get();
			if (value.size() == 0) {
				break;
			}

			if (value.size() != columns_.size()) {
				QString msg = "in plot " + name_ + " data row " + QString::number(row) + "there were " +
					QString::number(value.size()) + " entries, which is not valid. There should be " + QString::number(columns_.size()) + ".";
				emit logMessage(msg);
				break;
			}

			newData = true;
			for (int col = 0; col < value.size(); col++) {
				data_[col].push_back(value[col]);
			}
		}
	}

	return newData;
}

bool Plot::readV4Data()
{
	nt::DoubleSubscriber ds;

	if (columns_.size() > 0 && data_subscribers_.size() == 0) {
		data_subscribers_.resize(columns_.size());
		for (int i = 0; i < columns_.size(); i++) {
			QString topicname = plot_base_name_ + name_ + "/data/" + QString::number(i);
			auto topic = inst_.GetDoubleTopic(topicname.toStdString());
			nt::PubSubOptions options;
			options.structSize = sizeof(options);
			options.disableLocal = false;
			options.disableRemote = false;
			options.excludePublisher = true;
			options.excludeSelf = false;
			options.keepDuplicates = true;
			options.periodic = 0.02;
			options.pollStorage = 32;
			options.sendAll = true;
			options.prefixMatch = false;
			options.topicsOnly = false;
			data_subscribers_[i] = new nt::DoubleSubscriber();
			*(data_subscribers_[i]) = topic.Subscribe(0.0, options);
		}
	}

	bool received = false;
	for (int i = 0; i < data_subscribers_.size(); i++) {
		std::vector<nt::TimestampedDouble> data = data_subscribers_[i]->ReadQueue();
		for (int j = 0; j < data.size(); j++) {
			received = true;
			if (i < data_.size()) {
				data_[i].push_back(data[j].value);
			}
		}
	}


	return received;
}

void Plot::queryData()
{
	bool received = false;
	lock_.lock();

	if (version_ == -1) {
		int v = version_subscriber_.Get();
		if (v != -1) {
			version_ = v;
		}
		else {
			int pts = points_subscriber_.Get();
			if (pts != -1) {
				//
				// This is an old robot program that does not publish the version.  If we see data
				// start to show up, assume it is the original, NT3 based plot system.
				//
				version_ = 3;
			}
		}
	}

	std::vector<std::string> cols = column_subscriber_.Get();

	if (cols.size() > 0 && columns_.size() == 0) {
		initColumns(cols);
	}
	else if (cols.size() > 0)
	{
		if (!compareCols(cols)) {
			reset();
			initColumns(cols);
			emit restarted(name_);
		}
	}

	if (version_ == 3) {
		received = readV3Data();
	}
	else if (version_ == 4) {
		received = readV4Data();
	}

	bool isComplete = complete_subscriber_.Get();
	if (complete_ == false && isComplete == true) {
		//
		// We just completed, emit a signal
		//
		emit complete(name_);
	}
	else if (complete_ == true && isComplete == false) {
		//
		// We just reset emit a signal
		//
		reset();
		emit restarted(name_);
	}
	complete_ = isComplete;
	lock_.unlock();

	if (received) {
		if (received) {
			QString msg = "counts:";
			for (int i = 0; i < data_.size(); i++) {
				msg += " " + QString::number(data_[i].size());
			}
			qDebug() << msg;
		}

		emit dataArrived(name_);
	}
}