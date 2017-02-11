#include "MainView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QSplitter>
#include <QDockWidget>
#include <QSizePolicy>
#include "logic/SignalConverter.h"

#define BUTTONSIZE 35

MainView::MainView(QWidget *parent)
	: QWidget(parent)
	, m_WindowMode(WindowMode::SingleWindow)
{
	m_ToolBar = new CCToolBar(this);
	m_MuliView = new CCMultiView(this, 1, 1);
	init_window();
	this->setAutoFillBackground(true);
	this->setObjectName("MainView");
	this->setStyleSheet("#MainView{padding:0px;margin:0px;}");
}

MainView::MainView(QGLFormat& format, QWidget* parent, QGLWidget* sharedWidget)
	:QWidget(parent)
	, m_WindowMode(WindowMode::SingleWindow)
{
	m_ToolBar = new CCToolBar(this);
	m_MuliView = new CCMultiView(format, this, 1, 1, sharedWidget);
	init_window();
	this->setAutoFillBackground(true);
	this->setObjectName("MainView");
	this->setStyleSheet("#MainView{padding:0px;margin:0px;}");
}

MainView::~MainView()
{

}

void MainView::init_window()
{
	m_ToolsDropdown = new QToolButton;
	m_ToolsDropdown->setFixedSize(BUTTONSIZE, BUTTONSIZE);
	m_ToolsDropdown->setObjectName("tools_dropdown");
	
	m_WindwowChange = new QToolButton;
	m_WindwowChange->setFixedSize(BUTTONSIZE, BUTTONSIZE);
	m_WindwowChange->setObjectName("window_change");

	
	m_ToolBar->setFixedHeight(35);

	QHBoxLayout* toolsLayout = new QHBoxLayout;
	toolsLayout->addWidget(m_WindwowChange, 1, Qt::AlignLeft|Qt::AlignVCenter);
	toolsLayout->addStretch();
	toolsLayout->addWidget(m_ToolBar, 20, Qt::AlignRight);
	toolsLayout->addWidget(m_ToolsDropdown, 1, Qt::AlignRight|Qt::AlignVCenter);
	
	QGroupBox* toolsGrp = new QGroupBox;
	toolsGrp->setLayout(toolsLayout);
	toolsGrp->setStyleSheet("QGroupBox{background:black;border-style:none;margin:0px;}");


	m_Dictionary = new CCDictionary();
	reportEdit = new QTextEdit();

	reportEdit->setObjectName("reportEdit");
	reportEdit->setStyleSheet("QTextEdit{background:#626262;border-style:solid;border-color:#00162c;border-top-width:2px;border-right-width:1px;}");


	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addWidget(reportEdit, 3);
	hLayout->addWidget(m_Dictionary,1);
	hLayout->setSpacing(0);
	hLayout->setMargin(0);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(toolsGrp);
	layout->addWidget(m_MuliView, 12);
	//layout->addLayout(hLayout, 2);
	
	layout->setSpacing(0);
	layout->setContentsMargins(0,0,0,0);
	
	this->setLayout(layout);

	connect(m_ToolBar, SIGNAL(Operation(OPERATION_TYPE, OPERATION_MODE)), m_MuliView, SIGNAL(Operation(OPERATION_TYPE, OPERATION_MODE)));
	connect(this, SIGNAL(UpdateImageInfo(ImageInfo*,int)), m_MuliView, SIGNAL(UpdateImageInfo(ImageInfo*,int)));
	connect(m_MuliView, SIGNAL(NotifyAcceptThumb(QString, int)), this, SIGNAL(NotifyAcceptThumb(QString, int)));
	connect(m_WindwowChange, SIGNAL(clicked()), this, SLOT(change_window_layout()));
	connect(this, SIGNAL(LayoutChanged(int, int)), m_MuliView, SLOT(resetWindow(int, int)));
	connect(m_MuliView, SIGNAL(RetrieveDiagnosis(int,QString)), this, SLOT(RetrieveDiagnosis(int,QString)));
	connect(this, SIGNAL(DiagnosisOK(int, QString)), m_MuliView, SLOT(DiagnosisOK(int, QString)));
	//connect(m_Dictionary, SIGNAL(AppendEntries(QString)), this, SLOT(AppendEntries(QString)));
	connect(this, SIGNAL(ClearGLImageView(QList<int>)), m_MuliView, SLOT(ClearGLImageView(QList<int>)));

	connect(this, SIGNAL(EnableToolBar(bool)), m_ToolBar, SLOT(EnableToolbar(bool)));

	connect(SignalConverter::instance(), SIGNAL(PostIndex(QString,CustomLabel*)), this, SLOT(GetImageIndex(QString,CustomLabel*)));
	connect(this, SIGNAL(PostImage(QString,CustomLabel*)), SignalConverter::instance(), SIGNAL(GetImage(QString,CustomLabel*)));
}

void MainView::change_window_layout()
{
	//reportEdit->setText("");
	if (m_WindowMode == WindowMode::FourWindow)
	{
		m_WindowMode = WindowMode::SingleWindow;
		emit LayoutChanged(1, 1);
	}
	else if (m_WindowMode==WindowMode::SingleWindow)
	{
		m_WindowMode = WindowMode::TWOHORIZONAL;
		emit LayoutChanged(1, 2);
	}
	else if (m_WindowMode == WindowMode::TWOHORIZONAL)
	{
		m_WindowMode = WindowMode::TWOVERTICAL;
		emit LayoutChanged(2, 1);
	}
	else if (m_WindowMode == WindowMode::TWOVERTICAL)
	{
		m_WindowMode = WindowMode::FourWindow;
		emit LayoutChanged(2, 2);
	}
	emit SetWindowMode(m_WindowMode);
	emit EnableToolBar(false);
}

void MainView::AcceptSnapshot(QString name)
{
	//获取截图和诊断记录
	int index = name.toInt();
	QString pix_path = m_MuliView->getSnapDataFromImageView(index);

	QString str = reportEdit->toPlainText();
	SnapCache* cache = new SnapCache;
	cache->pixpath = pix_path;
	cache->diagnosis = str;
	emit Cache4Report(cache);
}

void MainView::GetImageIndex(QString name, CustomLabel* label)
{
	int index = name.toInt();
	QString pix_path = m_MuliView->getSnapDataFromImageView(index);
	emit PostImage(pix_path, label);
}

void MainView::AppendEntries(QString str)
{
	QString ori_str = reportEdit->toPlainText();
	ori_str += str;
	reportEdit->setText(ori_str);
}

void MainView::RetrieveDiagnosis(int ind, QString new_str)
{
	QString str = reportEdit->toPlainText();
	reportEdit->setText(new_str);
	emit DiagnosisOK(ind, str);//注意,此时的ind和str一定是不匹配的！！！
}

void MainView::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

