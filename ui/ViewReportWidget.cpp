#include "ViewReportWidget.h"
#include "globals/CCGlobalConsts.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QPdfWriter>
#include <QProcess>
#include <QThread>
#include <QMessageBox>
#include <QDate>

ViewReportWidget::ViewReportWidget(QWidget *parent)
	: QWidget(parent)
{
	init_window();
	process = new QProcess(this);
	this->setAutoFillBackground(true);
	this->setObjectName("viewReportWidget");
	//this->setStyleSheet("#viewReportWidget{background:#00162c;border-width:0px;}");
	this->setStyleSheet("#viewReportWidget{background:#222222;border-width:0px;}");
	setAcceptDrops(true);
}

ViewReportWidget::~ViewReportWidget()
{

}

void ViewReportWidget::init_window()
{
	cacheLayout = new QVBoxLayout;

	m_ConvertReportBtn = new QPushButton(QString::fromStdWString(L"生成报告"));
	m_ConvertReportBtn->setFixedHeight(36);
	m_ConvertReportBtn->setStyleSheet("QPushButton{border-style:outset;font-size:16pt;font-family:Microsoft YaHei;text-align:center;margin:0px;padding:0px;}"
		"QPushButton::pressed{border-style:outset;font-size:16pt;font-family:Microsoft YaHei;text-align:center;margin-left:2px;margin-top:2px;padding:0px;}");

	mainLayout = new QVBoxLayout;
	mainLayout->addLayout(cacheLayout);
	mainLayout->addWidget(m_ConvertReportBtn);
	mainLayout->setAlignment(m_ConvertReportBtn, Qt::AlignBottom);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mainLayout);

	connect(m_ConvertReportBtn, SIGNAL(clicked()), this, SLOT(CreateReportPDF()));
}

void ViewReportWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ViewReportWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("custom/snap"))
	{
		event->acceptProposedAction();
	}
}

void ViewReportWidget::dropEvent(QDropEvent *event)
{
	QString str(event->mimeData()->data("custom/snap"));

	if (m_CacheWidgetMap.size() >= 3)
	{
		QMessageBox::information(NULL, QString::fromStdWString(L"提示"),
			QString::fromStdWString(L"已超过当前可添加的最大序列数目！"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		return;
	}

	emit NotifyAcceptSnapshot(str);
	event->acceptProposedAction();
}

void ViewReportWidget::AddCache(SnapCache* cache)
{
	QString path = cache->pixpath;
	if (path == "")
		return;
	QString conclusion = cache->diagnosis;

	CCCaheWidget* wgt = new CCCaheWidget;
	wgt->setTitle(path);
	wgt->setImage(path);
	wgt->setDiagnosis(QString::fromStdWString(L"结论:")+conclusion);
	if (m_CacheWidgetMap.empty())
	{
		cacheLayout->insertWidget(0,wgt);
		cacheLayout->addStretch(5000);
	}
	else
		cacheLayout->insertWidget(m_CacheWidgetMap.size(), wgt);
	m_CacheWidgetMap[path] = wgt;
	connect(wgt, SIGNAL(DeleteCache(QString)), this, SLOT(DeleteCache(QString)));

	mainLayout->setSpacing(0);
	this->setLayout(mainLayout);
}

void ViewReportWidget::DeleteCache(QString path)
{
	CCCaheWidget* dying_widget = m_CacheWidgetMap[path];
	cacheLayout->removeWidget(dying_widget);
	dying_widget->setGeometry(0, 0, 0, 0);
	m_CacheWidgetMap.remove(path);

}

void ViewReportWidget::CreateReportPDF()
{
	//QFile f("cache/report.pdf");
	//f.open(QIODevice::WriteOnly);
	//QPdfWriter* writer = new QPdfWriter(&f);
	//QPainter* p = new QPainter(writer);
	//writer->setPageSize(QPagedPaintDevice::A4);
	//QRect rect = p->viewport();
	//int WIDTH = rect.width();
	//int HEIGHT = rect.height();

	//QFont font0("Arial", 18);//正文

	////获取报告日期
	//QDate date = QDate::currentDate();
	//int year = date.year();
	//int month = date.month();
	//int day = date.day();
	//QString today = QString("%1/%2/%3").arg(year).arg(month).arg(day);

	////绘制标题
	//QFont font("Arial", 20,QFont::Bold);
	//p->setFont(font);
	//p->drawText(QRect(WIDTH/4, HEIGHT/14, WIDTH/2, HEIGHT/12), QString::fromStdWString(L"上海慧龙动物医学影像中心"));//第一个页面  
	//QFont font1("Arial", 13);
	//p->setFont(font1);
	//p->drawText(QRect(WIDTH / 7, HEIGHT / 8, WIDTH / 7, HEIGHT / 15), QString::fromStdWString(L"编号: 00000"));
	//p->drawText(QRect(WIDTH *3/ 7, HEIGHT / 8, WIDTH / 7, HEIGHT / 15), QString::fromStdWString(L"阅片: admin"));
	//p->drawText(QRect(WIDTH *5/ 7-100, HEIGHT / 8, WIDTH *2/ 7, HEIGHT / 15), QString::fromStdWString(L"日期: ")+today);

	//p->setPen(QPen(QColor(0, 0, 0), 10));
	//p->drawLine(WIDTH / 8, HEIGHT / 7, WIDTH * 7 / 8, HEIGHT / 7);

	//QMap<QString, CCCaheWidget*>::iterator it = m_CacheWidgetMap.begin();

	//int start_y = HEIGHT / 6;
	//int start_x = WIDTH / 3;
	//for (it; it != m_CacheWidgetMap.end(); ++it)
	//{
	//	QString path = it.key();
	//	QString text = it.value()->getDiagnosis();
	//	QImage img(path);
	//	QSize imgsize = img.size();
	//	float ratio = (float)imgsize.height() / imgsize.width();
	//	QRect rect = QRect(start_x, start_y, WIDTH / 3, (float)WIDTH / 3 * ratio);
	//	imgsize.scale(QSize(rect.width(), rect.height()), Qt::KeepAspectRatio);
	//	p->drawImage(rect, img);
	//	start_y += (float)WIDTH / 3 * ratio;
	//	start_y += 100;
	//	p->setFont(font0);
	//	p->drawText(QRect(WIDTH / 8, start_y, WIDTH * 6 / 8, HEIGHT / 20), text);
	//	start_y += HEIGHT / 20;

	//	if (start_y > HEIGHT)
	//	{
	//		writer->newPage();
	//		start_y = HEIGHT / 12;
	//	}
	//}

	//delete p;
	//delete writer;
	//f.close();

	//emit GotoOtherTab(2);
	/////////////////
	//QString program_reader = "bin/reader.exe";
	//QStringList arguments_reader;
	//arguments_reader << "cache/report.pdf";
	// 
	//process->start(program_reader, arguments_reader);

	//emit ConvertPdf2Png();
	
}