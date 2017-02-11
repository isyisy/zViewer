#ifndef CCDATALOADER_H
#define CCDATALOADER_H

#include "globals/CCGlobalStructs.h"
#include "logic/CCDicomUtils.h"
#include <QObject>
#include <QList>
#include <QStringList>
#include <QUrl>

class CCDataLoader : public QObject
{
	Q_OBJECT

public:
	CCDataLoader();
	~CCDataLoader();

signals:
	void NewDicomUtils(CCDicomUtils*);
	void UpdateLoadProgress(QString name, int r0, int r1, int pos);
	void FinishLoading(QString name);

public slots:
	void loadDataFromUrls(QList<QUrl> urls);
	void loadDataFromUrls2(QList<QString>);
private:

	void OpenDicomFile(const char*);
	void OpenDicomDir(const char*);
	void importImages(QStringList);

	bool dicom_check(const char*);
};

#endif // CCDATALOADER_H
