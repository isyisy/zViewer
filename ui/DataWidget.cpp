#include "DataWidget.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QDebug>
#include <QProcess>
#include <QMessageBox>

#include "logic/CCDicomUtils.h"
#include "logic/Img2DicomUtils.h"
#include "logic/Configuration.h"


DataWidget::DataWidget(QWidget *parent)
	: QWidget(parent)
	, m_DataPath("Data")
	, m_CurrentSearchMode(SearchMode::Local_Import)
	, m_CurrentPatientId("")
	, m_CurrentStudyUid("")
	, m_CurrentSeriesUid("")
	, condition_modality("*")
	, condition_date_begin("19700101")
	, condition_date_end("21151115")
	, condition_tag("PatientName")
	, condition_value("")
	, pCurrentPatient(NULL)
	, pCurrentStudy(NULL)
	, pCurrentSeries(NULL)
	, m_upload_flag(false)
{
	series = new Series;
	study = new Study;
	patient = new Patient;

	m_DataMagager = DataManager::instance();

	init_window();
	init_pacs();
	init_database();

	m_workTimer = new QTimer(this);
	connect(m_workTimer, SIGNAL(timeout()), this, SLOT(restoreState()));
}

DataWidget::~DataWidget()
{
	m_ScpThread->quit();
	m_ScuThread->quit();
	if (m_pScp)
		delete m_pScp;
	if (m_pScu)
		delete m_pScu;
	delete series;
	delete study;
	delete patient;
}

void DataWidget::init_database()
{
	connect(this, SIGNAL(QueryPatient(QString, QString, QString, QString, QString)), m_DataMagager, SLOT(query_patient(QString, QString, QString, QString, QString)));
	connect(this, SIGNAL(QueryStudy(QString)), m_DataMagager, SLOT(query_study(QString)));
	connect(this, SIGNAL(QuerySeries(QString)), m_DataMagager, SLOT(query_series(QString)));
	connect(m_DataMagager, SIGNAL(PatientInfo(QList<QStringList>)), this, SLOT(process_patient_table(QList<QStringList>)));
	connect(m_DataMagager, SIGNAL(StudyInfo(QList<QStringList>)), this, SLOT(process_study_table(QList<QStringList>)));
	connect(m_DataMagager, SIGNAL(SeriesInfo(QList<QStringList>)), this, SLOT(process_series_table(QList<QStringList>)));
}

void DataWidget::init_pacs()
{
	qRegisterMetaType <OFString>("OFString");
	qRegisterMetaType <OFList<DcmDataset*>>("OFList<DcmDataset*>");
	qRegisterMetaType <DcmDataset*>("DcmDataset*");
	qRegisterMetaType <ErrorCode>("ErrorCode");

	m_Wathcer = FileWatcher::instance();
	Configuration::instance()->set_upload_dir("./Upload");
	Configuration::instance()->set_download_dir("./Data");
	m_Wathcer->set_path(Configuration::instance()->get_upload_dir());
	connect(m_Wathcer, SIGNAL(NewFileFound(QString)), this, SIGNAL(UploadImage(QString)));
	connect(m_Wathcer, SIGNAL(NewDirFound(QString)), this, SLOT(upload_dir_to_pacs(QString)));

	QString title = Configuration::instance()->get_title();
	unsigned short port = Configuration::instance()->get_listening_port().toUShort();
	PacsServer ps("","","","");

	QList<PacsServer> list = Configuration::instance()->get_pacsinfo();//默认第一个
	if (list.size() > 0)
		ps = list.at(0);
	m_pScp = new CCScp;
	m_pScp->init_association_parameter(OFString(title.toLocal8Bit().data()),port);

	m_pScu = new CCScu;
	m_pScu->init_association_parameter(title.toLocal8Bit().data(), ps.ae_title.toLocal8Bit().data(),
				ps.ip_address.toLocal8Bit().data(), ps.port.toUShort());

	m_ScpThread = new ScpThread;
	m_ScuThread = new ScuThread;
	m_ScpThread->SetScpHandle(m_pScp);
	m_pScp->moveToThread((QThread*)m_ScpThread);
	m_pScu->moveToThread((QThread*)m_ScuThread);
	connect(m_DataMagager, SIGNAL(BeginQuery(const OFString&, const char*, DcmDataset*)), m_pScu, SLOT(QueryPacs(const OFString&, const char*, DcmDataset*)));
	connect(m_pScu, SIGNAL(StudyListResults(OFList<DcmDataset*>)), m_DataMagager, SLOT(QueryStudyResultsHandler(OFList<DcmDataset*>)));
	connect(m_pScu, SIGNAL(SeriesListResults(OFList<DcmDataset*>)), m_DataMagager, SLOT(QuerySeriesResultsHandler(OFList<DcmDataset*>)));
	connect(this, SIGNAL(SearchPatient(QString,QString)), m_DataMagager, SLOT(search_patient(QString,QString)));
	connect(this, SIGNAL(SearchStudy(QString)), m_DataMagager, SLOT(search_study(QString)));
	connect(this, SIGNAL(SearchSeries(QString)), m_DataMagager, SLOT(search_series(QString)));
	connect(m_DataMagager, SIGNAL(PatientInfo(QMap<QString, Patient*>)),this, SLOT(process_patient_table(QMap<QString, Patient*>)));
	connect(m_DataMagager, SIGNAL(StudyInfo(QList<Study*>)), this, SLOT(process_study_table(QList<Study*>)));
	connect(m_DataMagager, SIGNAL(SeriesInfo(QMap<QString, Series*>)), this, SLOT(process_series_table(QMap<QString, Series*>)));
	connect(this, SIGNAL(DownloadSeries(QString, QString, QString)), m_DataMagager, SLOT(download_series(QString, QString, QString)));
	connect(m_DataMagager, SIGNAL(BeginDownload(const OFString&, const OFString&, DcmDataset*)), m_pScu, SLOT(MovePacs(const OFString&, const OFString&, DcmDataset*)));
	connect(this, SIGNAL(UploadImage(QString)), m_DataMagager, SLOT(upload_image(QString)));
	connect(m_DataMagager, SIGNAL(BeginUpload(const OFString&, DcmDataset*)), m_pScu, SLOT(StorePacs(const OFString&, DcmDataset*)));
	connect(this, SIGNAL(InsertDatabase(Patient*, Study*, Series*)), m_DataMagager, SLOT(save_to_database(Patient*, Study*, Series*)));

	connect(m_pScp, SIGNAL(OneInstanceStored()), this, SLOT(FinishDownload()));
	connect(m_pScu, SIGNAL(CurrentByteSended(unsigned long)), this, SLOT(UpdateUploadProgress(unsigned long)));

	connect(m_pScu, SIGNAL(ErrorSignal(ErrorCode)), this, SLOT(error_handle(ErrorCode)));
	m_ScuThread->start();
	m_ScpThread->start();
}


void DataWidget::init_window()
{
	open_file_btn = new QPushButton(QString::fromStdWString(L"导入文件"));
	open_dir_btn = new QPushButton(QString::fromStdWString(L"导入目录"));
	upload_file_btn = new QPushButton(QString::fromStdWString(L"上传至PACS"));

	QHBoxLayout* local_hl1 = new QHBoxLayout;
	local_hl1->addWidget(open_file_btn);
	local_hl1->addWidget(open_dir_btn);
	local_hl1->addWidget(upload_file_btn);

	mFileInfoText = new QTextEdit;
	QVBoxLayout* local_vl1 = new QVBoxLayout;
	local_vl1->addLayout(local_hl1,1);
	local_vl1->addWidget(mFileInfoText, 5);
	local_vl1->setSpacing(10);

	m_model = new QComboBox;
	m_model->setFixedSize(150, 30);
	m_model->addItem(QString::fromStdWString(L"所有模态"));
	m_model->addItem(QString::fromStdWString(L"CR"));
	m_model->addItem(QString::fromStdWString(L"DR"));
	m_model->addItem(QString::fromStdWString(L"CT"));
	m_model->addItem(QString::fromStdWString(L"MR"));
	m_model->addItem(QString::fromStdWString(L"XA"));
	m_model->addItem(QString::fromStdWString(L"US"));
	m_model->addItem(QString::fromStdWString(L"MG"));
	m_model->addItem(QString::fromStdWString(L"PT"));
	m_date = new QComboBox;
	m_date->setFixedSize(150, 30);
	m_date->addItem(QString::fromStdWString(L"所有日期"));
	m_date->addItem(QString::fromStdWString(L"日期介于"));
	QDate today = QDate::currentDate();
	m_date_edit1 = new QDateEdit;
	m_date_edit1->setDisabled(true);
	m_date_edit2 = new QDateEdit;
	m_date_edit2->setDisabled(true);
	m_date_edit1->setDate(QDate(1970, 1, 1));
	m_date_edit2->setDate(today);
	m_date_edit1->setFixedSize(160, 30);
	m_date_edit2->setFixedSize(160, 30);


	m_tag = new QComboBox;
	m_tag->setFixedSize(150, 30);
	m_tag->addItem(QString::fromStdWString(L"患者姓名"));
	m_tag->addItem(QString::fromStdWString(L"患者ID"));
	m_tag->addItem(QString::fromStdWString(L"检查号"));
	m_tag->addItem(QString::fromStdWString(L"检查描述"));

	m_tagValue = new QLineEdit;
	m_tagValue->setStyleSheet("QLineEdit{width:200px;height:30px;border-width:1px;border-style:solid;border-color:#999999;}");

	QGridLayout* gridLayout = new QGridLayout;
	gridLayout->addWidget(m_model, 0, 0, Qt::AlignLeft);
	gridLayout->addWidget(m_date, 1, 0, Qt::AlignLeft);
	gridLayout->addWidget(m_date_edit1, 1, 1, Qt::AlignLeft);
	gridLayout->addWidget(m_date_edit2, 1, 2, Qt::AlignLeft);
	gridLayout->addWidget(m_tag, 2, 0, Qt::AlignLeft);
	gridLayout->addWidget(m_tagValue, 2, 1, Qt::AlignLeft);
	gridLayout->setVerticalSpacing(20);
	gridLayout->setHorizontalSpacing(10);

	m_PacsSearchBtn = new QPushButton(QString::fromStdWString(L"PACS查询"));
	m_LocalSearchBtn = new QPushButton(QString::fromStdWString(L"本地查询"));
	m_PacsSetBtn = new QPushButton(QString::fromStdWString(L"PACS设置"));
	QHBoxLayout* h1 = new QHBoxLayout;
	h1->addWidget(m_LocalSearchBtn);
	h1->addWidget(m_PacsSearchBtn);
	h1->addWidget(m_PacsSetBtn);

	QVBoxLayout* v1 = new QVBoxLayout;
	v1->addLayout(gridLayout, 3);
	v1->addLayout(h1);
	v1->setSpacing(10);

	m_ProgressBar = new QProgressBar;
	m_ProgressBar->setRange(0, 1);
	m_ProgressBar->setValue(0);
	m_ProgressBar->setFormat(QString::fromStdWString(L"无任务"));
	m_ProgressBar->setStyleSheet("QProgressBar{font: normal bold 15pt Micosoft YaHei;text-align:center;color:black;}");

	m_OpenReportBtn = new QPushButton(QString::fromStdWString(L"读取报告"));
	m_OpenReportBtn->setFixedHeight(36);
	m_OpenReportBtn->setFixedWidth(180);
	m_OpenReportBtn->setStyleSheet("QPushButton{border-style:outset;font-size:16pt;font-family:Microsoft YaHei;text-align:center;margin:0px;padding:0px;}"
		"QPushButton::pressed{border-style:outset;font-size:16pt;font-family:Microsoft YaHei;text-align:center;margin-left:2px;margin-top:2px;padding:0px;}");

	m_Import2ViewBtn = new QPushButton(QString::fromStdWString(L"导入阅片"));
	m_Import2ViewBtn->setFixedHeight(36);
	m_Import2ViewBtn->setFixedWidth(180);
	m_Import2ViewBtn->setStyleSheet("QPushButton{border-style:outset;font-size:16pt;font-family:Microsoft YaHei;text-align:center;margin:0px;padding:0px;}"
		"QPushButton::pressed{border-style:outset;font-size:16pt;font-family:Microsoft YaHei;text-align:center;margin-left:2px;margin-top:2px;padding:0px;}");

	QHBoxLayout* btnLayout = new QHBoxLayout;
	btnLayout->addWidget(m_ProgressBar,2);
	btnLayout->addWidget(m_OpenReportBtn, 1,Qt::AlignRight);
	btnLayout->addWidget(m_Import2ViewBtn, 1, Qt::AlignRight);

	m_PatientTree = new QTableWidget;
	m_PatientTree->setObjectName("patientTree");
	m_PatientTree->setColumnCount(4);
	m_StudyTree = new QTableWidget;
	m_StudyTree->setObjectName("studyTree");
	m_StudyTree->setColumnCount(5);//4+uid(hide)
	m_StudyTree->hideColumn(0);
	m_SeriesTree = new QTableWidget;
	m_SeriesTree->setObjectName("seriesTree");
	m_SeriesTree->setColumnCount(6);//5+uid(hide)
	m_SeriesTree->hideColumn(0);
	QStringList headers, headers2, headers3;
	headers << QString::fromStdWString(L"患者ID") << QString::fromStdWString(L"患者姓名") << QString::fromStdWString(L"性别") <<
		QString::fromStdWString(L"出生日期");
	headers2 <<QString::fromStdWString(L"UID") << QString::fromStdWString(L"检查号") << QString::fromStdWString(L"检查日期") << QString::fromStdWString(L"检查描述") <<
		QString::fromStdWString(L"检查部位");
	headers3 << QString::fromStdWString(L"UID") << QString::fromStdWString(L"序列号") << QString::fromStdWString(L"模态") << QString::fromStdWString(L"序列描述") <<
		QString::fromStdWString(L"图像方位") << QString::fromStdWString(L"图像数目");
	m_PatientTree->setHorizontalHeaderLabels(headers);
	m_PatientTree->verticalHeader()->setVisible(false);
	m_PatientTree->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_PatientTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_StudyTree->setHorizontalHeaderLabels(headers2);
	m_StudyTree->verticalHeader()->setVisible(false);
	m_StudyTree->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_StudyTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_SeriesTree->setHorizontalHeaderLabels(headers3);
	m_SeriesTree->verticalHeader()->setVisible(false);
	m_SeriesTree->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_SeriesTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	QVBoxLayout* result_v1 = new QVBoxLayout;
	result_v1->addWidget(m_PatientTree,6);
	result_v1->addWidget(m_StudyTree,3);
	result_v1->addWidget(m_SeriesTree, 3);
	result_v1->addLayout(btnLayout, 1);

	m_SearchGroup = new QGroupBox(QString::fromStdWString(L"数据查询"));
	m_SearchGroup->setLayout(v1);
	m_LocalImportGrp = new QGroupBox(QString::fromStdWString(L"本地导入"));
	m_LocalImportGrp->setLayout(local_vl1);
	m_ResultsGroup = new QGroupBox(QString::fromStdWString(L"结果"));

	m_ResultsGroup->setLayout(result_v1);

	QVBoxLayout* leftPan = new QVBoxLayout;
	leftPan->addWidget(m_SearchGroup, 1);
	leftPan->addWidget(m_LocalImportGrp, 2);
	leftPan->setSpacing(20);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->addLayout(leftPan, 2);
	mainLayout->addWidget(m_ResultsGroup, 3);
	mainLayout->setMargin(20);
	mainLayout->setSpacing(20);
	this->setLayout(mainLayout);

	setStyleSheet("QGroupBox{border-width:1px;}");
	//setStyleSheet("QComboBox{border-width:1px;border-color:white;}");
	setStyleSheet("QPushButton{font-family:Microsoft YaHei;text-align:center;font-size:15pt;}");
	setStyleSheet("QGroupBox{font-family:Microsoft YaHei;text-align:center;}");


	connect(m_date, SIGNAL(currentIndexChanged(int)), this, SLOT(queryDateChanged()));
	connect(open_file_btn, SIGNAL(clicked()), this, SLOT(open_dicomfile()));
	connect(open_dir_btn, SIGNAL(clicked()), this, SLOT(open_dicomdirs()));
	connect(upload_file_btn, SIGNAL(clicked()), this, SLOT(upload_pacs()));
	connect(m_LocalSearchBtn, SIGNAL(clicked()), this, SLOT(search_local()));
	//connect(m_LocalSearchBtn, SIGNAL(clicked()), this, SLOT(search_pacs_today()));
	connect(m_PacsSearchBtn, SIGNAL(clicked()), this, SLOT(search_pacs()));
	connect(m_PacsSetBtn, SIGNAL(clicked()), this, SLOT(set_pacs()));

	connect(m_OpenReportBtn, SIGNAL(clicked()), this, SLOT(OpenReport()));
	connect(m_Import2ViewBtn, SIGNAL(clicked()), this, SLOT(importImages()));

	connect(this->m_PatientTree, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(showStudy(QTableWidgetItem*)));
	connect(this->m_StudyTree, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(showSeries(QTableWidgetItem*)));
	connect(this->m_SeriesTree, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(SeriesClickOnce(QTableWidgetItem*)));
	connect(this->m_SeriesTree, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(download_series(QTableWidgetItem*)));
	
	//connect(this->m_SeriesTree, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(getSeriesInstanceSlot(QTableWidgetItem*)));
}

void DataWidget::open_dicomfile()
{
	QFileDialog fileDialog(this);
	fileDialog.setFixedWidth(200);
	fileDialog.setWindowTitle(QString::fromStdWString(L"打开影像文件"));
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	if (fileDialog.exec() == QDialog::Accepted) {
		QString path = fileDialog.selectedFiles()[0];

		//QByteArray ba = path.toUtf8();
		//QByteArray byteArrayPercentEncoded = ba.toPercentEncoding();

		//m_DataPath = QString(path);
		m_DataPath = path;//保存文件路径

		QFileInfo fileInfo(path);
		QString suffix = fileInfo.suffix();
		if (QString::compare(suffix, QString("dcm"), Qt::CaseInsensitive) == 0)//dicom
		{
			OpenDicomFile(path);
		}
		else if (QString::compare(suffix, QString(""), Qt::CaseInsensitive) == 0)//dicom
		{
			if (check_dicom(path.toLocal8Bit().data()))
				OpenDicomFile(path);
			else
			{
				QMessageBox::information(NULL, QString::fromStdWString(L"提示"),
					QString::fromStdWString(L"不是DICOM文件！"),
					QMessageBox::Yes, QMessageBox::Yes);
				return;
			}
		}
		else if (QString::compare(suffix, QString("jpeg"), Qt::CaseInsensitive) == 0)
		{
			OpenImageFile(path);
		}
		else if(QString::compare(suffix, QString("jpg"), Qt::CaseInsensitive) == 0)
		{
			OpenImageFile(path);
		}
		else
		{
			QMessageBox::information(NULL, QString::fromStdWString(L"提示"),
				QString::fromStdWString(L"不支持的文件格式！"),
				QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
	}
	else {
		return;
	}
}

bool DataWidget::check_dicom(const char* path)
{
	FILE* f = NULL;
	char flag[4];
	if (f = fopen(path, "r"))
	{
		fseek(f, 128, 0);
		char* temp = fgets(flag, 5, f);
		QString str(temp);
		fclose(f);
		if (str == "DICM")
			return true;
		else
			return false;
	}
	return false;
}

void DataWidget::open_dicomdirs()
{
	QFileDialog fileDialog(this);
	fileDialog.setFixedWidth(200);
	fileDialog.setWindowTitle(QString::fromStdWString(L"打开影像目录"));
	fileDialog.setFileMode(QFileDialog::DirectoryOnly);
	QStringList fileNamesList;
	if (fileDialog.exec())
	{
		fileNamesList = fileDialog.selectedFiles();
	}
	else
	{
		return;
	}
	QDir dir(fileNamesList.at(0));
	dir.setFilter(QDir::Files | QDir::NoSymLinks);
	QFileInfoList list = dir.entryInfoList();

	int file_count = list.count();
	if (file_count <= 0)
	{
		return;
	}

	QString absolute_file_path = "";
	for (int i = 0; i<file_count;i++)
	{
		QFileInfo file_info = list.at(i);
		QString suffix = file_info.suffix();
		if (QString::compare(suffix, QString("dcm"), Qt::CaseInsensitive) == 0
			|| QString::compare(suffix, QString(""), Qt::CaseInsensitive) == 0)
		{
			absolute_file_path = file_info.absoluteFilePath();
			break;
		}
	}
	if (absolute_file_path != "")
	{
		m_DataPath = fileNamesList.at(0);//保存文件夹路径
		OpenDicomFile(absolute_file_path);
	}
}

void DataWidget::OpenDicomFile(QString filePath)
{
	m_CurrentSearchMode = SearchMode::Local_Import;

	if (!m_PatientList.empty())
		m_PatientList.clear();
	m_PatientTree->clearContents();
	m_PatientTree->setRowCount(0);
	m_StudyTree->clearContents();
	m_StudyTree->setRowCount(0);
	m_SeriesTree->clearContents();
	m_SeriesTree->setRowCount(0);


	CCDicomUtils* pUtils = new CCDicomUtils;
	pUtils->readDicomFile(filePath.toLocal8Bit().data());
	
	series->series_number = pUtils->seriesNumber();
	series->series_modality = pUtils->seriesModality();
	series->series_description = pUtils->seriesDescription();
	series->series_image_orientation = pUtils->seriesImageOrientation();
	series->series_image_number = QString::number(pUtils->depth());
	
	study->study_accession_number = pUtils->studyAccessionNumber();
	study->study_date = pUtils->studyDate();
	study->study_description = pUtils->studyDescription();
	study->study_bodypart = pUtils->studyBodyPart();
	
	patient->patient_id = pUtils->patientId();
	patient->patient_name = pUtils->patientName();
	patient->patient_birthday = pUtils->patientBirthday();
	patient->patient_sex = pUtils->patientSex();

	delete pUtils;

	m_PatientList.push_back(patient);

	m_PatientTree->insertRow(0);
	m_PatientTree->setItem(0, 0, new QTableWidgetItem(patient->patient_id));
	m_PatientTree->setItem(0, 1, new QTableWidgetItem(patient->patient_name));
	m_PatientTree->setItem(0, 2, new QTableWidgetItem(patient->patient_sex));
	m_PatientTree->setItem(0, 3, new QTableWidgetItem(patient->patient_birthday));
}

void DataWidget::OpenImageFile(QString filePath)
{
	m_CurrentSearchMode = SearchMode::Local_Import;

	convertjpg2dcm(filePath);
}

void DataWidget::convertjpg2dcm(QString filePath)
{
	Img2DicomUtils *pUtils = new Img2DicomUtils;
	QString dicomPath = filePath + ".dcm";
	pUtils->setPaths(filePath);

	CCDicomInfoDlg dlg;
	QString p_name = "PatientName=";
	QString p_id = "PatientID=";
	QString p_sex = "PatientSex=";
	QString p_birth = "PatientBirthDate=";

	QString s_acce = "AccessionNumber=";
	QString s_date = "StudyDate=";
	QString s_desc = "StudyDescription=";
	QString s_body = "BodyPartExamined=";

	QString r_numb = "SeriesNumber=";
	QString r_desc = "SeriesDescription=";
	QString r_moda = "Modality=";

	QString charac = "SpecificCharacterSet=";

	if (dlg.exec() == QDialog::Accepted)
	{
		p_name += dlg.p_name;
		p_id += dlg.p_id;
		p_sex += dlg.p_sex;
		p_birth += dlg.p_birth;

		s_acce += dlg.s_acce;
		s_date += dlg.s_date;
		s_desc += dlg.s_desc;
		s_body += dlg.s_body;

		r_numb += dlg.r_numb;
		r_moda += dlg.r_moda;
		r_desc += dlg.r_desc;
	}
	charac += "GB18030";
	QList<QString> list;
	list.push_back(p_id);
	list.push_back(p_name);
	list.push_back(p_sex);
	list.push_back(p_birth);
	list.push_back(s_acce);

	list.push_back(s_desc);
	list.push_back(s_date);
	list.push_back(s_body);
	list.push_back(r_numb);
	list.push_back(r_moda);
	list.push_back(r_desc);
	list.push_back(charac);
	pUtils->setKeys(list);
	pUtils->write(dicomPath);

	m_DataPath = dicomPath;
	OpenDicomFile(dicomPath);
}

void DataWidget::queryDateChanged()
{
	QDate today = QDate::currentDate();
	m_date_edit1->setDate(QDate(1970,1,1));
	m_date_edit2->setDate(today);

	if (m_date->currentIndex() == 0)
	{
		m_date_edit1->setDisabled(true);
		m_date_edit2->setDisabled(true);
	}
	else
	{
		m_date_edit1->setDisabled(false);
		m_date_edit2->setDisabled(false);
	}
}

void DataWidget::get_query_contidion()
{
	//modality
	switch (m_model->currentIndex())
	{
	case 0:
		condition_modality = "*";
		break;
	case 1:
		condition_modality = "CR";
		break;
	case 2:
		condition_modality = "DR";
		break;
	case 3:
		condition_modality = "CT";
		break;
	case 4:
		condition_modality = "MR";
		break;
	case 5:
		condition_modality = "XA";
		break;
	case 6:
		condition_modality = "US";
		break;
	case 7:
		condition_modality = "MG";
		break;
	case 8:
		condition_modality = "PT";
		break;
	default:
		condition_modality = "*";
		break;
	}
	//date
	condition_date_begin = m_date_edit1->date().toString("yyyyMMdd");
	condition_date_end = m_date_edit2->date().toString("yyyyMMdd");
	//custom tag
	switch (m_tag->currentIndex())
	{
	case 0:
		condition_tag = "PatientName";
		break;
	case 1:
		condition_tag = "PatientId";
		break;
	case 2:
		condition_tag = "AccessionNumber";
		break;
	case 3:
		condition_tag = "StudyDescription";
		break;
	case 4:
		condition_tag = "DoctorName";
		break;
	case 5:
		condition_tag = "Instritution";
		break;
	default:
		break;
	}
	condition_value = m_tagValue->text();
}

void DataWidget::upload_pacs()
{
	QStringList nameList;
	QFileDialog fileDialog(this);
	fileDialog.setFixedWidth(200);
	fileDialog.setWindowTitle(QString::fromStdWString(L"上传影像文件"));
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setFileMode(QFileDialog::ExistingFile);
	//fileDialog.setNameFilter(tr("DICOM(*.dcm)"));
	if (fileDialog.exec() == QDialog::Accepted) {
		nameList = fileDialog.selectedFiles();
	}
	for (int i = 0; i < nameList.size(); ++i)
	{
		QString name = nameList.at(i);
		QFileInfo fileInfo(name);
		QString suffix = fileInfo.suffix();
		if ((QString::compare(suffix, QString("jpg"), Qt::CaseInsensitive) == 0)
			|| (QString::compare(suffix, QString("jpeg"), Qt::CaseInsensitive) == 0))//dicom
		{
			convertjpg2dcm(fileInfo.absoluteFilePath());
			name = m_DataPath;
		}
		else if (!check_dicom(name.toLocal8Bit().data()))
		{
			QMessageBox::information(NULL, QString::fromStdWString(L"提示"),
				QString::fromStdWString(L"不是DICOM文件！"),
				QMessageBox::Yes, QMessageBox::Yes);
			continue;
		}
		BeginUploadImage(name);
	}
}

void DataWidget::BeginUploadImage(QString path)
{
	m_upload_flag = true;

	QFileInfo info(path);
	m_upload_size = info.size();
	emit UploadImage(path);
	m_ProgressBar->setRange(0, 99);
	m_ProgressBar->setValue(0);
	m_ProgressBar->setFormat(QString::fromStdWString(L"等待上传"));
}

void DataWidget::UpdateUploadProgress(unsigned long current)
{
	if (!m_upload_flag) return;
	int pos = current / 10000;
	int total = m_upload_size / 10000;
	float ff = (float)pos / (float)total;
	int ii = ff * 100;
	m_ProgressBar->setValue(ii);
	m_ProgressBar->setFormat(QString::fromStdWString(L"正在上传") + QString("%1%").arg(ii));
	
	if (ff>0.98 )
	{
		Sleep(100);
		m_ProgressBar->setFormat(QString::fromStdWString(L"上传完成"));
		m_ProgressBar->setValue(0);
		m_ProgressBar->setFormat(QString::fromStdWString(L"无任务"));
		m_upload_flag = false;
		QMessageBox::information(NULL, QString::fromStdWString(L"提示"),
			QString::fromStdWString(L"上传成功！"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	}
	
}

void DataWidget::upload_dir_to_pacs(QString path)
{
	QDir dir(path);
	QStringList file_list = dir.entryList();
	for (int i = 0; i < file_list.size(); ++i)
	{
		QString name = file_list.at(i);
		QFileInfo info(path+"/"+name);
		if (info.isFile())
			emit UploadImage(path + "/" + name);
	}
}

void DataWidget::search_local()
{
	m_CurrentSearchMode = SearchMode::Local_Search;
	get_query_contidion();
	emit QueryPatient(condition_modality, condition_date_begin, condition_date_end, condition_tag, condition_value);
}

void DataWidget::search_pacs()
{
	m_CurrentSearchMode = SearchMode::Pacs_Search;
	//test
	m_pScu->init_dataset();
	get_query_contidion();

	m_pScu->SetQueryLevel();
	if (condition_tag=="PatientName")
		m_pScu->AddQueryCondition(DCM_PatientName, QString("*"+condition_value+"*").toLocal8Bit().data());
	else if (condition_tag=="PatientId")
		m_pScu->AddQueryCondition(DCM_PatientID, QString("*" + condition_value + "*").toLocal8Bit().data());
	else if (condition_tag=="AccessionNumber")
		m_pScu->AddQueryCondition(DCM_AccessionNumber, QString("*" + condition_value + "*").toLocal8Bit().data());
	else if (condition_tag=="StudyDescription")
		m_pScu->AddQueryCondition(DCM_StudyDescription, QString("*" + condition_value + "*").toLocal8Bit().data());

	emit SearchPatient(condition_date_begin,condition_date_end);

	//设置状态
	//m_workTimer->start(3000);
	m_ProgressBar->setFormat(QString::fromStdWString(L"正在查询Patient..."));

}

void DataWidget::search_pacs_today()
{
	m_CurrentSearchMode = SearchMode::Pacs_Search;
	//test
	m_pScu->init_dataset();
	get_query_contidion();
	condition_date_begin = QDate::currentDate().toString("yyyyMMdd");
	condition_date_begin = QDate::currentDate().toString("yyyyMMdd");
	

	m_pScu->SetQueryLevel();
	if (condition_tag == "PatientName")
		m_pScu->AddQueryCondition(DCM_PatientName, QString("*" + condition_value + "*").toLocal8Bit().data());
	else if (condition_tag == "PatientId")
		m_pScu->AddQueryCondition(DCM_PatientID, QString("*" + condition_value + "*").toLocal8Bit().data());
	else if (condition_tag == "AccessionNumber")
		m_pScu->AddQueryCondition(DCM_AccessionNumber, QString("*" + condition_value + "*").toLocal8Bit().data());
	else if (condition_tag == "StudyDescription")
		m_pScu->AddQueryCondition(DCM_StudyDescription, QString("*" + condition_value + "*").toLocal8Bit().data());

	emit SearchPatient(condition_date_begin, condition_date_end);

	//设置状态
	//m_workTimer->start(3000);
	m_ProgressBar->setFormat(QString::fromStdWString(L"正在查询Patient..."));
}

void DataWidget::set_pacs()
{
	PacsSetDlg pacsdlg;
	if (pacsdlg.exec() == QDialog::Accepted)
	{
		OFString title = OFString(pacsdlg.m_mytitle.toLocal8Bit().data());
		unsigned short port = pacsdlg.m_listening_port.toUShort();
		OFString peer_title = OFString(pacsdlg.m_peer_title.toLocal8Bit().data());
		OFString peer_ip = OFString(pacsdlg.m_peer_ip.toLocal8Bit().data());
		unsigned short peer_port = pacsdlg.m_peer_port.toUShort();

		m_pScu->init_association_parameter(title, peer_title, peer_ip, peer_port);
	}
}

/************************************************************************/
/* pacs  slots                                                         */
/************************************************************************/
void DataWidget::process_patient_table(QMap<QString, Patient*> pmap)
{
	m_ProgressBar->setFormat(QString::fromStdWString(L"Patient查询完成"));
	Sleep(100);
	m_ProgressBar->setFormat(QString::fromStdWString(L"无任务"));

	//clear all table content
	m_PatientTree->clearContents();
	m_PatientTree->setRowCount(0);
	m_StudyTree->clearContents();
	m_StudyTree->setRowCount(0);
	m_SeriesTree->clearContents();
	m_SeriesTree->setRowCount(0);

	QMap<QString, Patient*>::iterator it = pmap.begin();
	int row_index = 0;
	for (it; it != pmap.end(); ++it)
	{
		pCurrentPatient = (*it);
		m_PatientTree->insertRow(row_index);
		m_PatientTree->setItem(row_index, 0, new QTableWidgetItem(pCurrentPatient->patient_id));
		m_PatientTree->setItem(row_index, 1, new QTableWidgetItem(pCurrentPatient->patient_name));
		m_PatientTree->setItem(row_index, 2, new QTableWidgetItem(pCurrentPatient->patient_sex));
		m_PatientTree->setItem(row_index, 3, new QTableWidgetItem(pCurrentPatient->patient_birthday));

		++row_index;
	}
}

void DataWidget::process_study_table(QList<Study*> slist)
{
	//m_ProgressBar->setFormat(QString::fromStdWString(L"Study查询完成"));
	//Sleep(100);
	//m_ProgressBar->setFormat(QString::fromStdWString(L"无任务"));

	//clear study and series table content
	m_StudyTree->clearContents();
	m_StudyTree->setRowCount(0);
	m_SeriesTree->clearContents();
	m_SeriesTree->setRowCount(0);

	QList<Study*>::iterator it = slist.begin();
	int row_index = 0;
	for (it; it != slist.end(); ++it)
	{
		pCurrentStudy = (*it);
		m_StudyTree->insertRow(row_index);
		m_StudyTree->setItem(row_index, 0, new QTableWidgetItem(pCurrentStudy->study_instance_uid));
		m_StudyTree->setItem(row_index, 1, new QTableWidgetItem(pCurrentStudy->study_accession_number));
		m_StudyTree->setItem(row_index, 2, new QTableWidgetItem(pCurrentStudy->study_date));
		m_StudyTree->setItem(row_index, 3, new QTableWidgetItem(pCurrentStudy->study_description));
		m_StudyTree->setItem(row_index, 4, new QTableWidgetItem(pCurrentStudy->study_bodypart));

		++row_index;
	}

}

void DataWidget::process_series_table(QMap<QString, Series*> smap)
{
	m_ProgressBar->setFormat(QString::fromStdWString(L"Series查询完成"));
	Sleep(100);
	m_ProgressBar->setFormat(QString::fromStdWString(L"无任务"));

	//clear series table content
	m_SeriesTree->clearContents();
	m_SeriesTree->setRowCount(0);

	QMap<QString, Series*>::iterator it = smap.begin();
	int row_index = 0;
	for (it; it != smap.end(); ++it)
	{
		pCurrentSeries = (*it);
		m_SeriesTree->insertRow(row_index);
		m_SeriesTree->setItem(row_index, 0, new QTableWidgetItem(pCurrentSeries->series_instance_uid));
		m_SeriesTree->setItem(row_index, 1, new QTableWidgetItem(pCurrentSeries->series_number));
		m_SeriesTree->setItem(row_index, 2, new QTableWidgetItem(pCurrentSeries->series_modality));
		m_SeriesTree->setItem(row_index, 3, new QTableWidgetItem(pCurrentSeries->series_description));
		m_SeriesTree->setItem(row_index, 4, new QTableWidgetItem(pCurrentSeries->series_image_orientation));
		m_SeriesTree->setItem(row_index, 5, new QTableWidgetItem(pCurrentSeries->series_image_number));

		++row_index;
	}
}
/************************************************************************/
/* local database slots                                                */
/************************************************************************/
void DataWidget::process_patient_table(QList<QStringList> list)
{
	//clear all table content
	m_PatientTree->clearContents();
	m_PatientTree->setRowCount(0);
	m_StudyTree->clearContents();
	m_StudyTree->setRowCount(0);
	m_SeriesTree->clearContents();
	m_SeriesTree->setRowCount(0);

	int index = 0;
	QList<QStringList>::iterator it = list.begin();
	for (it; it != list.end(); ++it)
	{
		QStringList strlist = (*it);
		m_PatientTree->insertRow(index);
		m_PatientTree->setItem(index, 0, new QTableWidgetItem(strlist.at(0)));
		m_PatientTree->setItem(index, 1, new QTableWidgetItem(strlist.at(1)));
		m_PatientTree->setItem(index, 2, new QTableWidgetItem(strlist.at(2)));
		m_PatientTree->setItem(index, 3, new QTableWidgetItem(strlist.at(3)));
		index++;
	}
}

void DataWidget::process_study_table(QList<QStringList> list)
{
	m_StudyTree->clearContents();
	m_StudyTree->setRowCount(0);
	m_SeriesTree->clearContents();
	m_SeriesTree->setRowCount(0);

	int index = 0;
	QList<QStringList>::iterator it = list.begin();
	for (it; it != list.end(); ++it)
	{
		QStringList strlist = (*it);
		m_StudyTree->insertRow(index);
		m_StudyTree->setItem(index, 0, new QTableWidgetItem(strlist.at(0)));
		m_StudyTree->setItem(index, 1, new QTableWidgetItem(strlist.at(1)));
		m_StudyTree->setItem(index, 2, new QTableWidgetItem(strlist.at(2)));
		m_StudyTree->setItem(index, 3, new QTableWidgetItem(strlist.at(3)));
		m_StudyTree->setItem(index, 4, new QTableWidgetItem(strlist.at(4)));
		index++;
	}
}

void DataWidget::process_series_table(QList<QStringList> list)
{
	m_SeriesTree->clearContents();
	m_SeriesTree->setRowCount(0);

	int index = 0;
	QList<QStringList>::iterator it = list.begin();
	for (it; it != list.end(); ++it)
	{
		QStringList strlist = (*it);
		m_SeriesTree->insertRow(index);
		m_SeriesTree->setItem(index, 0, new QTableWidgetItem(strlist.at(0)));
		m_SeriesTree->setItem(index, 1, new QTableWidgetItem(strlist.at(1)));
		m_SeriesTree->setItem(index, 2, new QTableWidgetItem(strlist.at(2)));
		m_SeriesTree->setItem(index, 3, new QTableWidgetItem(strlist.at(3)));
		index++;
	}
}

/************************************************************************/
/* local import, local database, pacs show in table                     */
/************************************************************************/
void DataWidget::showStudy(QTableWidgetItem* item)
{
	//clear study
	m_StudyTree->clearContents();
	m_StudyTree->setRowCount(0);
	m_SeriesTree->clearContents();
	m_SeriesTree->setRowCount(0);

	if (m_CurrentSearchMode == SearchMode::Local_Import)
	{
		int row = item->row();
		pCurrentStudy = study;

		m_StudyTree->insertRow(0);
		m_StudyTree->setItem(0, 0, new QTableWidgetItem(pCurrentStudy->study_instance_uid));
		m_StudyTree->setItem(0, 1, new QTableWidgetItem(pCurrentStudy->study_accession_number));
		m_StudyTree->setItem(0, 2, new QTableWidgetItem(pCurrentStudy->study_date));
		m_StudyTree->setItem(0, 3, new QTableWidgetItem(pCurrentStudy->study_description));
		m_StudyTree->setItem(0, 4, new QTableWidgetItem(pCurrentStudy->study_bodypart));
	}
	else if (m_CurrentSearchMode == SearchMode::Pacs_Search)
	{
		int row = item->row();
		QString uid = m_PatientTree->item(row,0)->text();
		patient->patient_id = uid;
		patient->patient_name = m_PatientTree->item(row, 1)->text();
		patient->patient_sex = m_PatientTree->item(row, 2)->text();
		patient->patient_birthday = m_PatientTree->item(row, 3)->text();

		pCurrentPatient = patient;
		m_CurrentPatientId = uid;
		emit SearchStudy(uid);
		//m_ProgressBar->setFormat(QString::fromStdWString(L"正在查询Study..."));
	}
	else if (m_CurrentSearchMode==SearchMode::Local_Search)
	{
		int row = item->row();
		QString uid = m_PatientTree->item(row, 0)->text();
		patient->patient_id = uid;
		patient->patient_name = m_PatientTree->item(row, 1)->text();
		patient->patient_sex = m_PatientTree->item(row, 2)->text();
		patient->patient_birthday = m_PatientTree->item(row, 3)->text();

		pCurrentPatient = patient;
		m_CurrentPatientId = uid;

		emit QueryStudy(uid);
	}
}

void DataWidget::showSeries(QTableWidgetItem* item)
{
	m_SeriesTree->clearContents();
	m_SeriesTree->setRowCount(0);

	if (m_CurrentSearchMode == SearchMode::Local_Import)
	{
		int row = item->row();
		pCurrentSeries = series;
		//qDebug() << "MMMMMM:" << pCurrentSeries->series_modality;

		m_SeriesTree->insertRow(0);
		m_SeriesTree->setItem(0, 0, new QTableWidgetItem(pCurrentSeries->series_instance_uid));
		m_SeriesTree->setItem(0, 1, new QTableWidgetItem(pCurrentSeries->series_number));
		m_SeriesTree->setItem(0, 2, new QTableWidgetItem(pCurrentSeries->series_modality));
		m_SeriesTree->setItem(0, 3, new QTableWidgetItem(pCurrentSeries->series_description));
		m_SeriesTree->setItem(0, 4, new QTableWidgetItem(pCurrentSeries->series_image_orientation));
		m_SeriesTree->setItem(0, 5, new QTableWidgetItem(pCurrentSeries->series_image_number));
	}
	else if (m_CurrentSearchMode == SearchMode::Pacs_Search)
	{
		int row = item->row();
		QString uid = m_StudyTree->item(row, 0)->text();
		study->study_instance_uid = uid;
		study->study_accession_number = m_StudyTree->item(row, 1)->text();
		study->study_date = m_StudyTree->item(row, 2)->text();
		study->study_description = m_StudyTree->item(row, 3)->text();
		study->study_bodypart = m_StudyTree->item(row, 4)->text();

		pCurrentStudy = study;
		m_CurrentStudyUid = uid;

		m_pScu->SetQueryLevel("SERIES");
		m_pScu->AddQueryCondition(DCM_StudyInstanceUID, uid.toLocal8Bit().data());

		emit SearchSeries(uid);
		m_ProgressBar->setFormat(QString::fromStdWString(L"正在查询Series..."));
	}
	else if (m_CurrentSearchMode == SearchMode::Local_Search)
	{
		int row = item->row();
		QString uid = m_StudyTree->item(row, 0)->text();
		study->study_instance_uid = uid;
		study->study_accession_number = m_StudyTree->item(row, 1)->text();
		study->study_date = m_StudyTree->item(row, 2)->text();
		study->study_description = m_StudyTree->item(row, 3)->text();
		study->study_bodypart = m_StudyTree->item(row, 4)->text();

		pCurrentStudy = study;
		m_CurrentStudyUid = uid;

		emit QuerySeries(uid);
	}
}

void DataWidget::SeriesClickOnce(QTableWidgetItem* item)
{
	if (m_CurrentSearchMode == SearchMode::Local_Import)
		return;

	int row = item->row();
	QString uid = m_SeriesTree->item(row, 0)->text();
	series->series_instance_uid = uid;

	pCurrentSeries = series;
	//qDebug() << "MMMMMM:"<<pCurrentSeries->series_modality;
	m_CurrentSeriesUid = uid;
}

void DataWidget::download_series(QTableWidgetItem* item)
{
	if (m_CurrentSearchMode == SearchMode::Pacs_Search)
	{
		int row = item->row();
		QString series_uid = m_SeriesTree->item(row, 0)->text();
		series->series_instance_uid = series_uid;
		series->series_number = m_SeriesTree->item(row, 1)->text();
		series->series_modality = m_SeriesTree->item(row, 2)->text();
		series->series_description = m_SeriesTree->item(row, 3)->text();
		series->series_image_orientation = m_SeriesTree->item(row, 4)->text();
		series->series_image_number = m_SeriesTree->item(row, 5)->text();

		pCurrentSeries = series;
		m_CurrentSeriesUid = series_uid;

		//插入数据库
		emit InsertDatabase(pCurrentPatient, pCurrentStudy, pCurrentSeries);
		//设置保存目录
		QString rootPath = "./Data";
		QDir dir;
		if (!dir.exists(rootPath))
			dir.mkdir(rootPath);
		if (!dir.exists(rootPath + "\\" + m_CurrentPatientId))
			dir.mkdir(rootPath + "\\" + m_CurrentPatientId);
		if (!dir.exists(rootPath + "\\" + m_CurrentPatientId + "\\" + m_CurrentStudyUid))
			dir.mkdir(rootPath + "\\" + m_CurrentPatientId + "\\" + m_CurrentStudyUid);
		if (!dir.exists(rootPath + "\\" + m_CurrentPatientId + "\\" + m_CurrentStudyUid + "\\" + m_CurrentSeriesUid))
			dir.mkdir(rootPath + "\\" + m_CurrentPatientId + "\\" + m_CurrentStudyUid + "\\" + m_CurrentSeriesUid);
		//删除目录下所有文件
		QString absolutePath = rootPath + "\\" + m_CurrentPatientId + "\\" + m_CurrentStudyUid + "\\" + m_CurrentSeriesUid;
		QDir dir1(absolutePath);
		QStringList list = dir1.entryList(QDir::Files);
		for (int i = 0; i < list.size(); ++i)
		{
			QFile::remove(absolutePath+"/"+list.at(i));
		}

		m_pScp->SetRoot(OFString(rootPath.toLocal8Bit().data()));
		m_pScp->SetSubDir(OFString((m_CurrentPatientId + "\\" + m_CurrentStudyUid + "\\" + m_CurrentSeriesUid).toLocal8Bit().data()));

		emit DownloadSeries(m_CurrentPatientId, m_CurrentStudyUid, m_CurrentSeriesUid);

		m_ProgressBar->setValue(0);
		m_ProgressBar->setFormat(QString::fromStdWString(L"正在下载")+QString("%1%").arg(0));
		
	}
}

void DataWidget::FinishDownload()
{
	m_ProgressBar->setValue(1);
	m_ProgressBar->setFormat(QString::fromStdWString(L"正在下载") + QString("%1%").arg(1));
	Sleep(100);
	m_ProgressBar->setFormat(QString::fromStdWString(L"下载完成"));
	m_ProgressBar->setValue(0);
	m_ProgressBar->setFormat(QString::fromStdWString(L"无任务"));
}

void DataWidget::importImages()
{
	if (m_CurrentSearchMode != SearchMode::Local_Import)
	{
		QString root = "./Data/";
		QString dir1 = pCurrentPatient->patient_id;
		QString dir2 = pCurrentStudy->study_instance_uid;
		QString dir3 = pCurrentSeries->series_instance_uid;
		m_DataPath = root + dir1 + "/" + dir2 + "/" + dir3;
	}

	//QUrl url(QString("file:///")+m_DataPath);
	//QList<QUrl> list;
	//list.push_back(url);
	QList<QString> str_list;
	str_list.push_back(m_DataPath);
	emit GotoOtherTab(1);
	emit ImportImages2View2(str_list);
}

void DataWidget::OpenReport()
{
	/*if (m_CurrentSearchMode != SearchMode::Local_Import)
	{
	QString root = "./Data/";
	if (pCurrentPatient == NULL || pCurrentSeries == NULL || pCurrentStudy == NULL)
	return;
	QString dir1 = pCurrentPatient->patient_id;
	QString dir2 = pCurrentStudy->study_instance_uid;
	QString dir3 = pCurrentSeries->series_instance_uid;
	m_DataPath = root + dir1 + "/" + dir2 + "/" + dir3;

	QDir dir(m_DataPath);
	if (!dir.exists()) return;
	QStringList list = dir.entryList(QDir::Files);
	if (list.size() < 1)
	return;
	QString absolutePath = m_DataPath + "/"+ list.at(0);


	QString program = "bin/d2p.exe";
	QStringList arguments;
	arguments << absolutePath << "cache/report.pdf";
	QProcess *process = new QProcess(this);
	process->start(program, arguments);
	process->waitForFinished(2000);

	emit GotoOtherTab(2);
	emit FinishConvertDcm2Pdf();
	}*/
}

void DataWidget::restoreState()
{
	m_workTimer->stop();
	m_ProgressBar->setFormat(QString::fromStdWString(L"无任务"));
}

void DataWidget::error_handle(ErrorCode code)
{
	if (ErrorCode::INIT_NETWORK == code)
	{
		QMessageBox::information(NULL, QString::fromStdWString(L"提示"),
			QString::fromStdWString(L"初始化网络失败！"),
			QMessageBox::Yes, QMessageBox::Yes);
	}
	else if (ErrorCode::ASSOCIATION == code)
	{
		QMessageBox::information(NULL, QString::fromStdWString(L"提示"),
			QString::fromStdWString(L"连接服务器失败！"),
			QMessageBox::Yes, QMessageBox::Yes);
	}
	m_ProgressBar->setFormat(QString::fromStdWString(L"无任务"));
}

void DataWidget::resizeEvent(QResizeEvent* event)
{
	int widget_width = m_PatientTree->width();
	int per_item_width = widget_width / 4;
	QHeaderView* headerView = m_PatientTree->horizontalHeader();
	headerView->resizeSection(0, per_item_width);
	headerView->resizeSection(1, per_item_width);
	headerView->resizeSection(2, per_item_width);
	headerView->resizeSection(3, per_item_width);
	headerView->setStretchLastSection(true);
	headerView->setDefaultAlignment(Qt::AlignCenter);

	widget_width = m_StudyTree->width();
	per_item_width = widget_width / 4;
	headerView = m_StudyTree->horizontalHeader();
	headerView->resizeSection(0, per_item_width);
	headerView->resizeSection(1, per_item_width);
	headerView->resizeSection(2, per_item_width);
	headerView->setStretchLastSection(true);
	headerView->setDefaultAlignment(Qt::AlignCenter);

	widget_width = m_SeriesTree->width();
	per_item_width = widget_width / 5;
	headerView = m_SeriesTree->horizontalHeader();
	headerView->resizeSection(0, per_item_width);
	headerView->resizeSection(1, per_item_width);
	headerView->resizeSection(2, per_item_width);
	headerView->resizeSection(3, per_item_width);
	headerView->setStretchLastSection(true);
	headerView->setDefaultAlignment(Qt::AlignCenter);
}