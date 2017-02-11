#include "ViewWidget.h"
#include <QHBoxLayout>
#include <QGLFormat>
#include <QScrollArea>

ViewWidget::ViewWidget(QWidget *parent)
	: QWidget(parent)
	, report_hide(false)
{
	init_window();
}

ViewWidget::~ViewWidget()
{
}

void ViewWidget::init_window()
{
	QGLFormat format;
	format.setVersion(3, 3);
	format.setProfile(QGLFormat::CoreProfile);
	format.setDepth(true);
	format.setDoubleBuffer(true);
	format.setSamples(2);
	format.setRgba(true);

	thumbWidget = new ThumbWidget(format);
	thumbWidget->setObjectName("thumbWidget");
	thumbWidget->installEventFilter(this);
	thumbWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	mainWidget = new MainView(format,this,thumbWidget);//ÉèÖÃsharedWidget

	reportEditWidget = new ReportEditView(this);

	view2ReportWidget = new ViewReportWidget(this);

	grid = new QGridLayout(this);
	grid->addWidget(thumbWidget,0, 0);
	grid->addWidget(mainWidget, 0, 1);
	grid->addWidget(reportEditWidget, 0, 2);
	//grid->addWidget(view2ReportWidget, 0, 3);
	grid->setColumnStretch(1, 5);
	grid->setColumnStretch(2, 2);
	thumbWidget->setFixedWidth(220);
	view2ReportWidget->setFixedWidth(220);

	grid->setMargin(0);
	grid->setHorizontalSpacing(0);
	grid->setVerticalSpacing(5);
	grid->setContentsMargins(0,0,0,0);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addLayout(grid);
	this->setLayout(layout);

	connect(thumbWidget, SIGNAL(UpdateImageInfo(ImageInfo*,int)), mainWidget, SIGNAL(UpdateImageInfo(ImageInfo*,int)));
	connect(mainWidget, SIGNAL(NotifyAcceptThumb(QString, int)), thumbWidget, SLOT(PrepareImageInfo(QString,int)));
	connect(view2ReportWidget, SIGNAL(NotifyAcceptSnapshot(QString)), mainWidget, SLOT(AcceptSnapshot(QString)));
	connect(mainWidget, SIGNAL(Cache4Report(SnapCache*)), view2ReportWidget, SLOT(AddCache(SnapCache*)));
	connect(mainWidget, SIGNAL(SetWindowMode(WindowMode)), thumbWidget, SLOT(SetWindowMode(WindowMode)));

	connect(thumbWidget, SIGNAL(ClearGLImageView(QList<int>)), mainWidget, SIGNAL(ClearGLImageView(QList<int>)));
	connect(this, SIGNAL(ImportImages2View(QList<QUrl>)), thumbWidget, SLOT(CheckReadyForUpdate(QList<QUrl>)));
	connect(this, SIGNAL(ImportImages2View2(QList<QString>)), thumbWidget, SLOT(CheckReadyForUpdate2(QList<QString>)));
	connect(thumbWidget, SIGNAL(EnableToolBar(bool)), mainWidget, SIGNAL(EnableToolBar(bool)));

	connect(reportEditWidget, SIGNAL(GotoOtherTab(int)), this, SIGNAL(GotoOtherTab(int)));
	connect(reportEditWidget, SIGNAL(ConvertPdf2Png()), this, SIGNAL(ConvertPdf2Png()));

	connect(thumbWidget, SIGNAL(NewImagePathUpdate(QString)), this, SIGNAL(NewImagePathUpdate(QString)));
	connect(thumbWidget, SIGNAL(RelayWindow()), this, SLOT(relay_window()));

	connect(thumbWidget, SIGNAL(PostBasicInfo(BasicInfo*)), reportEditWidget, SLOT(ShowInformation(BasicInfo*)));
}

//#include <QDebug>
//bool ViewWidget::eventFilter(QObject *obj, QEvent *event)
//{
//	if (obj->objectName() == "thumbWidget")
//	{
//		if (event->type() == QEvent::MouseButtonPress)
//		{
//			QMouseEvent *mouseEvent = static_cast<QMouseEvent *> (event);
//			if (mouseEvent->button()==Qt::RightButton)
//			{
//				relay_window();
//			}
//		}
//	}
//	return false;
//}

void ViewWidget::relay_window()
{
	if (report_hide)
	{
		report_hide = false;
		reportEditWidget->show();
		grid->setColumnStretch(1, 5);
		grid->setColumnStretch(2, 2);
		this->setLayout(grid);
	}
	else
	{
		report_hide = true;
		grid->setColumnStretch(1, 400);
		grid->setColumnStretch(2, 1);
		reportEditWidget->hide();
		this->setLayout(grid);
	}
}