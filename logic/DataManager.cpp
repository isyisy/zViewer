#include "DataManager.h"
#include "dcmtk\dcmnet\diutil.h"
#include <QDebug>
#include <QByteArray>
#include "logic/Configuration.h"

DataManager* DataManager::m_pDataManager = NULL;

DataManager::DataManager()
	:m_Dataset(new DcmDataset)
{

}

DataManager::~DataManager()
{

}

DataManager* DataManager::instance()
{
	if (NULL == m_pDataManager)
		m_pDataManager = new DataManager;
	return m_pDataManager;
}

/************************************************************************/
/* pacs                                                                */
/************************************************************************/
void DataManager::QueryStudyResultsHandler(OFList<DcmDataset*> DcmResultDataList)
{
	clearStudyData();

	OFString patientID, patientName, patientSex, patientBirth;
	OFString studyInstanceUID, studyDate, accessionNumber, studyDescription, studyExamBodyPart;
	OFIterator<DcmDataset*> it = DcmResultDataList.begin();
	for (it; it != DcmResultDataList.end(); ++it)
	{
		(*it)->findAndGetOFString(DCM_StudyDate, studyDate);
		QString date_str(studyDate.c_str());
		if (date_str.compare(m_date_begin)<0 || date_str.compare(m_date_end)>0)
			continue;

		(*it)->findAndGetOFString(DCM_PatientID, patientID);
		(*it)->findAndGetOFString(DCM_PatientName, patientName);

		(*it)->findAndGetOFString(DCM_PatientSex, patientSex);
		(*it)->findAndGetOFString(DCM_PatientBirthDate, patientBirth);
		
		(*it)->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID);
		(*it)->findAndGetOFString(DCM_AccessionNumber, accessionNumber);
		
		(*it)->findAndGetOFString(DCM_StudyDescription, studyDescription);
		(*it)->findAndGetOFString(DCM_BodyPartExamined, studyExamBodyPart);

		Patient* pInfo = new Patient;
		pInfo->patient_id = QString::fromLocal8Bit(patientID.c_str());
		pInfo->patient_name = QString::fromLocal8Bit(patientName.c_str());
		pInfo->patient_sex = QString::fromLocal8Bit(patientSex.c_str());
		pInfo->patient_birthday = QString(patientBirth.c_str());
		m_PatientInfoMap.insert(pInfo->patient_id, pInfo);

		Study* sInfo = new Study;
		sInfo->study_instance_uid = QString(studyInstanceUID.c_str());
		sInfo->study_accession_number = QString(accessionNumber.c_str());
		sInfo->study_date = QString(studyDate.c_str());
		sInfo->study_description = QString::fromLocal8Bit(studyDescription.c_str());
		sInfo->study_bodypart = QString::fromLocal8Bit(studyExamBodyPart.c_str());
		m_StudyInfoMap.insert(sInfo->study_instance_uid, sInfo);

		m_PatientStudyMMap.insert(pInfo->patient_id, sInfo->study_instance_uid);
	}
	emit PatientInfo(m_PatientInfoMap);
}

void DataManager::QuerySeriesResultsHandler(OFList<DcmDataset*> results)
{
	clearSeriesData();
	OFString studyInstanceUID, seriesInstanceUID, seriesNumber, seriesModality;
	OFString seriesDescription, seriesImageDirection;
	OFIterator<DcmDataset*> it = results.begin();
	for (it; it != results.end(); ++it)
	{
		(*it)->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID);
		(*it)->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID);
		(*it)->findAndGetOFString(DCM_SeriesNumber, seriesNumber);
		(*it)->findAndGetOFString(DCM_Modality, seriesModality);
		(*it)->findAndGetOFString(DCM_SeriesDescription, seriesDescription);
		(*it)->findAndGetOFString(DCM_ImageOrientationPatient, seriesImageDirection);

		Series* sInfo = new Series;
		sInfo->series_instance_uid = QString(seriesInstanceUID.c_str());
		sInfo->series_number = QString(seriesNumber.c_str());
		sInfo->series_modality = QString(seriesModality.c_str());
		sInfo->series_description = QString::fromLocal8Bit(seriesDescription.c_str());
		sInfo->series_image_orientation = QString::fromLocal8Bit(seriesImageDirection.c_str());

		m_SeriesInfoMap.insert(sInfo->series_instance_uid, sInfo);
		m_StudySeriesMMap.insert(QString(studyInstanceUID.c_str()), sInfo->series_instance_uid);
	}
	emit SeriesInfo(m_SeriesInfoMap);
}

void DataManager::search_patient(QString date0,QString date1)
{
	m_date_begin = date0;
	m_date_end = date1;
	emit BeginQuery(UID_FINDStudyRootQueryRetrieveInformationModel, "STUDY");
}

void DataManager::search_study(QString patient_id)
{
	QList<QString> id_list = m_PatientStudyMMap.values(patient_id);
	QList<Study*> study_list;
	QList<QString>::iterator it = id_list.begin();
	for (it; it != id_list.end(); ++it)
	{
		study_list.push_back(m_StudyInfoMap[(*it)]);
	}
	emit StudyInfo(study_list);
}

void DataManager::search_series(QString study_uid)
{
	emit BeginQuery(UID_FINDStudyRootQueryRetrieveInformationModel, "SERIES");
}

void DataManager::download_series(QString patient_id, QString study_id, QString series_id)
{
	m_Dataset->clear();
	QByteArray patientId = patient_id.toLocal8Bit();
	QByteArray studyInstance = study_id.toLocal8Bit();
	QByteArray seriesInstance = series_id.toLocal8Bit();
	m_Dataset->putAndInsertString(DCM_QueryRetrieveLevel, "SERIES");
	m_Dataset->putAndInsertString(DCM_PatientID, patientId.data());
	m_Dataset->putAndInsertString(DCM_StudyInstanceUID, studyInstance.data());
	m_Dataset->putAndInsertString(DCM_SeriesInstanceUID, seriesInstance.data());

	QString ae_title = Configuration::instance()->get_title();
	OFString title = OFString(ae_title.toLocal8Bit().data());
	emit BeginDownload(title, UID_MOVEStudyRootQueryRetrieveInformationModel, m_Dataset);
}

void DataManager::upload_image(QString path)
{
	m_Dataset->clear();

	OFCondition cond = fileformat.loadFile(path.toLocal8Bit().data());
	m_Dataset = fileformat.getDataset();
	//m_Dataset = fileformat.getDataset();
	if (cond.bad())
	{
		//do something
	}

	emit BeginUpload(UID_CTImageStorage, m_Dataset);
	if (cond.bad())
	{
		//do something
		return;
	}
}

/************************************************************************/
/* local database                                                      */
/************************************************************************/
bool DataManager::isDatabaseConnect()
{
	if (QSqlDatabase::contains("VETSQL"))
		m_db = QSqlDatabase::database("VETSQL");
	else
		m_db = QSqlDatabase::addDatabase("QSQLITE", "VETSQL");
	m_db.setDatabaseName("./localdb.db3");
	if (m_db.open())
		return true;
	else
		return false;
}

void DataManager::save_to_database(Patient* p0, Study* p1, Series* p2)
{
	QString pid = p0->patient_id;
	QString pname = p0->patient_name;
	QString psex = p0->patient_sex;
	QString pbirth = p0->patient_birthday;

	QString suid = p1->study_instance_uid;
	QString saccession = p1->study_accession_number;
	QString sdate = p1->study_date;
	QString sdescrip = p1->study_description;
	QString sbodypart = p1->study_bodypart;

	QString uid = p2->series_instance_uid;
	QString seriesnum = p2->series_number;
	QString modality = p2->series_modality;
	QString descrip = p2->series_description;
	QString orient = p2->series_image_orientation;
	QString count = p2->series_image_number;

	bool result = insert_patient_table(pid,pname,psex,pbirth);
	result = insert_study_table(suid, saccession, sdate, sdescrip, sbodypart, pid);
	result = insert_series_table(uid, seriesnum, modality, descrip, orient, count, suid);
}

bool DataManager::insert_patient_table(QString id, QString name, QString sex, QString birth)
{
	if (!isDatabaseConnect()) return false;
	QSqlQuery query(m_db);
	bool result = query.prepare("insert into Patient('Pid','Pname','Psex','Pbirth')values(?,?,?,?);");
	query.bindValue(0, id);
	query.bindValue(1, name);
	query.bindValue(2, sex);
	query.bindValue(3, birth);
	result = query.exec();
	if (result)
		return true;
	else
	{
		//do something
		return false;
	}
}

bool DataManager::insert_study_table(QString uid, QString accnum, QString date, QString desc, QString body, QString foreignKey)
{
	if (!isDatabaseConnect()) return false;
	QSqlQuery query(m_db);
	bool result = query.prepare("insert into Study('Suid','SaccessNumber','Sdate','Sdiscrip','Sbodypart','Spatientid')values(?,?,?,?,?,?);");
	query.bindValue(0, uid);
	query.bindValue(1, accnum);
	query.bindValue(2, date);
	query.bindValue(3, desc);
	query.bindValue(4, body);
	query.bindValue(5, foreignKey);
	result = query.exec();
	if (result)
		return true;
	else
	{
		//do something
		return false;
	}
}

bool DataManager::insert_series_table(QString uid, QString sernum, QString modality, QString desc, QString orient, QString count, QString foreignKey)
{
	if (!isDatabaseConnect()) return false;
	QSqlQuery query(m_db);
	bool result = query.prepare("insert into Series('Suid','SseriesNumber','Smodality','Sdiscrip','Sorient','Scount','Sstudyid')values(?,?,?,?,?,?,?);");
	query.bindValue(0, uid);
	query.bindValue(1, sernum);
	query.bindValue(2, modality);
	query.bindValue(3, desc);
	query.bindValue(4, orient);
	query.bindValue(5, count);
	query.bindValue(6, foreignKey);
	result = query.exec();
	if (result)
		return true;
	else
	{
		//do something
		return false;
	}
}

void DataManager::query_patient(QString modality, QString date0, QString date1, QString tag, QString value)
{
	//modality暂时不考虑
	if (!isDatabaseConnect())
	{
		//do something
		return;
	}
	QSqlQuery query(m_db);
	bool result = true;
	m_DataBaseResultsList.clear();

	QString value_str = "*";
	value_str = (QString)"%" + value + "%";
	if (tag == "PatientId")
		result = query.prepare(QString("select * from Patient,Study where Patient.Pid=Study.Spatientid and Study.Spatientid like '%1' and Study.Sdate >= '%2' and Study.Sdate <= '%3';").arg(value_str).arg(date0).arg(date1));
	else if (tag=="PatientName")
		result = query.prepare(QString("select * from Patient,Study where Patient.Pid=Study.Spatientid and Patient.Pname like '%1' and Study.Sdate >= '%2' and Study.Sdate <= '%3';").arg(value_str).arg(date0).arg(date1));
	else if (tag=="AccessionNumber")
		result = query.prepare(QString("select * from Patient,Study where Patient.Pid=Study.Spatientid and Study.SaccessNumber like '%1' and Study.Sdate >= '%2' and Study.Sdate <= '%3';").arg(value_str).arg(date0).arg(date1));
	else if (tag=="StudyDescription")
		result = query.prepare(QString("select * from Patient,Study where Patient.Pid=Study.Spatientid and Study.Sdiscrip like '%1' and Study.Sdate >= '%2' and Study.Sdate <= '%3';").arg(value_str).arg(date0).arg(date1));

	result = query.exec();
	QStringList patientList;

	while (query.next())
	{
		QString patientId = query.value(0).toString();
		QString patientName = query.value(1).toString();
		QString patientSex = query.value(2).toString();
		QString patientBirth = query.value(3).toString();

		QString StudyUid = query.value(4).toString();
		QString AccessionNumber = query.value(5).toString();
		QString StudyDate = query.value(6).toString();
		QString Description = query.value(7).toString();
		QString BodyPart = query.value(8).toString();

		patientList << patientId << patientName << patientSex << patientBirth<<StudyUid<<AccessionNumber<<StudyDate<<Description<<BodyPart;
		m_DataBaseResultsList.push_back(patientList);
		patientList.clear();
	}
	emit PatientInfo(m_DataBaseResultsList);
}

void DataManager::query_study(QString pid)
{
	m_StudyResultsList.clear();

	//此处不再查询数据库，直接查询上一步中得到的List
	QList<QStringList>::iterator it = m_DataBaseResultsList.begin();
	for (it; it != m_DataBaseResultsList.end(); ++it)
	{
		QStringList list_str = (*it);
		if (list_str.at(0) != pid)
			continue;
		QStringList study_list;
		study_list << list_str.at(4) << list_str.at(5) << list_str.at(6) << list_str.at(7) << list_str.at(8);
		m_StudyResultsList.push_back(study_list);
		study_list.clear();
	}
	emit StudyInfo(m_StudyResultsList);
}

void DataManager::query_series(QString suid)
{
	//modality暂时不考虑
	if (!isDatabaseConnect())
	{
		//do something
		return;
	}
	QSqlQuery query(m_db);
	bool result = true;
	m_SeriesResultsList.clear();
	result = query.prepare(QString("select * from Series where Series.Sstudyid = '%1';").arg(suid));
	result = query.exec();

	QStringList series;
	while (query.next())
	{
		QString SeriesUid = query.value(0).toString();
		QString SeriesNumber = query.value(1).toString();
		QString Modality = query.value(2).toString();
		QString Description = query.value(3).toString();
		//QString Orientation = query.value(4).toString();
		series << SeriesUid << SeriesNumber << Modality << Description;
		m_SeriesResultsList.push_back(series);
		series.clear();
	}
	emit SeriesInfo(m_SeriesResultsList);
}

/************************************************************************/
/*clean                                                                */
/************************************************************************/
void DataManager::clearStudyData()
{
	QMap<QString, Patient*>::iterator it = m_PatientInfoMap.begin();
	for (it; it != m_PatientInfoMap.end(); ++it)
	{
		Patient* pp = (*it);
		delete pp;
	}
	m_PatientInfoMap.clear();
	QMap<QString, Study*>::iterator it1 = m_StudyInfoMap.begin();
	for (it1; it1 != m_StudyInfoMap.end(); ++it1)
	{
		Study* ps = (*it1);
		delete ps;
	}
	m_StudyInfoMap.clear();
	m_PatientStudyMMap.clear();
}

void DataManager::clearSeriesData()
{
	QMap<QString, Series*>::iterator it = m_SeriesInfoMap.begin();
	for (it; it != m_SeriesInfoMap.end(); ++it)
	{
		Series* ps = (*it);
		delete ps;
	}
	m_SeriesInfoMap.clear();
	m_StudySeriesMMap.clear();
}