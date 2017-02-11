#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QList>
#include <QUrl>
#include <QMap>
#include <QMultiMap>
#include <QProgressDialog>
#include <QProgressBar>

#include "ui/CCTabButtonWidget.h"
#include "globals/CCGlobalStructs.h"
#include "ui/CCDicomInfoDlg.h"
#include "ui/PacsSetDlg.h"
#include "logic/FileWatcher.h"

#include "logic/Scp.h"
#include "logic/Scu.h"
#include "logic/ScpThread.h"
#include "logic/ScuThread.h"
#include "logic/DataManager.h"

#include <QTimer>

class DataWidget : public QWidget
{
	Q_OBJECT
public:
	enum Modality
	{
		ALL=0,
		CR, //1
		DR, //2
		CT, //3
		MR, //4
		XA, //5
		US, //6
		MG, //7
		PT  //8
	};
	enum SearchTag
	{
		PatientName = 0,//0
		PatientId,//1
		AccessionNumber,//2
		StudyDescription,//3
		DoctorName,//4
		Instritution//5
	};
	enum SearchMode//查询模式，
	{
		Local_Import = 0,
		Local_Search,
		Pacs_Search
	};
public:
	DataWidget(QWidget *parent);
	~DataWidget();
signals:
	void GotoOtherTab(int index);//跳转到其他Tab页
	//下面的两个函数，Qurl未解决中文路径问题，故中文路径导入使用QString函数版，但对于拖拽无效
	void ImportImages2View(QList<QUrl>);
	void ImportImages2View2(QList<QString>);
	//pacs
	void SearchPatient(QString, QString);//传入参数是起止date
	void SearchStudy(QString patientid);
	void SearchSeries(QString studyuid);

	void DownloadSeries(QString,QString,QString);
	void UploadImage(QString);
	void InsertDatabase(Patient*,Study*,Series*);
	//local database
	void QueryPatient(QString, QString, QString, QString, QString);
	void QueryStudy(QString);
	void QuerySeries(QString);

	void FinishConvertDcm2Pdf();
	

protected:
	void resizeEvent(QResizeEvent* event);
public slots:
	void open_dicomfile();
	void open_dicomdirs();

	void queryDateChanged();//查询日期的变化

	void upload_pacs();
	void upload_dir_to_pacs(QString);
	void search_local();
	void search_pacs();
	void search_pacs_today();
	void set_pacs();
	void showStudy(QTableWidgetItem*);
	void showSeries(QTableWidgetItem*);
	void SeriesClickOnce(QTableWidgetItem*);
	void download_series(QTableWidgetItem*);

	void importImages();
	void OpenReport();
	//pacs
	void process_patient_table(QMap<QString, Patient*>);
	void process_study_table(QList<Study*>);
	void process_series_table(QMap<QString, Series*>);
	//local database
	void process_patient_table(QList<QStringList>);
	void process_study_table(QList<QStringList>);
	void process_series_table(QList<QStringList>);

	void FinishDownload();
	void BeginUploadImage(QString);
	void UpdateUploadProgress(unsigned long);

	void restoreState();
	void error_handle(ErrorCode);
private:
	void init_window();
	void init_pacs();
	void init_database();
	void convertjpg2dcm(QString filePath);
	void OpenDicomFile(QString filePath);
	void OpenImageFile(QString filePath);
	void get_query_contidion();

	bool check_dicom(const char*);
private:

	QList<Patient*> m_PatientList;//所有病人信息
	QString m_DataPath;//文件或者文件夹路径
	CCTabButtonWidget* tabButton;

	QGroupBox* m_LocalImportGrp;
	QGroupBox* m_SearchGroup;
	QGroupBox* m_ResultsGroup;

	QPushButton* m_LocalSearchBtn;
	QPushButton* m_PacsSearchBtn;
	QPushButton* m_PacsSetBtn;

	QComboBox* m_model;
	QComboBox* m_date;
	QDateEdit* m_date_edit1;
	QDateEdit* m_date_edit2;

	QComboBox* m_tag;
	QLineEdit* m_tagValue;

	QLineEdit* m_FilePath;
	QPushButton* open_file_btn;
	QPushButton* open_dir_btn;
	QPushButton* upload_file_btn;

	QTextEdit* mFileInfoText;

	QTableWidget* m_PatientTree;
	QTableWidget* m_SeriesTree;
	QTableWidget* m_StudyTree;

	QPushButton* m_OpenReportBtn;
	QPushButton* m_Import2ViewBtn;

	Series *series;
	Series* pCurrentSeries;
	Study *study;
	Study* pCurrentStudy;
	Patient *patient;
	Patient* pCurrentPatient;


	//*pacs related*//
	CCScp* m_pScp;
	CCScu* m_pScu;
	ScuThread* m_ScuThread;
	ScpThread* m_ScpThread;
	DataManager* m_DataMagager;

	SearchMode m_CurrentSearchMode;
	QString m_CurrentPatientId;
	QString m_CurrentStudyUid;
	QString m_CurrentSeriesUid;

	//////////////查询条件、不合理，临时
	QString condition_modality;
	QString condition_date_begin;
	QString condition_date_end;
	QString condition_tag;//自定义选择的tag
	QString condition_value;//对应上面的tag

	//
	FileWatcher* m_Wathcer;
	QProgressDialog* progressDlg;
	QProgressBar* m_ProgressBar;

	bool m_upload_flag;
	unsigned long m_upload_size;

	QTimer *m_workTimer;
};

#endif // DATAWIDGET_H
