#include "ReportEditView.h"
#include "globals/CCGlobalConsts.h"
#include <QStyleOption>
#include <QPainter>
#include <QGridLayout>
#include <QPixmap>
#include <QImage>
#include <QDate>
#include <QPdfWriter>

ReportEditView::ReportEditView(QWidget *parent)
	: QWidget(parent)
	, m_id_label(new QLabel(QString::fromLocal8Bit("ID:")))
	, m_name_label(new QLabel(QString::fromLocal8Bit("名字:")))
	, m_type_label(new QLabel(QString::fromLocal8Bit("种类:")))
	, m_breed_label(new QLabel(QString::fromLocal8Bit("品种:")))
	, m_sex_label(new QLabel(QString::fromLocal8Bit("性别:")))
	, m_age_label(new QLabel(QString::fromLocal8Bit("年龄:")))
	, m_body_label(new QLabel(QString::fromLocal8Bit("检查部位:")))
	, m_date_label(new QLabel(QString::fromLocal8Bit("日期:")))
	, m_modality_label(new QLabel(QString::fromLocal8Bit("模态:")))
	, m_id_edit(new QLineEdit)
	, m_name_edit(new QLineEdit)
	, m_type_edit(new QLineEdit)
	, m_breed_edit(new QLineEdit)
	, m_sex_edit(new QLineEdit)
	, m_age_edit(new QLineEdit)
	, m_body_edit(new QLineEdit)
	, m_date_edit(new QLineEdit)
	, m_modality_edit(new QLineEdit)
	, m_BasicInfoGrp(new QGroupBox(QString::fromLocal8Bit("基本信息")))
	, m_BaiweiGrp(new QGroupBox(QString::fromLocal8Bit("摆位所见及建议")))
	, m_ZhenduanGrp(new QGroupBox(QString::fromLocal8Bit("影像所见及诊断意见")))
	, m_ReportInfoGrp(new QGroupBox(QString::fromLocal8Bit("报告信息")))
	, m_BaiweiLabel(new CustomLabel)
	, m_ZhenduanLabel(new CustomLabel)
	, m_BaiweiEdit(new QTextEdit)
	, m_ZhenduanEdit(new QTextEdit)
	, m_report_date_label(new QLabel(QString::fromLocal8Bit("报告日期:")))
	, m_report_doctor1_label(new QLabel(QString::fromLocal8Bit("报告医生:")))
	, m_report_doctor2_label(new QLabel(QString::fromLocal8Bit("审核医生:")))
	, m_report_date_edit(new QLineEdit)
	, m_report_doctor1_edit(new QLineEdit)
	, m_report_doctor2_edit(new QLineEdit)
	, m_preview_btn(new QPushButton(QString::fromLocal8Bit("报告预览")))
	, m_Radio0(new QRadioButton(QString::fromLocal8Bit("摆位报告")))
	, m_Radio2(new QRadioButton(QString::fromLocal8Bit("摆位/诊断报告")))
	, process(new QProcess)
	, reportMode(0)
{
	init_window();

	this->setAutoFillBackground(true);
	this->setObjectName("viewReportWidget");
	//this->setStyleSheet("#viewReportWidget{background:#00162c;border-width:0px;}");
	this->setStyleSheet("#viewReportWidget{background:#121212;border-width:0px;}");
}

ReportEditView::~ReportEditView()
{

}

void ReportEditView::init_window()
{
	//基本信息
	QGridLayout *infoLayout = new QGridLayout;
	infoLayout->addWidget(m_id_label, 0, 0, Qt::AlignRight);
	infoLayout->addWidget(m_id_edit, 0, 1, Qt::AlignCenter);
	infoLayout->addWidget(m_name_label, 0, 2, Qt::AlignRight);
	infoLayout->addWidget(m_name_edit, 0, 3, Qt::AlignCenter);
	//infoLayout->addWidget(m_type_label, 0, 4, Qt::AlignRight);
	//infoLayout->addWidget(m_type_edit, 0, 5, Qt::AlignCenter);

	//infoLayout->addWidget(m_breed_label, 1, 0, Qt::AlignRight);
	//infoLayout->addWidget(m_breed_edit, 1, 1, Qt::AlignCenter);
	infoLayout->addWidget(m_sex_label, 1, 0, Qt::AlignRight);
	infoLayout->addWidget(m_sex_edit, 1, 1, Qt::AlignCenter);
	infoLayout->addWidget(m_age_label, 1, 2, Qt::AlignRight);
	infoLayout->addWidget(m_age_edit, 1, 3, Qt::AlignCenter);

	//infoLayout->addWidget(m_body_label, 2, 0, Qt::AlignRight);
	//infoLayout->addWidget(m_body_edit, 2, 1, Qt::AlignCenter);
	infoLayout->addWidget(m_date_label, 2, 0, Qt::AlignRight);
	infoLayout->addWidget(m_date_edit, 2, 1, Qt::AlignCenter);
	infoLayout->addWidget(m_modality_label, 2, 2, Qt::AlignRight);
	infoLayout->addWidget(m_modality_edit, 2, 3, Qt::AlignCenter);

	infoLayout->setHorizontalSpacing(5);
	m_BasicInfoGrp->setLayout(infoLayout);

	//摆位
	/*m_BaiweiLabel->setPixmap(QPixmap("./images/blank.png"));
	QHBoxLayout *baiwei_hl = new QHBoxLayout;
	baiwei_hl->addWidget(m_BaiweiEdit, 5);
	baiwei_hl->addWidget(m_BaiweiLabel, 2);
	baiwei_hl->setSpacing(5);
	m_BaiweiGrp->setLayout(baiwei_hl);*/

	//诊断
	m_ZhenduanLabel->setPixmap(QPixmap("./images/blank.png"));
	QHBoxLayout *zhenduan_hl = new QHBoxLayout;
	zhenduan_hl->addWidget(m_ZhenduanEdit, 5);
	zhenduan_hl->addWidget(m_ZhenduanLabel, 2);
	zhenduan_hl->setSpacing(5);
	m_ZhenduanGrp->setLayout(zhenduan_hl);

	//报告信息
	QGridLayout *report_gl = new QGridLayout;
	report_gl->addWidget(m_report_date_label, 0, 0, Qt::AlignRight);
	report_gl->addWidget(m_report_date_edit, 0, 1, Qt::AlignCenter);
	report_gl->addWidget(m_report_doctor1_label, 0, 2, Qt::AlignRight);
	report_gl->addWidget(m_report_doctor1_edit, 0, 3, Qt::AlignCenter);
	//report_gl->addWidget(m_report_doctor2_label, 0, 4, Qt::AlignRight);
	//report_gl->addWidget(m_report_doctor2_edit, 0, 5, Qt::AlignCenter);
	QHBoxLayout* hl = new QHBoxLayout;

	//hl->addWidget(m_Radio0, 1);
	//hl->addWidget(m_Radio2, 1);
	m_preview_btn->setFixedWidth(180);
	hl->addWidget(m_preview_btn, 4, Qt::AlignRight);
	report_gl->addLayout(hl, 1, 0, 1, 6);
	m_ReportInfoGrp->setLayout(report_gl);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_BasicInfoGrp,1);
	//mainLayout->addWidget(m_BaiweiGrp,3);
	mainLayout->addWidget(m_ZhenduanGrp,4);
	mainLayout->addWidget(m_ReportInfoGrp,1);
	mainLayout->setSpacing(10);

	//style
	set_style();
	this->setLayout(mainLayout);
	
	//
	m_report_date_edit->setText(QDate::currentDate().toString("yyyy/MM/dd"));
	m_report_date_edit->setReadOnly(true);
	m_report_doctor1_edit->setText(QString::fromLocal8Bit("Kobe"));
	m_Radio0->setChecked(true);
	
	//signal-slots
	connect(m_Radio0, SIGNAL(clicked()), this, SLOT(RadioChanged()));
	//connect(m_Radio1, SIGNAL(clicked()), this, SLOT(RadioChanged()));
	connect(m_Radio2, SIGNAL(clicked()), this, SLOT(RadioChanged()));
	connect(m_preview_btn, SIGNAL(clicked()), this, SLOT(ReportPreview()));
}

void ReportEditView::ShowInformation(BasicInfo* info)
{
	BasicInfo *i = info;
	m_id_edit->setText(i->id);
	m_name_edit->setText(i->name);
	m_age_edit->setText(QString::number(i->age));
	m_sex_edit->setText(i->sex);
	m_body_edit->setText(i->body);
	m_date_edit->setText(i->date);
	m_modality_edit->setText(i->modality);
	delete i;
}

void ReportEditView::set_style()
{
	m_id_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);
	m_name_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);
	m_type_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);
	m_breed_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);
	m_sex_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);
	m_age_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);
	m_body_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);
	m_date_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);
	m_modality_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);
	m_id_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);
	m_name_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);
	m_type_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);
	m_breed_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);
	m_sex_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);
	m_age_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);
	m_body_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);
	m_date_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);
	m_modality_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);

	m_report_date_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);
	m_report_doctor1_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);
	m_report_doctor2_label->setStyleSheet(CCGLobalConsts::NORMAL_LABEL);

	m_report_date_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);
	m_report_doctor1_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);
	m_report_doctor2_edit->setStyleSheet(CCGLobalConsts::NORMAL_LINEEDIT);

	m_ReportInfoGrp->setStyleSheet(CCGLobalConsts::NORMAL_GROUPBOX);
	m_BasicInfoGrp->setStyleSheet(CCGLobalConsts::NORMAL_GROUPBOX);
	m_BaiweiGrp->setStyleSheet(CCGLobalConsts::NORMAL_GROUPBOX);
	m_ZhenduanGrp->setStyleSheet(CCGLobalConsts::NORMAL_GROUPBOX);

	m_Radio0->setStyleSheet(CCGLobalConsts::NORMAL_RADIO);
	//m_Radio1->setStyleSheet(CCGLobalConsts::NORMAL_RADIO);
	m_Radio2->setStyleSheet(CCGLobalConsts::NORMAL_RADIO);
	
}

#include <QDebug>
void ReportEditView::RadioChanged()
{
	if (sender() == m_Radio0)
	{
		reportMode = 0;
	}
	//else if (sender()==m_Radio1)
	//{
	//	reportMode = 1;
	//}
	else if (sender() == m_Radio2)
	{
		reportMode = 2;
	}
}

void ReportEditView::paintEvent(QPaintEvent *)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/************************************************************************/
/* 报告排版                                                              */
/************************************************************************/

void ReportEditView::ReportPreview()
{
	//QFile f("cache/report.pdf");
	//f.open(QIODevice::WriteOnly);
	//QPdfWriter* writer = new QPdfWriter(&f);
	//QPainter* p = new QPainter(writer);
	//writer->setPageSize(QPagedPaintDevice::A4);
	//QRect rect = p->viewport();
	//int WIDTH = rect.width();
	//int HEIGHT = rect.height();

	////表头
	//p->setFont(QFont("Arial", 20));
	//p->drawText(QRect(WIDTH *2/7 , HEIGHT/18, WIDTH / 2, HEIGHT / 12), QString::fromStdWString(L"上海慧龙动物医学影像中心"));//第一个页面  
	//p->setFont(QFont("Arial", 23, QFont::Bold));
	//p->drawText(QRect(WIDTH *2/ 5, HEIGHT / 11, WIDTH / 2, HEIGHT / 12), QString::fromStdWString(L"检查报告单"));//第一个页面  

	//p->setFont(QFont("Arial", 13));
	//p->drawText(QRect(WIDTH / 8, HEIGHT *2/14 , WIDTH / 7, HEIGHT / 14), QString::fromStdWString(L"编号: DR001"));
	//p->drawText(QRect(WIDTH * 3 / 8, HEIGHT *2/ 14, WIDTH / 7, HEIGHT / 14), QString::fromStdWString(L"阅片: ")+m_report_doctor1_edit->text());
	//p->drawText(QRect(WIDTH * 5 / 8, HEIGHT *2/ 14, WIDTH *2/ 7, HEIGHT / 14), QString::fromStdWString(L"日期: ")+m_report_date_edit->text());

	//p->setPen(QPen(QColor(0, 0, 0), 10));
	//p->drawLine(WIDTH / 9, HEIGHT *2 / 12, WIDTH * 8 / 9, HEIGHT *2/ 12);
	////基本信息
	//p->setFont(QFont("Arial", 18));
	//p->drawText(QRect(WIDTH * 1 / 8, HEIGHT * 4 / 24+200, WIDTH / 3, HEIGHT / 12), QString::fromLocal8Bit("名字: ") + m_name_edit->text());
	//p->drawText(QRect(WIDTH * 3 / 7, HEIGHT * 4 / 24+200, WIDTH / 5, HEIGHT / 12), QString::fromLocal8Bit("性别: ") + m_sex_edit->text());
	//p->drawText(QRect(WIDTH * 4 / 7, HEIGHT * 4 / 24+200, WIDTH / 5, HEIGHT / 12), QString::fromLocal8Bit("年龄: ") + m_age_edit->text());
	//p->drawText(QRect(WIDTH * 5 / 7, HEIGHT * 4 / 24+200, WIDTH / 5, HEIGHT / 12), QString::fromLocal8Bit("种类: ") + m_type_edit->text());
	//p->drawLine(WIDTH / 9, HEIGHT * 11 / 48 - 100, WIDTH * 8 / 9, HEIGHT * 11 / 48 - 100);
	////摆位/诊断
	//int start_y = HEIGHT * 11 / 48 ;
	//int start_x = WIDTH * 1 / 8;
	//draw_baiei_report(p, WIDTH, HEIGHT, start_x, start_y);
	//
	//start_x = WIDTH * 1 / 8;
	//if (reportMode == 2)//需要诊断报告
	//{
	//	draw_zhenduan_report(p, WIDTH, HEIGHT, start_x, start_y);
	//}
	//
	//delete p;
	//delete writer;
	//f.close();

	//emit GotoOtherTab(2);
	/////////////////
	//QString program_reader = "bin/reader.exe";
	//QStringList arguments_reader;
	//arguments_reader << "cache/report.pdf";

	//process->start(program_reader, arguments_reader);

	//emit ConvertPdf2Png();

}


void ReportEditView::draw_baiei_report(QPainter* p, int WIDTH, int HEIGHT, int& start_x, int& start_y)
{
	QImage img;
	QSize imgsize;
	QRect imgrect;
	float ratio;
	p->setFont(QFont("Arial", 18, QFont::Bold));
	p->drawText(QRect(start_x, start_y, WIDTH  / 3, HEIGHT / 12), QString::fromLocal8Bit("摆位所见及建议:\n"));
	start_y += HEIGHT / 30;
	int number = m_BaiweiLabel->image_number();
	QVector<QString> imageSet = m_BaiweiLabel->image_set();
	if (number == 1){
		start_x = WIDTH / 4;
		img.load(imageSet.at(0));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3, (float)WIDTH / 3 * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (float)WIDTH / 3 * ratio + 50;
	}
	else if (number == 2){
		start_x = WIDTH / 6;
		img.load(imageSet.at(0));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);

		img.load(imageSet.at(1));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(WIDTH / 2 + 50, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (WIDTH / 3 - 50) * ratio + 50;

	}
	else if (number == 3)
	{
		start_x = WIDTH / 6;
		img.load(imageSet.at(0));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);

		img.load(imageSet.at(1));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(WIDTH / 2 + 50, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (WIDTH / 3 - 50) * ratio + 50;

		img.load(imageSet.at(2));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (WIDTH / 3 - 50) * ratio + 50;
	}
	else if (number == 4)
	{
		start_x = WIDTH / 6;
		img.load(imageSet.at(0));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);

		img.load(imageSet.at(1));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(WIDTH / 2 + 50, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (WIDTH / 3 - 50) * ratio + 50;

		img.load(imageSet.at(2));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);

		img.load(imageSet.at(3));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(WIDTH / 2 + 50, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (WIDTH / 3 - 50) * ratio + 50;
	}
	else
	{
		start_y += 50;
		//draw nothing
	}
	p->setFont(QFont("Arial", 18));
	p->drawText(QRect(WIDTH * 1 / 8, start_y, WIDTH * 6 / 8, HEIGHT / 8), m_BaiweiEdit->toPlainText());
	start_y += HEIGHT / 8;
}

void ReportEditView::draw_zhenduan_report(QPainter* p, int WIDTH, int HEIGHT, int &start_x, int &start_y)
{
	QImage img;
	QSize imgsize;
	QRect imgrect;
	float ratio;
	p->setFont(QFont("Arial", 18, QFont::Bold));
	p->drawText(QRect(start_x, start_y, WIDTH /3 , HEIGHT / 12), QString::fromLocal8Bit("影像所见及诊断意见:\n"));
	start_y += HEIGHT / 30;
	int number = m_ZhenduanLabel->image_number();
	QVector<QString> imageSet = m_ZhenduanLabel->image_set();
	if (number == 1){
		start_x = WIDTH / 4;
		img.load(imageSet.at(0));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3, (float)WIDTH / 3 * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (float)WIDTH / 3 * ratio + 50;
	}
	else if (number == 2){
		start_x = WIDTH / 6;
		img.load(imageSet.at(0));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);

		img.load(imageSet.at(1));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(WIDTH / 2 + 50, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (WIDTH / 3 - 50) * ratio + 50;

	}
	else if (number == 3)
	{
		start_x = WIDTH / 6;
		img.load(imageSet.at(0));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);

		img.load(imageSet.at(1));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(WIDTH / 2 + 50, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (WIDTH / 3 - 50) * ratio + 50;

		img.load(imageSet.at(2));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (WIDTH / 3 - 50) * ratio + 50;
	}
	else if (number == 4)
	{
		start_x = WIDTH / 6;
		img.load(imageSet.at(0));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);

		img.load(imageSet.at(1));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(WIDTH / 2 + 50, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (WIDTH / 3 - 50) * ratio + 50;

		img.load(imageSet.at(2));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(start_x, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);

		img.load(imageSet.at(3));
		imgsize = img.size();
		ratio = (float)imgsize.height() / imgsize.width();
		imgrect = QRect(WIDTH / 2 + 50, start_y, WIDTH / 3 - 50, (float)(WIDTH / 3 - 50) * ratio);
		imgsize.scale(QSize(imgrect.width(), imgrect.height()), Qt::KeepAspectRatio);
		p->drawImage(imgrect, img);
		start_y += (WIDTH / 3 - 50) * ratio + 50;
	}
	else
	{
		start_y += 50;
		//draw nothing
	}
	p->setFont(QFont("Arial", 18));
	p->drawText(QRect(WIDTH * 1 / 8, start_y, WIDTH * 6 / 8, HEIGHT / 8), m_ZhenduanEdit->toPlainText());

}
