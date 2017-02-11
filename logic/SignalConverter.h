#ifndef SIGNALCONVERTER_H
#define SIGNALCONVERTER_H

#include <QObject>

class CustomLabel;

class SignalConverter : public QObject
{
	Q_OBJECT

public:
	~SignalConverter();
	static SignalConverter* instance(){
		if (m_instance == NULL)
			m_instance = new SignalConverter;
		return m_instance;
	}

signals:
	void PostIndex(QString, CustomLabel*);
	void GetImage(QString, CustomLabel*);

private:
	SignalConverter(QObject *parent = 0);
private:
	static SignalConverter* m_instance;
};

#endif // SIGNALCONVERTER_H
