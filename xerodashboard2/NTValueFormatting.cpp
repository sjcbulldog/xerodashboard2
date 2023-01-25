#include "NTValueFormatting.h"
#include <QtCore/QDebug>
#include <networktables/BooleanTopic.h>
#include <networktables/BooleanArrayTopic.h>
#include <networktables/DoubleTopic.h>
#include <networktables/DoubleArrayTopic.h>
#include <networktables/FloatTopic.h>
#include <networktables/FloatArrayTopic.h>
#include <networktables/IntegerTopic.h>
#include <networktables/IntegerArrayTopic.h>
#include <networktables/StringTopic.h>
#include <networktables/StringArrayTopic.h>
#include <networktables/GenericEntry.h>

QString NTValueFormatting::getValueString(nt::Value value)
{
	QString ret;

	switch (value.type()) {
	case NT_Type::NT_BOOLEAN:
		ret = value.GetBoolean() ? "true" : "false";
		break;

	case NT_Type::NT_BOOLEAN_ARRAY:
		ret = booleanArray(value.GetBooleanArray());
		break;

	case NT_Type::NT_DOUBLE:
		ret = QString::number(value.GetDouble());
		break;

	case NT_Type::NT_DOUBLE_ARRAY:
		ret = doubleArray(value.GetDoubleArray());
		break;

	case NT_Type::NT_FLOAT:
		ret = QString::number(value.GetFloat());
		break;

	case NT_Type::NT_FLOAT_ARRAY:
		ret = floatArray(value.GetFloatArray());
		break;

	case NT_Type::NT_INTEGER:
		ret = QString::number(value.GetInteger());
		break;

	case NT_Type::NT_INTEGER_ARRAY:
		ret = integerArray(value.GetIntegerArray());
		break;

	case NT_Type::NT_STRING:
		ret = QString::fromStdString(std::string(value.GetString()));
		break;

	case NT_Type::NT_STRING_ARRAY:
		ret = stringArray(value.GetStringArray());
		break;
	}

	return ret;
}

QString NTValueFormatting::getValueString(nt::NetworkTableInstance& inst, const QString& node)
{
	QString ret;
	nt::Topic topic = inst.GetTopic(node.toStdString());
	return getValueString(topic.GenericSubscribe().Get());
}

QString NTValueFormatting::booleanArray(std::span<const int> value)
{
	QString ret;

	ret += "[";
	for (int i = 0; i < value.size(); i++) {
		if (i != 0) {
			ret += ", ";
		}

		ret += value[i] ? "true " : "false";
	}
	ret += "]";

	return ret;
}

QString NTValueFormatting::doubleArray(std::span<const double> value)
{
	QString ret;

	ret += "[";
	for (int i = 0; i < value.size(); i++) {
		if (i != 0) {
			ret += ", ";
		}

		ret += QString::number(value[i]);
	}
	ret += "]";

	return ret;
}

QString NTValueFormatting::floatArray(std::span<const float> value)
{
	QString ret;

	ret += "[";
	for (int i = 0; i < value.size(); i++) {
		if (i != 0) {
			ret += ", ";
		}

		ret += QString::number(value[i]);
	}
	ret += "]";

	return ret;
}

QString NTValueFormatting::integerArray(std::span<const int64_t> value)
{
	QString ret;

	ret += "[";
	for (int i = 0; i < value.size(); i++) {
		if (i != 0) {
			ret += ", ";
		}

		ret += QString::number(value[i]);
	}
	ret += "]";

	return ret;
}

QString NTValueFormatting::stringArray(std::span<const std::string> value)
{
	QString ret;

	ret += "[";
	for (int i = 0; i < value.size(); i++) {
		if (i != 0) {
			ret += ", ";
		}

		ret += QString::fromStdString(value[i]);
	}
	ret += "]";

	return ret;
}