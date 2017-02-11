#include "CCToolBar.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPainter>

#define TOOLBUTTON_SIZE 35


CCToolBar::CCToolBar(QWidget *parent)
	: QWidget(parent)
	, m_DragMode(true)//normal state
	, m_ZoomMode(false)
	, m_BrightnessMode(false)
	, m_LengthMode(false)
	, m_AngleMode(false)
	, m_RectangleMode(false)
	, m_EllipseMode(false)
	, m_PencilMode(false)
	, m_ArrowMode(false)
	, m_TextMode(false)
{
	init_window();
}

CCToolBar::~CCToolBar()
{

}

void CCToolBar::init_window()
{
	create_toolbar();
	setWindowOpacity(0.1);
	QHBoxLayout* vLayout = new QHBoxLayout;
	vLayout->addWidget(adjust_zoom_btn,1,Qt::AlignCenter);
	vLayout->addWidget(adjust_undo_btn, Qt::AlignCenter);
	vLayout->addWidget(adjust_window_btn, Qt::AlignCenter);
	
	vLayout->addStretch();

	//vLayout->addWidget(measure_length_btn, Qt::AlignCenter);
	//vLayout->addWidget(measure_angle_btn, Qt::AlignCenter);
	vLayout->addWidget(measure_area_btn, Qt::AlignCenter);
	vLayout->addWidget(measure_area2_btn, Qt::AlignCenter);
	vLayout->addStretch();

	//vLayout->addWidget(text_pencil_btn, Qt::AlignCenter);
	//vLayout->addWidget(text_arrow_btn, Qt::AlignCenter);
	//vLayout->addWidget(text_showhide_btn, Qt::AlignCenter);
	//vLayout->addStretch();
	vLayout->setSpacing(10);
	vLayout->setMargin(0);
	QGroupBox* grpBox = new QGroupBox;
	grpBox->setLayout(vLayout);
	grpBox->setStyleSheet("QGroupBox{background:black;border-style:none;margin:0px;}");

	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->addWidget(grpBox);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	this->setLayout(mainLayout);
	this->setFixedHeight(35);

	connect(adjust_zoom_btn, SIGNAL(clicked()), this, SLOT(function_zoom()));
	connect(adjust_window_btn, SIGNAL(clicked()), this, SLOT(function_brightness()));
	connect(adjust_undo_btn, SIGNAL(clicked()), this, SLOT(function_zoomback()));
	connect(measure_length_btn, SIGNAL(clicked()), this, SLOT(function_linemeasure()));
	connect(measure_angle_btn, SIGNAL(clicked()), this, SLOT(function_anglemeasure()));
	connect(measure_area_btn, SIGNAL(clicked()), this, SLOT(function_rectangle()));
	connect(measure_area2_btn, SIGNAL(clicked()), this, SLOT(function_ellipse()));
	connect(text_pencil_btn, SIGNAL(clicked()), this, SLOT(function_draw()));
	connect(text_arrow_btn, SIGNAL(clicked()), this, SLOT(function_arrow()));
	connect(text_showhide_btn, SIGNAL(clicked()), this, SLOT(function_text()));

}

void CCToolBar::EnableToolbar(bool b)
{
	if (b)
	{
		adjust_undo_btn->setEnabled(true);
		adjust_zoom_btn->setEnabled(true);
		adjust_window_btn->setEnabled(true);
		measure_area_btn->setEnabled(true);
		measure_area2_btn->setEnabled(true);
		measure_length_btn->setEnabled(true);
		measure_angle_btn->setEnabled(true);
		text_arrow_btn->setEnabled(true);
		text_pencil_btn->setEnabled(true);
		text_showhide_btn->setEnabled(true);
	}
	else
	{
		adjust_undo_btn->setEnabled(false);
		adjust_zoom_btn->setEnabled(false);
		adjust_window_btn->setEnabled(false);
		measure_area_btn->setEnabled(false);
		measure_area2_btn->setEnabled(false);
		measure_length_btn->setEnabled(false);
		measure_angle_btn->setEnabled(false);
		text_arrow_btn->setEnabled(false);
		text_pencil_btn->setEnabled(false);
		text_showhide_btn->setEnabled(false);
	}
}

void CCToolBar::create_toolbar()
{
	/*adjust related*/
	adjust_zoom_btn = new QToolButton(this);
	adjust_zoom_btn->setCheckable(true);
	adjust_zoom_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	adjust_zoom_btn->setObjectName("adjust_zoom_btn");
	adjust_zoom_btn->setToolTip(QString::fromStdWString(L"缩放"));
	adjust_zoom_btn->setCheckable(true);

	/*adjust_zoomback_btn = new QToolButton(this);
	adjust_zoomback_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	adjust_zoomback_btn->setObjectName("adjust_zoomback_btn");
	adjust_zoomback_btn->setToolTip(QString::fromStdWString(L"还原"));
	adjust_zoomback_btn->setCheckable(true);
	*/
	adjust_window_btn = new QToolButton(this);
	adjust_window_btn->setCheckable(true);
	adjust_window_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	adjust_window_btn->setObjectName("adjust_window_btn");
	adjust_window_btn->setToolTip(QString::fromStdWString(L"窗宽/窗位"));
	adjust_window_btn->setCheckable(true);

	adjust_undo_btn = new QToolButton(this);
	adjust_undo_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	adjust_undo_btn->setObjectName("adjust_undo_btn");
	adjust_undo_btn->setToolTip(QString::fromStdWString(L"缩放还原"));
	adjust_undo_btn->setCheckable(true);

	/*measure related*/
	measure_length_btn = new QToolButton(this);
	measure_length_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	measure_length_btn->setObjectName("measure_length_btn");
	measure_length_btn->setToolTip(QString::fromStdWString(L"长度测量"));
	measure_length_btn->setCheckable(true);

	measure_angle_btn = new QToolButton(this);
	measure_angle_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	measure_angle_btn->setObjectName("measure_angle_btn");
	measure_angle_btn->setToolTip(QString::fromStdWString(L"角度测量"));
	measure_angle_btn->setCheckable(true);

	measure_area_btn = new QToolButton(this);
	measure_area_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	measure_area_btn->setObjectName("measure_area_btn");
	measure_area_btn->setToolTip(QString::fromStdWString(L"矩形标记"));
	measure_area_btn->setCheckable(true);

	measure_area2_btn = new QToolButton(this);
	measure_area2_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	measure_area2_btn->setObjectName("measure_area2_btn");
	measure_area2_btn->setToolTip(QString::fromStdWString(L"椭圆标记"));
	measure_area2_btn->setCheckable(true);

	/*label related*/
	text_pencil_btn = new QToolButton(this);
	text_pencil_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	text_pencil_btn->setObjectName("text_pencil_btn");
	text_pencil_btn->setToolTip(QString::fromStdWString(L"手动绘制"));
	text_pencil_btn->setCheckable(true);

	text_arrow_btn = new QToolButton(this);
	text_arrow_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	text_arrow_btn->setObjectName("text_arrow_btn");
	text_arrow_btn->setToolTip(QString::fromStdWString(L"箭头标记"));
	text_arrow_btn->setCheckable(true);

	text_showhide_btn = new QToolButton(this);
	text_showhide_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	text_showhide_btn->setObjectName("text_showhide_btn");
	text_showhide_btn->setToolTip(QString::fromStdWString(L"隐藏标记"));
	text_showhide_btn->setCheckable(true);

	//下面的初始化代码有必要?,防止不同机器结果不同
	adjust_undo_btn->setEnabled(false);
	adjust_zoom_btn->setEnabled(false);
	adjust_window_btn->setEnabled(false);
	measure_area_btn->setEnabled(false);
	measure_area2_btn->setEnabled(false);
	measure_length_btn->setEnabled(false);
	measure_angle_btn->setEnabled(false);
	text_arrow_btn->setEnabled(false);
	text_pencil_btn->setEnabled(false);
	text_showhide_btn->setEnabled(false);

	//
	window_layout_btn = new QToolButton(this);
	window_layout_btn->setFixedSize(TOOLBUTTON_SIZE, TOOLBUTTON_SIZE);
	window_layout_btn->setObjectName("window_layout_btn");
	window_layout_btn->setToolTip(QString::fromStdWString(L"窗口数目"));

}

void CCToolBar::function_zoom()
{
	if (m_ZoomMode)
	{
		m_ZoomMode = false;
		m_DragMode = true;
		adjust_zoom_btn->setChecked(false);
		adjust_window_btn->setEnabled(true);
		measure_area_btn->setEnabled(true);
		measure_area2_btn->setEnabled(true);
		measure_length_btn->setEnabled(true);
		measure_angle_btn->setEnabled(true);
		text_arrow_btn->setEnabled(true);
		text_pencil_btn->setEnabled(true);
		text_showhide_btn->setEnabled(true);
		emit Operation(ZOOMPAN, OFF);
	}
	else
	{
		m_ZoomMode = true;
		m_DragMode = false;
		m_BrightnessMode = false;
		m_LengthMode = false;
		m_AngleMode = false;
		m_RectangleMode = false;
		m_EllipseMode = false;
		m_ArrowMode = false;
		m_TextMode = false;
		m_PencilMode = false;
		adjust_zoom_btn->setChecked(true);
		adjust_window_btn->setEnabled(false);
		measure_area_btn->setEnabled(false);
		measure_area2_btn->setEnabled(false);
		measure_length_btn->setEnabled(false);
		measure_angle_btn->setEnabled(false);
		text_arrow_btn->setEnabled(false);
		text_pencil_btn->setEnabled(false);
		text_showhide_btn->setEnabled(false);
		emit Operation(ZOOMPAN, ON);
	}
}

void CCToolBar::function_zoomback()
{
	m_ZoomMode = false;
	m_BrightnessMode = false;
	m_DragMode = true;
	adjust_zoom_btn->setEnabled(true);
	adjust_window_btn->setEnabled(true);
	measure_area_btn->setEnabled(true);
	measure_area2_btn->setEnabled(true);
	measure_length_btn->setEnabled(true);
	measure_angle_btn->setEnabled(true);
	text_arrow_btn->setEnabled(true);
	text_pencil_btn->setEnabled(true);
	text_showhide_btn->setEnabled(true);
	//
	adjust_zoom_btn->setChecked(false);
	adjust_window_btn->setChecked(false);
	measure_area_btn->setChecked(false);
	measure_area2_btn->setChecked(false);
	measure_length_btn->setChecked(false);
	measure_angle_btn->setChecked(false);
	text_arrow_btn->setChecked(false);
	text_pencil_btn->setChecked(false);
	text_showhide_btn->setChecked(false);

	emit Operation(ZOOMBACK, ON);//On与Off一样
}

void CCToolBar::function_brightness()
{
	if (m_BrightnessMode)
	{
		m_BrightnessMode = false;
		m_DragMode = true;
		adjust_window_btn->setChecked(false);
		adjust_zoom_btn->setEnabled(true);
		measure_area_btn->setEnabled(true);
		measure_area2_btn->setEnabled(true);
		measure_length_btn->setEnabled(true);
		measure_angle_btn->setEnabled(true);
		text_arrow_btn->setEnabled(true);
		text_pencil_btn->setEnabled(true);
		text_showhide_btn->setEnabled(true);
		emit Operation(BRIGHTNESS, OFF);
	}
	else
	{
		m_BrightnessMode = true;
		m_DragMode = false;
		m_ZoomMode = false;
		m_RectangleMode = false;
		m_EllipseMode = false;
		m_LengthMode = false;
		m_AngleMode = false;
		m_ArrowMode = false;
		m_TextMode = false;
		m_PencilMode = false;
		adjust_window_btn->setChecked(true);
		adjust_zoom_btn->setEnabled(false);
		measure_area_btn->setEnabled(false);
		measure_area2_btn->setEnabled(false);
		measure_length_btn->setEnabled(false);
		measure_angle_btn->setEnabled(false);
		text_arrow_btn->setEnabled(false);
		text_pencil_btn->setEnabled(false);
		text_showhide_btn->setEnabled(false);
		emit Operation(BRIGHTNESS, ON);
	}
}

void CCToolBar::function_linemeasure()
{
	if (m_LengthMode)
	{
		m_LengthMode = false;
		m_DragMode = true;
		measure_length_btn->setChecked(false);
		adjust_zoom_btn->setEnabled(true);
		adjust_window_btn->setEnabled(true);
		measure_area2_btn->setEnabled(true);
		measure_area_btn->setEnabled(true);
		measure_angle_btn->setEnabled(true);
		text_arrow_btn->setEnabled(true);
		text_pencil_btn->setEnabled(true);
		text_showhide_btn->setEnabled(true);
		emit Operation(LINEMEASURE, OFF);
	}
	else
	{
		m_LengthMode = true;
		m_DragMode = false;
		m_ZoomMode = false;
		m_BrightnessMode = false;
		m_EllipseMode = false;
		m_RectangleMode = false;
		m_AngleMode = false;
		m_ArrowMode = false;
		m_TextMode = false;
		m_PencilMode = false;
		measure_length_btn->setChecked(true);
		adjust_zoom_btn->setEnabled(false);
		adjust_window_btn->setEnabled(false);
		measure_area2_btn->setEnabled(false);
		measure_area_btn->setEnabled(false);
		measure_angle_btn->setEnabled(false);
		text_arrow_btn->setEnabled(false);
		text_pencil_btn->setEnabled(false);
		text_showhide_btn->setEnabled(false);
		emit Operation(LINEMEASURE, ON);
	}
}

void CCToolBar::function_anglemeasure()
{
	if (m_AngleMode)
	{
		m_AngleMode = false;
		m_DragMode = true;
		measure_angle_btn->setChecked(false);
		adjust_zoom_btn->setEnabled(true);
		adjust_window_btn->setEnabled(true);
		measure_area2_btn->setEnabled(true);
		measure_area_btn->setEnabled(true);
		measure_length_btn->setEnabled(true);
		text_arrow_btn->setEnabled(true);
		text_pencil_btn->setEnabled(true);
		text_showhide_btn->setEnabled(true);
		emit Operation(ANGLEMEASURE, OFF);
	}
	else
	{
		m_AngleMode = true;
		m_DragMode = false;
		m_ZoomMode = false;
		m_BrightnessMode = false;
		m_EllipseMode = false;
		m_RectangleMode = false;
		m_LengthMode = false;
		m_ArrowMode = false;
		m_TextMode = false;
		m_PencilMode = false;
		measure_angle_btn->setChecked(true);
		adjust_zoom_btn->setEnabled(false);
		adjust_window_btn->setEnabled(false);
		measure_area2_btn->setEnabled(false);
		measure_area_btn->setEnabled(false);
		measure_length_btn->setEnabled(false);
		text_arrow_btn->setEnabled(false);
		text_pencil_btn->setEnabled(false);
		text_showhide_btn->setEnabled(false);
		emit Operation(ANGLEMEASURE, ON);
	}
} 

void CCToolBar::function_rectangle()
{
	if (m_RectangleMode)
	{
		m_RectangleMode = false;
		m_DragMode = true;
		measure_area_btn->setChecked(false);
		adjust_zoom_btn->setEnabled(true);
		adjust_window_btn->setEnabled(true);
		measure_area2_btn->setEnabled(true);
		measure_length_btn->setEnabled(true);
		measure_angle_btn->setEnabled(true);
		text_arrow_btn->setEnabled(true);
		text_pencil_btn->setEnabled(true);
		text_showhide_btn->setEnabled(true);
		emit Operation(RECTANGLE, OFF);
	}
	else
	{
		m_RectangleMode = true;
		m_DragMode = false;
		m_ZoomMode = false;
		m_BrightnessMode = false;
		m_EllipseMode = false;
		m_LengthMode = false;
		m_AngleMode = false;
		m_ArrowMode = false;
		m_TextMode = false;
		m_PencilMode = false;
		measure_area_btn->setChecked(true);
		adjust_zoom_btn->setEnabled(false);
		adjust_window_btn->setEnabled(false);
		measure_area2_btn->setEnabled(false);
		measure_length_btn->setEnabled(false);
		measure_angle_btn->setEnabled(false);
		text_arrow_btn->setEnabled(false);
		text_pencil_btn->setEnabled(false);
		text_showhide_btn->setEnabled(false);
		emit Operation(RECTANGLE, ON);
	}
}

void CCToolBar::function_ellipse()
{
	if (m_EllipseMode)
	{
		m_EllipseMode = false;
		m_DragMode = true;
		measure_area2_btn->setChecked(false);
		adjust_zoom_btn->setEnabled(true);
		adjust_window_btn->setEnabled(true);
		measure_area_btn->setEnabled(true);
		measure_length_btn->setEnabled(true);
		measure_angle_btn->setEnabled(true);
		text_arrow_btn->setEnabled(true);
		text_pencil_btn->setEnabled(true);
		text_showhide_btn->setEnabled(true);
		emit Operation(ELLIPSE, OFF);
	}
	else
	{
		m_EllipseMode = true;
		m_DragMode = false;
		m_ZoomMode = false;
		m_BrightnessMode = false;
		m_RectangleMode = false;
		m_LengthMode = false;
		m_AngleMode = false;
		m_ArrowMode = false;
		m_TextMode = false;
		m_PencilMode = false;
		measure_area2_btn->setChecked(true);
		adjust_zoom_btn->setEnabled(false);
		adjust_window_btn->setEnabled(false);
		measure_area_btn->setEnabled(false);
		measure_length_btn->setEnabled(false);
		measure_angle_btn->setEnabled(false);
		text_arrow_btn->setEnabled(false);
		text_pencil_btn->setEnabled(false);
		text_showhide_btn->setEnabled(false);
		emit Operation(ELLIPSE, ON);
	}
}

void CCToolBar::function_draw()
{
	if (m_PencilMode)
	{
		m_PencilMode = false;
		m_DragMode = true;
		text_pencil_btn->setChecked(false);
		adjust_zoom_btn->setEnabled(true);
		adjust_window_btn->setEnabled(true);
		measure_area2_btn->setEnabled(true);
		measure_area_btn->setEnabled(true);
		measure_angle_btn->setEnabled(true);
		text_arrow_btn->setEnabled(true);
		measure_length_btn->setEnabled(true);
		text_showhide_btn->setEnabled(true);
		emit Operation(DRAW, OFF);
	}
	else
	{
		m_PencilMode = true;
		m_DragMode = false;
		m_ZoomMode = false;
		m_BrightnessMode = false;
		m_EllipseMode = false;
		m_RectangleMode = false;
		m_AngleMode = false;
		m_ArrowMode = false;
		m_TextMode = false;
		m_LengthMode = false;
		text_pencil_btn->setChecked(true);
		adjust_zoom_btn->setEnabled(false);
		adjust_window_btn->setEnabled(false);
		measure_area2_btn->setEnabled(false);
		measure_area_btn->setEnabled(false);
		measure_angle_btn->setEnabled(false);
		text_arrow_btn->setEnabled(false);
		measure_length_btn->setEnabled(false);
		text_showhide_btn->setEnabled(false);
		emit Operation(DRAW, ON);
	}
}

void CCToolBar::function_arrow()
{
	if (m_ArrowMode)
	{
		m_ArrowMode = false;
		m_DragMode = true;
		text_arrow_btn->setChecked(false);
		adjust_zoom_btn->setEnabled(true);
		adjust_window_btn->setEnabled(true);
		measure_area2_btn->setEnabled(true);
		measure_area_btn->setEnabled(true);
		measure_angle_btn->setEnabled(true);
		text_pencil_btn->setEnabled(true);
		measure_length_btn->setEnabled(true);
		text_showhide_btn->setEnabled(true);
		emit Operation(ARROW, OFF);
	}
	else
	{
		m_ArrowMode = true;
		m_DragMode = false;
		m_ZoomMode = false;
		m_BrightnessMode = false;
		m_EllipseMode = false;
		m_RectangleMode = false;
		m_AngleMode = false;
		m_PencilMode = false;
		m_TextMode = false;
		m_LengthMode = false;
		text_arrow_btn->setChecked(true);
		adjust_zoom_btn->setEnabled(false);
		adjust_window_btn->setEnabled(false);
		measure_area2_btn->setEnabled(false);
		measure_area_btn->setEnabled(false);
		measure_angle_btn->setEnabled(false);
		text_pencil_btn->setEnabled(false);
		measure_length_btn->setEnabled(false);
		text_showhide_btn->setEnabled(false);
		emit Operation(ARROW, ON);
	}
}

void CCToolBar::function_text()
{
	if (m_TextMode)
	{
		m_TextMode = false;
		m_DragMode = true;
		text_showhide_btn->setChecked(false);
		adjust_zoom_btn->setEnabled(true);
		adjust_window_btn->setEnabled(true);
		measure_area2_btn->setEnabled(true);
		measure_area_btn->setEnabled(true);
		measure_angle_btn->setEnabled(true);
		text_arrow_btn->setEnabled(true);
		measure_length_btn->setEnabled(true);
		text_pencil_btn->setEnabled(true);
		emit Operation(TEXT, OFF);
	}
	else
	{
		m_TextMode = true;
		m_DragMode = false;
		m_ZoomMode = false;
		m_BrightnessMode = false;
		m_EllipseMode = false;
		m_RectangleMode = false;
		m_AngleMode = false;
		m_ArrowMode = false;
		m_PencilMode = false;
		m_LengthMode = false;
		text_showhide_btn->setChecked(true);
		adjust_zoom_btn->setEnabled(false);
		adjust_window_btn->setEnabled(false);
		measure_area2_btn->setEnabled(false);
		measure_area_btn->setEnabled(false);
		measure_angle_btn->setEnabled(false);
		text_arrow_btn->setEnabled(false);
		measure_length_btn->setEnabled(false);
		text_pencil_btn->setEnabled(false);
		emit Operation(TEXT, ON);
	}
}

void CCToolBar::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	painter.setBrush(QBrush(QColor(0, 0, 0, 0)));
	painter.drawRect(this->rect());
}