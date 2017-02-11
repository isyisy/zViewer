#include "CCCaheWidget.h"
#include <windows.h>
#include <QApplication>
#include <QStyleOption>
#include <QDebug>

CCCaheWidget::CCCaheWidget(QWidget *parent)
	: QWidget(parent)
{
	init_window();
	this->setAutoFillBackground(true);
	this->setObjectName("CCCaheWidget");
	//this->setStyleSheet("#CCCaheWidget{border-width:1px;border-style:solid;border-color:#00162c;}");
	this->setStyleSheet("#CCCaheWidget{border-width:1px;border-style:solid;border-color:#222222;}");
}

CCCaheWidget::~CCCaheWidget()
{

}

void CCCaheWidget::init_window()
{
	m_CloseButton = new QPushButton(this);
	m_CloseButton->setObjectName("delete_cache_btn");
	m_CloseButton->setFixedSize(20, 20);//尺寸不一定合适
	connect(m_CloseButton, SIGNAL(clicked()), this, SLOT(deleteCurretCache()));

	m_TitleGroup = new QGroupBox(this);
	m_TitleLabel = new QLabel("20151102",this);
	m_TitleLabel->setAlignment(Qt::AlignCenter);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addWidget(m_TitleLabel, 8);
	hLayout->addWidget(m_CloseButton, 1);
	hLayout->setSpacing(0);
	hLayout->setMargin(0);
	hLayout->setAlignment(m_CloseButton, Qt::AlignRight | Qt::AlignVCenter);

	m_TitleGroup->setLayout(hLayout);
	m_TitleLabel->setStyleSheet("QLabel{background:transparent;font-size:14px;}");
	m_TitleGroup->setStyleSheet("QGroupBox{background:#7e4027;border-style:none;margin:0px;}"
		"QGroupBox::checked{background:#ae4027;border-style:outset;border-color:green;border-width:2px;border-bottom-style:none;}");

	//image
	m_imageGroup = new QGroupBox(this);
	m_ImageLabel = new QLabel(this);
	m_ImageLabel->setFixedWidth(180);
	m_ImageLabel->setAlignment(Qt::AlignCenter);
	m_ImageLabel->setStyleSheet("QLabel{background:#222222;border-style:none;}");
	QHBoxLayout* hLayout2 = new QHBoxLayout;
	hLayout2->addWidget(m_ImageLabel);
	hLayout2->setMargin(0);
	hLayout2->setSpacing(0);
	m_imageGroup->setLayout(hLayout2);
	m_imageGroup->setStyleSheet("QGroupBox{background:transparent;border-style:none;margin:0px;}"
		"QGroupBox::checked{background:transparent;margin-bottom:2px;border-style:outset;border-color:green;border-width:2px;border-top-style:none;}");

	//diagnosis
	m_DiagnosisGroup = new QGroupBox(this);
	m_DiagnosisLabel = new QLabel("",this);
	m_DiagnosisLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

	QHBoxLayout* hLayout3 = new QHBoxLayout;
	hLayout3->addWidget(m_DiagnosisLabel);
	hLayout3->setSpacing(0);
	hLayout3->setMargin(0);

	m_DiagnosisGroup->setLayout(hLayout3);
	m_DiagnosisLabel->setStyleSheet("QLabel{background:transparent;font-size:14px;}");
	m_DiagnosisGroup->setStyleSheet("QGroupBox{background:#666666;border-style:none;margin:0px;}"
		"QGroupBox::checked{background:#ae4027;border-style:outset;border-color:green;border-width:2px;border-bottom-style:none;}");

	//main layout
	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_TitleGroup, 1);
	mainLayout->addWidget(m_imageGroup, 6);
	mainLayout->addWidget(m_DiagnosisGroup, 1);
	mainLayout->setSpacing(0);
	mainLayout->setMargin(0);

	this->setLayout(mainLayout);
	this->setMaximumHeight(220);//需要设置最大高度？

	m_ImageLabel->installEventFilter(this);
	m_TitleLabel->installEventFilter(this);
	m_DiagnosisLabel->installEventFilter(this);
}

void CCCaheWidget::setTitle(QString name)
{
	uid = name;
	m_TitleLabel->setText("");

}

void CCCaheWidget::setImage(QString path/* ="" */)
{
	QImage img(path);
	QImage image = img.scaled(QSize(180, 180), Qt::KeepAspectRatio);
	QPixmap pixmap = QPixmap::fromImage(image);
	m_ImageLabel->setPixmap(pixmap);
}

void CCCaheWidget::setDiagnosis(QString content/* ="" */)
{
	m_DiagnosisLabel->setText(content);
	str_diagnosis = content;
}

void CCCaheWidget::mousePressEvent(QMouseEvent* event)
{
	//qDebug() << "mouse pressed!";
}

void CCCaheWidget::paintEvent(QPaintEvent *e)
{

}

bool CCCaheWidget::eventFilter(QObject *obj, QEvent *ev)
{
	POINT p;
	QPoint pp;
	QMouseEvent *event;
	switch (ev->type())
	{
	case QEvent::MouseButtonPress:
		setMouseTracking(true);//十分必要，触发鼠标移动消息
		GetCursorPos(&p);
		pp.setX(p.x);
		pp.setY(p.y);
		event = new QMouseEvent(QEvent::MouseButtonPress, pp, Qt::LeftButton, 0, 0);
		QApplication::sendEvent(this, event);
		break;
	default:
		break;
	}
	return false;
}

void CCCaheWidget::deleteCurretCache()
{
	emit DeleteCache(uid);
}