#pragma once

#include <QtCore/QString>
#include <networktables/NetworkTableInstance.h>

class NTValueFormatting
{
public:
	static QString getValueString(nt::NetworkTableInstance& inst, const QString& node);

private:
	static QString booleanArray(nt::NetworkTableInstance& inst, const QString& node);
	static QString doubleArray(nt::NetworkTableInstance& inst, const QString& node);
	static QString floatArray(nt::NetworkTableInstance& inst, const QString& node);
	static QString integerArray(nt::NetworkTableInstance& inst, const QString& node);
	static QString stringArray(nt::NetworkTableInstance& inst, const QString& node);
};

