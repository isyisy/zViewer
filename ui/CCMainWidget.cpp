#include "CCMainWidget.h"

CCMainWidget::CCMainWidget(QWidget *parent)
	: QWidget(parent)
	, pWindow((QObject*)parent)
{
	init_window();
}

CCMainWidget::~CCMainWidget()
{

}

void CCMainWidget::init_window()
{
	m_DataWidget = new DataWidget(this);
	m_ViewWidget = new ViewWidget(this);

	m_StackWidget = new QStackedWidget(this);
	m_StackWidget->addWidget(m_DataWidget);
	m_StackWidget->addWidget(m_ViewWidget);
	m_StackWidget->setCurrentIndex(0);

	m_TabHeader = new CCTabHeaderWidget(this);
	m_TabHeader->setObjectName("CCTabHeaderWidget");
	m_TabHeader->setMinimumHeight(40);

	m_TabHeader->installEventFilter(pWindow);

	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(m_TabHeader);
	vLayout->addWidget(m_StackWidget);
	vLayout->setSpacing(0);
	vLayout->setMargin(0);

	this->setLayout(vLayout);

	//signals-slot between tab header and stacked view
	connect(m_TabHeader, SIGNAL(UpdataStackView(int)),this,SLOT(UpdateStackView(int)));
	connect(m_TabHeader, SIGNAL(CloseWindow()), this, SIGNAL(CloseWindow()));
	connect(m_DataWidget, SIGNAL(GotoOtherTab(int)), m_TabHeader, SLOT(UpdateTab(int)));
	connect(m_ViewWidget, SIGNAL(GotoOtherTab(int)), m_TabHeader, SLOT(UpdateTab(int)));
	connect(m_DataWidget, SIGNAL(ImportImages2View(QList<QUrl>)), m_ViewWidget, SIGNAL(ImportImages2View(QList<QUrl>)));
	connect(m_DataWidget, SIGNAL(ImportImages2View2(QList<QString>)), m_ViewWidget, SIGNAL(ImportImages2View2(QList<QString>)));
}


void CCMainWidget::UpdateStackView(int index)
{
	m_StackWidget->setCurrentIndex(index);

}