#pragma once

#include <QtCore/QString>
#include <networktables/NetworkTableInstance.h>

class NTValueFormatting
{
public:
	static QString getValueString(nt::NetworkTableInstance& inst, const QString& node);
	static QString getValueString(nt::Value value);

private:
	static QString booleanArray(std::span<const int> value);
	static QString doubleArray(std::span<const double> value);
	static QString floatArray(std::span<const float> value);
	static QString integerArray(std::span<const int64_t> value);
	static QString stringArray(std::span<const std::string> value);
};

