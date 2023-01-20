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

QString NTValueFormatting::getValueString(nt::NetworkTableInstance &inst, const QString& node)
{
	QString ret;
	nt::Topic topic = inst.GetTopic(node.toStdString());

	switch (topic.GetType())
	{
	case nt::NetworkTableType::kBoolean:
		ret = (inst.GetBooleanTopic(node.toStdString()).Subscribe(false).Get() ? "true " : "false");
		break;

	case nt::NetworkTableType::kBooleanArray:
		ret = booleanArray(inst, node);
		break;

	case nt::NetworkTableType::kDouble:
		ret = QString::number(inst.GetDoubleTopic(node.toStdString()).Subscribe(0.0).Get());
		break;

	case nt::NetworkTableType::kDoubleArray:
		ret = doubleArray(inst, node);
		break;

	case nt::NetworkTableType::kFloat:
		ret = QString::number(inst.GetFloatTopic(node.toStdString()).Subscribe(0.0).Get());
		break;

	case nt::NetworkTableType::kFloatArray:
		ret = floatArray(inst, node);
		break;

	case nt::NetworkTableType::kInteger:
		ret = QString::number(inst.GetIntegerTopic(node.toStdString()).Subscribe(0.0).Get());
		break;

	case nt::NetworkTableType::kIntegerArray:
		ret = integerArray(inst, node);
		break;

	case nt::NetworkTableType::kString:
		ret = QString::fromStdString(inst.GetStringTopic(node.toStdString()).Subscribe("<NULL>").Get());
		break;

	case nt::NetworkTableType::kStringArray:
		ret = stringArray(inst, node);
		break;
	}

	return ret;
}

QString NTValueFormatting::booleanArray(nt::NetworkTableInstance& inst, const QString& node)
{
	QString ret;

	auto value = inst.GetBooleanArrayTopic(node.toStdString()).Subscribe({}).Get();
	for (int i = 0; i < value.size(); i++) {
		if (i != 0) {
			ret += ", ";
		}

		ret += value[i] ? "true " : "false";
	}

	return ret;
}

QString NTValueFormatting::doubleArray(nt::NetworkTableInstance& inst, const QString& node)
{
	QString ret;

	auto value = inst.GetDoubleArrayTopic(node.toStdString()).Subscribe({}).Get();
	for (int i = 0; i < value.size(); i++) {
		if (i != 0) {
			ret += ", ";
		}

		ret += QString::number(value[i]);
	}

	return ret;
}

QString NTValueFormatting::floatArray(nt::NetworkTableInstance& inst, const QString& node)
{
	QString ret;

	auto value = inst.GetFloatArrayTopic(node.toStdString()).Subscribe({}).Get();
	for (int i = 0; i < value.size(); i++) {
		if (i != 0) {
			ret += ", ";
		}

		ret += QString::number(value[i]);
	}

	return ret;
}

QString NTValueFormatting::integerArray(nt::NetworkTableInstance& inst, const QString& node)
{
	QString ret;

	auto value = inst.GetIntegerArrayTopic(node.toStdString()).Subscribe({}).Get();
	for (int i = 0; i < value.size(); i++) {
		if (i != 0) {
			ret += ", ";
		}

		ret += QString::number(value[i]);
	}

	return ret;
}

QString NTValueFormatting::stringArray(nt::NetworkTableInstance& inst, const QString& node)
{
	QString ret;

	auto value = inst.GetStringArrayTopic(node.toStdString()).Subscribe({}).Get();
	for (int i = 0; i < value.size(); i++) {
		if (i != 0) {
			ret += ", ";
		}

		ret += QString::fromStdString(value[i]);
	}

	return ret;
}