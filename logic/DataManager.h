#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "globals/CCGlobalStructs.h"
#include "dcmtk\dcmdata\dcdatset.h"
#include "dcmtk\dcmdata\dcdeftag.h"
#include "dcmtk\dcmdata\dcfilefo.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include <QMultiMap>

#include <QSqlDatabase>
#include <QSqlQuery>

class DataManager : public QObject
{
	Q_OBJECT

public:
	static DataManager* instance();
	~DataManager();
signals:
	//pacs
	void PatientInfo(QMap<QString,Patient*>);
	void StudyInfo(QList<Study*>);
	void SeriesInfo(QMap<QString,Series*>);

	void BeginQuery(const OFString &sopClass, const char*, DcmDataset *dataset=0);
	void BeginDownload(const OFString&, const OFString&, DcmDataset* dataset=0);
	void BeginUpload(const OFString&, DcmDataset* dataset =0);
	//local database
	void PatientInfo(QList<QStringList>);
	void StudyInfo(QList<QStringList>);
	void SeriesInfo(QList<QStringList>);

public slots:
	//response server
	void QueryStudyResultsHandler(OFList<DcmDataset*>);
	void QuerySeriesResultsHandler(OFList<DcmDataset*>);
	//pacs search
	void search_patient(QString,QString);
	void search_study(QString);
	void search_series(QString);

	void download_series(QString, QString, QString);
	void save_to_database(Patient*,Study*,Series*);
	void upload_image(QString);//传入路径
	//local database search
	void query_patient(QString,QString,QString,QString,QString);
	void query_study(QString);
	void query_series(QString);

private:
	DataManager();
	void clearStudyData();
	void clearSeriesData();

	bool isDatabaseConnect();
	bool insert_patient_table(QString, QString, QString, QString);
	bool insert_study_table(QString,QString,QString,QString,QString,QString foreignKey);
	bool insert_series_table(QString, QString, QString, QString, QString, QString, QString foreignKey);

private:
	QMap<QString, Patient*> m_PatientInfoMap;
	QMap<QString, Study*> m_StudyInfoMap;
	QMap<QString, Series*> m_SeriesInfoMap;
	QMultiMap<QString, QString> m_PatientStudyMMap;
	QMultiMap<QString, QString> m_StudySeriesMMap;

	static DataManager* m_pDataManager;
	DcmDataset *m_Dataset;//作为信息传递使用

	///////////////local database///////////////
	QSqlDatabase m_db;
	QSqlQuery *m_dbQuery;
	QList<QStringList> m_DataBaseResultsList;//Patient查询本地得到的Patient和Study表的连接
	QList<QStringList> m_StudyResultsList;//Study查询m_DataBaseResultsList得到的study信息
	QList<QStringList> m_SeriesResultsList;//Series本地查询得到的表
	DcmFileFormat fileformat;//不要局部

	QString m_date_begin;
	QString m_date_end;
};

#endif // DATAMANAGER_H
