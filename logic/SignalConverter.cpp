#include "SignalConverter.h"

SignalConverter* SignalConverter::m_instance = NULL;

SignalConverter::SignalConverter(QObject *parent)
	: QObject(parent)
{

}

SignalConverter::~SignalConverter()
{

}
