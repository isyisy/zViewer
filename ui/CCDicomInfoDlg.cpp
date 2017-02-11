#include "CCDicomInfoDlg.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDate>
#include <QTime>
#include <QDateTime>

CCDicomInfoDlg::CCDicomInfoDlg()
{
	init_window();
	init_controls();
}

CCDicomInfoDlg::~CCDicomInfoDlg()
{

}

void CCDicomInfoDlg::init_window()
{
	m_PatientId = new QLabel(QString::fromStdWString(L"住院号："),this);
	m_Edit_PatientId = new QLineEdit(this);
	m_PatientName = new QLabel(QString::fromStdWString(L"姓名："),this);
	m_Edit_PatientName = new QLineEdit(this);
	m_PatientSex = new QLabel(QString::fromStdWString(L"性别："),this);
	m_Edit_PatientSex = new QLineEdit(this);
	m_PatientBirth = new QLabel(QString::fromStdWString(L"出生日期："),this);
	m_Edit_PatientBirth = new QLineEdit(this);

	m_StudyDate = new QLabel(QString::fromStdWString(L"检查日期："),this);
	m_Edit_StudyDate = new QLineEdit(this);
	m_AccessionNumber = new QLabel(QString::fromStdWString(L"检查号："),this);
	m_Edit_AccessionNumber = new QLineEdit(this);
	m_StudyDescription = new QLabel(QString::fromStdWString(L"检查描述："),this);
	m_Edit_StudyDescription = new QLineEdit(this);
	m_BodyPart = new QLabel(QString::fromStdWString(L"检查部位："),this);
	m_Edit_BodyPart = new QLineEdit(this);

	m_SeriesNumber = new QLabel(QString::fromStdWString(L"序列号："),this);
	m_Edit_SeriesNumber = new QLineEdit(this);
	m_SeriesDesctiption = new QLabel(QString::fromStdWString(L"序列描述："),this);
	m_Edit_SeriesDesctiption = new QLineEdit(this);
	m_SeriesModality = new QLabel(QString::fromStdWString(L"模态："),this);
	m_Edit_SeriesModality = new QLineEdit(this);

	QGridLayout* grid = new QGridLayout;
	grid->addWidget(m_PatientId, 0, 0, Qt::AlignRight | Qt::AlignCenter);
	grid->addWidget(m_Edit_PatientId, 0, 1, Qt::AlignLeft | Qt::AlignCenter);
	grid->addWidget(m_PatientName, 1, 0, Qt::AlignRight | Qt::AlignCenter);
	grid->addWidget(m_Edit_PatientName, 1, 1, Qt::AlignLeft | Qt::AlignCenter);
	grid->addWidget(m_PatientSex, 2, 0, Qt::AlignRight | Qt::AlignCenter);
	grid->addWidget(m_Edit_PatientSex, 2, 1, Qt::AlignLeft | Qt::AlignCenter);
	grid->addWidget(m_PatientBirth, 3, 0, Qt::AlignRight | Qt::AlignCenter);
	grid->addWidget(m_Edit_PatientBirth, 3, 1, Qt::AlignLeft | Qt::AlignCenter);

	grid->addWidget(m_AccessionNumber, 4, 0, Qt::AlignRight | Qt::AlignCenter);
	grid->addWidget(m_Edit_AccessionNumber, 4, 1, Qt::AlignLeft | Qt::AlignCenter);
	grid->addWidget(m_StudyDate, 5, 0, Qt::AlignRight | Qt::AlignCenter);
	grid->addWidget(m_Edit_StudyDate, 5, 1, Qt::AlignLeft | Qt::AlignCenter);
	grid->addWidget(m_StudyDescription, 6, 0, Qt::AlignRight | Qt::AlignCenter);
	grid->addWidget(m_Edit_StudyDescription, 6, 1, Qt::AlignLeft | Qt::AlignCenter);
	grid->addWidget(m_BodyPart, 7, 0, Qt::AlignRight | Qt::AlignCenter);
	grid->addWidget(m_Edit_BodyPart, 7, 1, Qt::AlignLeft | Qt::AlignCenter);

	grid->addWidget(m_SeriesNumber, 8, 0, Qt::AlignRight | Qt::AlignCenter);
	grid->addWidget(m_Edit_SeriesNumber, 8, 1, Qt::AlignLeft | Qt::AlignCenter);
	grid->addWidget(m_SeriesDesctiption, 9, 0, Qt::AlignRight | Qt::AlignCenter);
	grid->addWidget(m_Edit_SeriesDesctiption, 9, 1, Qt::AlignLeft | Qt::AlignCenter);
	grid->addWidget(m_SeriesModality, 10, 0, Qt::AlignRight | Qt::AlignCenter);
	grid->addWidget(m_Edit_SeriesModality, 10, 1, Qt::AlignLeft | Qt::AlignCenter);

	grid->setColumnStretch(0, 1);
	grid->setColumnStretch(1, 1);

	QGroupBox* infoGroup = new QGroupBox(this);
	infoGroup->setLayout(grid);

	m_CancelBtn = new QPushButton(QString::fromStdWString(L"取消"),this);
	m_OkBtn = new QPushButton(QString::fromStdWString(L"确定"),this);

	QHBoxLayout *hL = new QHBoxLayout;
	hL->addStretch(2);
	hL->addWidget(m_CancelBtn, 1);
	hL->addWidget(m_OkBtn, 1);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(infoGroup, 10);
	mainLayout->addLayout(hL, 1);

	this->setFixedSize(300, 500);
	this->setLayout(mainLayout);

	connect(m_OkBtn, SIGNAL(clicked()), this, SLOT(_ok()));
	connect(m_CancelBtn, SIGNAL(clicked()), this, SLOT(_cancel()));

	setStyleSheet("QLineEdit{border-width:1px;border-style:solid;border-color:#999999;}"
		"QLabel{font: normal 13pt Micosoft YaHei; color:white;}");
}

void CCDicomInfoDlg::init_controls()
{
	QDate date = QDate::currentDate();
	QString str_date = date.toString("yyyyMMdd");
	QTime time = QTime::currentTime();
	QString str_time = time.toString("hhmmss");
	m_Edit_StudyDate->setText(str_date);
	m_Edit_SeriesModality->setText("JPEG");
	m_Edit_SeriesDesctiption->setText("custom image");
}

void CCDicomInfoDlg::_ok()
{
	p_name = m_Edit_PatientName->text();
	p_id = m_Edit_PatientId->text();
	p_sex = m_Edit_PatientSex->text();
	p_birth = m_Edit_PatientBirth->text();

	s_acce = m_Edit_AccessionNumber->text();
	s_date = m_Edit_StudyDate->text();
	s_desc = m_Edit_StudyDescription->text();
	s_body = m_Edit_BodyPart->text();

	r_numb = m_Edit_SeriesNumber->text();
	r_desc = m_Edit_SeriesDesctiption->text();
	r_moda = m_Edit_SeriesModality->text();

	QDialog::accept();
}

void CCDicomInfoDlg::_cancel()
{
	p_name = "";
	p_id = "";
	p_sex = "";
	p_birth = "";

	s_acce = "";
	s_date = "";
	s_desc = "";
	s_body = "";

	r_numb = "";
	r_desc = "";
	r_moda = "";

	QDialog::reject();
}