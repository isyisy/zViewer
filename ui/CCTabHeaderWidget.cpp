#include "CCTabHeaderWidget.h"
#include <QGroupBox>
#include <QStyleOption>
#include <QPainter>
#include "CCMainWidget.h"

CCTabHeaderWidget::CCTabHeaderWidget(QWidget *parent)
	: QWidget(parent)
{
	pWindow = ((CCMainWidget*)parent)->getWindowInstance();
	init_window();
	this->setAutoFillBackground(true);
	this->setObjectName("CCTabHeaderWidget");
	this->setStyleSheet("#CCTabHeaderWidget{background:#313131;}");
	
}

CCTabHeaderWidget::~CCTabHeaderWidget()
{

}

void CCTabHeaderWidget::init_window()
{
	m_Data = new CCTabButtonWidget(this,0);
	m_Data->setCheckedState(true);
	m_Data->setImageUrls(":/images/data.png");
	m_Data->setTitleName(QString::fromStdWString(L"Êý¾Ý"));

	m_View = new CCTabButtonWidget(this,1);
	m_View->setCheckedState(false);
	m_View->setImageUrls(":/images/view.png");
	m_View->setTitleName(QString::fromStdWString(L"Ó°Ïñ"));

	QHBoxLayout* hLayout = new QHBoxLayout(this);//Tab group
	hLayout->addWidget(m_Data);
	hLayout->addWidget(m_View);
	hLayout->addStretch();
	hLayout->setSpacing(10);
	hLayout->setMargin(0);
	QGroupBox* mainGrp = new QGroupBox;
	mainGrp->setStyleSheet(QString("QGroupBox{background:#313131;border-style:none;margin:0px;}"));
	mainGrp->setLayout(hLayout);

	m_LogoBtn = new QToolButton;
	m_LogoBtn->setObjectName("logo_btn");
	m_LogoBtn->setFixedHeight(40);
	m_LogoBtn->setMinimumWidth(300);
	QHBoxLayout* hLayout1 = new QHBoxLayout;
	hLayout1->addWidget(m_LogoBtn);
	hLayout1->setAlignment(m_LogoBtn, Qt::AlignLeft);
	hLayout1->setSpacing(0);
	hLayout1->setContentsMargins(10, 0, 0, 0);
	QGroupBox* logoGrp = new QGroupBox;
	logoGrp->setLayout(hLayout1);
	logoGrp->setStyleSheet(QString("QGroupBox{background:#313131;border-style:none;margin:0px;}"));
	
	m_CloseWindow = new QToolButton;
	m_CloseWindow->setObjectName("close");
	m_CloseWindow->setFixedSize(40, 40);

	QHBoxLayout* hLayout2 = new QHBoxLayout;
	hLayout2->addWidget(m_CloseWindow);
	hLayout2->setAlignment(m_CloseWindow, Qt::AlignRight);
	
	hLayout2->setSpacing(10);
	hLayout2->setMargin(0);
	QGroupBox* menuGrp = new QGroupBox;
	menuGrp->setLayout(hLayout2);
	menuGrp->setStyleSheet(QString("QGroupBox{background:#313131;border-style:none;margin:0px;}"));
	QHBoxLayout* mainLayout = new QHBoxLayout;

	mainGrp->setMaximumWidth(320);
	mainLayout->addWidget(mainGrp,1);
	//mainLayout->addWidget(logoGrp,3);
	mainLayout->addWidget(menuGrp);


	logoGrp->installEventFilter(pWindow);
	m_LogoBtn->installEventFilter(pWindow);
	menuGrp->installEventFilter(pWindow);

	mainLayout->setAlignment(menuGrp, Qt::AlignRight);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	this->setLayout(mainLayout);

	connect(m_Data, SIGNAL(UpdateTab(int)), this, SLOT(UpdateTab(int)));
	connect(m_View, SIGNAL(UpdateTab(int)), this, SLOT(UpdateTab(int)));
	connect(m_CloseWindow, SIGNAL(clicked()), this, SIGNAL(CloseWindow()));
}

void CCTabHeaderWidget::UpdateTab(int index)
{
	if (index == 0)//Data
	{
		m_Data->setCheckedState(true);
		m_View->setCheckedState(false);
	}
	else if (index==1)//View
	{
		m_View->setCheckedState(true);
		m_Data->setCheckedState(false);
	}
	
	emit UpdataStackView(index);
}

void CCTabHeaderWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}