#include "CCTabButtonWidget.h"
#include "globals/CCGlobalConsts.h"
#include <QEvent>
#include <QDebug>


CCTabButtonWidget::CCTabButtonWidget(QWidget *parent, int index)
: QWidget(parent)
, isChecked(false)
, titleName(QString::fromStdWString(L"测试"))
, imageUrls(":/images/report.png")
, mTabIndex(index)
{
	image = new QLabel;
	title = new QLabel;
	title->setText(titleName);
	title->setStyleSheet(CCGLobalConsts::QML_NORMAL_TAB_TITLE);
	
	QHBoxLayout* hlayout = new QHBoxLayout;
	hlayout->addWidget(image, 1);
	hlayout->addWidget(title, 2);
	grpBox = new QGroupBox;
	grpBox->setLayout(hlayout);
	grpBox->setStyleSheet(CCGLobalConsts::QML_NORMAL_TAB_BUTTON);


	grpBox->installEventFilter(this);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->addWidget(grpBox);
	mainLayout->setSpacing(5);
	mainLayout->setContentsMargins(0,0,0,0);

	this->setFixedSize(120, 55);//宽高比尽量接近2:1
	this->setLayout(mainLayout);

}

CCTabButtonWidget::~CCTabButtonWidget()
{

}

void CCTabButtonWidget::setTitleName(QString name)
{
	titleName = name;
	title->setText(titleName);
}

void CCTabButtonWidget::setImageUrls(QString urls)
{
	imageUrls = urls;
	image->setStyleSheet("QLabel{background:transparent;border-image:url(" + imageUrls + ");}");
}

void CCTabButtonWidget::setCheckedState(bool b)
{
	isChecked = b;
	if (isChecked)
	{
		grpBox->setStyleSheet(CCGLobalConsts::QML_CHECKED_TAB_BUTTON);
		title->setStyleSheet(CCGLobalConsts::QML_CHECKED_TAB_TITLE);
	}
	else
	{
		grpBox->setStyleSheet(CCGLobalConsts::QML_NORMAL_TAB_BUTTON);
		title->setStyleSheet(CCGLobalConsts::QML_NORMAL_TAB_TITLE);
	}
}

bool CCTabButtonWidget::eventFilter(QObject *obj, QEvent *event)
{
	switch (event->type())
	{
	case QEvent::MouseButtonPress:
		if (!isChecked)
		{
			isChecked = true;
			grpBox->setStyleSheet(CCGLobalConsts::QML_CHECKED_TAB_BUTTON);
			title->setStyleSheet(CCGLobalConsts::QML_CHECKED_TAB_TITLE);
			emit UpdateTab(mTabIndex);
		}
		break;
	case QEvent::Enter:
		if (!isChecked)
		{
			grpBox->setStyleSheet(CCGLobalConsts::QML_HOVER_TAB_BUTTON);
			title->setStyleSheet(CCGLobalConsts::QML_HOVER_TAB_TITLE);
		}
		break;
	case QEvent::Leave:
		if (!isChecked)
		{
			grpBox->setStyleSheet(CCGLobalConsts::QML_NORMAL_TAB_BUTTON);
			title->setStyleSheet(CCGLobalConsts::QML_NORMAL_TAB_TITLE);
		}
		break;

	default:
		break;
	}
	return false;
}
