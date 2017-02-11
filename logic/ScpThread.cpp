#include "ScpThread.h"

ScpThread::ScpThread(QObject *parent)
	: QThread(parent)
{

}

ScpThread::~ScpThread()
{

}

void ScpThread::SetScpHandle(DcmStorageSCP* scp)
{
	pStoreScp = scp;
}

void ScpThread::run()
{
	if (pStoreScp)
		pStoreScp->listen();
}