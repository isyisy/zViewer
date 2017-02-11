#include "CCSeriesWidget.h"
#include "globals/CCGlobalConsts.h"
#include <windows.h>
#include <QApplication>
#include <QStyleOption>
#include <QSize>
#include <QDebug>
#include <QPainter>

#define IMAGE_WIDTH 220

CCSeriesWidget::CCSeriesWidget(QWidget *parent, QString name, WindowMode mode)
	: QWidget(parent)
	, m_UniqueName(name)
	, m_PatientName("")
	, m_SeriesMode("")
	, m_SeriesTime("")
	, m_ImageData(NULL)
	, m_IsChecked(false)//初始化未选中
	, m_IsReady(false)//初始化数据未准备好
	, m_WindowMode(mode)
{
	init_window();
	this->setAutoFillBackground(true);
	this->setObjectName("CCSeriesWidget");
	this->setStyleSheet("#CCSeriesWidget{border-width:1px;border-style:solid;border-color:#00162c;}");
	setAcceptDrops(true);
}

CCSeriesWidget::~CCSeriesWidget()
{

}

void CCSeriesWidget::init_window()
{
	//newly added/////////////////////////////////
	m_CloseButton = new QPushButton(this);
	m_CloseButton->setObjectName("delete_series_btn");
	m_CloseButton->setFixedSize(30, 30);//尺寸不一定合适
	connect(m_CloseButton, SIGNAL(clicked()), this, SLOT(deleteCurretSeries()));

	//info group
	m_infoGroup = new QGroupBox(this);
	m_TimeInfoLabel = new QLabel("2013-04-16 14:09:00",this);
	m_TimeInfoLabel->setAlignment(Qt::AlignCenter);
	m_TimeInfoLabel->setStyleSheet("QLabel{background:transparent;font-size:14px;}");
	m_SeriesInfoLabel = new QLabel("CT: 1 series",this);
	m_SeriesInfoLabel->setAlignment(Qt::AlignCenter);
	m_SeriesInfoLabel->setStyleSheet("QLabel{background:transparent;font-size:14px;}");
	QVBoxLayout* vl1 = new QVBoxLayout;
	vl1->addWidget(m_TimeInfoLabel, 1, Qt::AlignBottom | Qt::AlignHCenter);
	vl1->addWidget(m_SeriesInfoLabel, 1, Qt::AlignCenter);
	vl1->setSpacing(0);
	vl1->setMargin(0);

	//newly added/////////////////////////////////
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->addLayout(vl1, 8);
	hLayout->addWidget(m_CloseButton, 1);
	hLayout->setSpacing(0);
	hLayout->setMargin(0);
	hLayout->setAlignment(m_CloseButton, Qt::AlignRight | Qt::AlignVCenter);
	//newly added/////////////////////////////////

	//m_infoGroup->setLayout(vl1);
	m_infoGroup->setLayout(hLayout);
	m_infoGroup->setStyleSheet("QGroupBox{background:#7e4027;border-style:none;margin:0px;}"
		"QGroupBox::checked{background:#ae4027;border-style:outset;border-color:green;border-width:2px;border-bottom-style:none;}");

	//series group
	m_imageGroup = new QGroupBox(this);
	m_ImageLabel = new QLabel(this);
	m_ImageLabel->setAlignment(Qt::AlignCenter);
	m_ImageLabel->setStyleSheet("QLabel{background:#222222;border-style:none;}");
		
	m_LoadProgress = new QProgressBar(this);
	m_LoadProgress->setStyleSheet(CCGLobalConsts::QML_PROGRESSBAR);
	m_LoadProgress->setFixedHeight(10);
	QVBoxLayout* vl2 = new QVBoxLayout;
	vl2->addWidget(m_ImageLabel, Qt::AlignHCenter);
	vl2->addWidget(m_LoadProgress, Qt::AlignBottom);
	vl2->setSpacing(0);
	vl2->setMargin(0);
	m_imageGroup->setLayout(vl2);
	m_imageGroup->setStyleSheet("QGroupBox{background:transparent;border-style:none;margin:0px;}"
		"QGroupBox::checked{background:transparent;margin-bottom:2px;border-style:outset;border-color:green;border-width:2px;border-top-style:none;}");

	//main layout
	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_infoGroup,1);
	mainLayout->addWidget(m_imageGroup,4);
	mainLayout->setSpacing(0);
	mainLayout->setMargin(0);
	
	this->setLayout(mainLayout);
	this->setMaximumHeight(200);//需要设置最大高度？

	m_ImageLabel->installEventFilter(this);
	m_TimeInfoLabel->installEventFilter(this);
	m_SeriesInfoLabel->installEventFilter(this);
}

void CCSeriesWidget::setLoadProgress(int range0/* =0 */, int range1/* =100 */, int pos/* =0 */)
{
	m_LoadProgress->setRange(range0, range1);
	m_LoadProgress->setValue(pos);
	m_LoadProgress->setFormat(QString("%1/%2").arg(pos).arg(range1));
}

void CCSeriesWidget::setSeriesInfo(QString patientName, QString seriesTime, QString seriesMode)
{
	m_PatientName = patientName;
	m_SeriesTime = seriesTime;
	m_SeriesMode = seriesMode;
	m_TimeInfoLabel->setText(m_SeriesTime);
	m_SeriesInfoLabel->setText(m_SeriesMode);
}

void CCSeriesWidget::setImageData(unsigned char* pData, int w, int h)
{
	m_ImageData = pData;
	image_width = w;
	image_height = h;
	QImage img(m_ImageData, image_width, image_height, QImage::Format_Grayscale8);
	QImage image = img.scaled(QSize(IMAGE_WIDTH,IMAGE_WIDTH), Qt::KeepAspectRatio);
	QPixmap pixmap = QPixmap::fromImage(image);
	m_ImageLabel->setPixmap(pixmap);
}

void CCSeriesWidget::deleteCurretSeries()
{
	emit DeleteSeries(this->m_UniqueName);
}

void CCSeriesWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CCSeriesWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		dragStartPosition = event->pos();
	}
	//多窗口取消鼠标点击事件
	//if (m_WindowMode != WindowMode::SingleWindow)
		//return;

	if (!m_IsChecked)
	{
		m_infoGroup->setChecked(true);
		m_imageGroup->setChecked(true);
		m_IsChecked = true;
		emit SetCheckedState(m_UniqueName);
	}
	
}

void CCSeriesWidget::mouseMoveEvent(QMouseEvent *event)
{
	//单窗口不需要拖动
	//if (m_WindowMode == WindowMode::SingleWindow)
		//return;
	if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
		return;
	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;
	QByteArray info = m_UniqueName.toLocal8Bit();
	mimeData->setData("custom/thumb", info);

	drag->setPixmap(QPixmap(":/images/drag.png"));
	drag->setMimeData(mimeData);
	drag->start(Qt::MoveAction);
}

void CCSeriesWidget::dragEnterEvent(QDragEnterEvent *event)
{
	event->acceptProposedAction();
}

void CCSeriesWidget::cancelCheckState()
{
	m_infoGroup->setChecked(false);
	m_imageGroup->setChecked(false);
	m_IsChecked = false;
}

void CCSeriesWidget::resetCheckState()
{
	//暂时全部取消掉
	/*m_infoGroup->setChecked(true);
	m_imageGroup->setChecked(true);
	m_IsChecked = true;*/
}

void CCSeriesWidget::resetWindowMode(WindowMode mode)
{
	this->m_WindowMode = mode;
	if (m_WindowMode != WindowMode::SingleWindow)
		cancelCheckState();
}

bool CCSeriesWidget::eventFilter(QObject *obj, QEvent *ev)
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
	case QEvent::MouseButtonRelease:
		setMouseTracking(false);
		GetCursorPos(&p);
		pp.setX(p.x);
		pp.setY(p.y);
		event = new QMouseEvent(QEvent::MouseButtonRelease, pp, Qt::LeftButton, 0, 0);
		QApplication::sendEvent(this, event);
		break;
	case QEvent::MouseMove:
		GetCursorPos(&p);
		pp.setX(p.x);
		pp.setY(p.y);
		event = new QMouseEvent(QEvent::MouseMove, pp, Qt::NoButton, 0, 0);//参考API，参数3必须是0，即Qt::NoButton
		QApplication::sendEvent(this, event);
		break;
	default:
		break;
	}
	return false;
}