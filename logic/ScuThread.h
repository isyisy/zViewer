#ifndef SCUTHREAD_H
#define SCUTHREAD_H

#include <QThread>


class ScuThread : public QThread
{
	Q_OBJECT

public:
	ScuThread(QObject *parent=0);
	~ScuThread();
	
private:
	
};

#endif // SCUTHREAD_H
