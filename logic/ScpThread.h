#ifndef SCPTHREAD_H
#define SCPTHREAD_H

#include <QThread>
#include "dcmtk/dcmnet/dstorscp.h"

class ScpThread : public QThread
{
	Q_OBJECT

public:
	ScpThread(QObject *parent=0);
	~ScpThread();
	void SetScpHandle(DcmStorageSCP* scp);

protected:
	void run();

private:
	DcmStorageSCP* pStoreScp;
};

#endif // SCPTHREAD_H
