#include "CustomLabel.h"

#include <QDebug>
#include <QMimeData>
#include "logic/SignalConverter.h"

#define MAX_THUMB_NUMBER 4

CustomLabel::CustomLabel(QWidget *parent)
	: QLabel(parent)
	, m_ImageNumber(0)
{
	this->setAcceptDrops(true);
	this->setStyleSheet(CCGLobalConsts::NORMAL_LABEL_IMAGE);

	connect(this, SIGNAL(PostIndex(QString, CustomLabel*)), SignalConverter::instance(), SIGNAL(PostIndex(QString,CustomLabel*)));
	connect(SignalConverter::instance(), SIGNAL(GetImage(QString,CustomLabel*)), this, SLOT(GetImage(QString,CustomLabel*)));
}

CustomLabel::~CustomLabel()
{

}

void CustomLabel::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("custom/snap"))
	{
		if (m_ImageNumber<MAX_THUMB_NUMBER)
			event->acceptProposedAction();
	}
}

void CustomLabel::GetImage(QString path, CustomLabel* label)
{
	if (label == this)
	{
		m_ThumbPathList.push_back(path);
		m_ImageNumber = m_ThumbPathList.size();
		update();
	}
	
}

void CustomLabel::dropEvent(QDropEvent *event)
{
	if (MAX_THUMB_NUMBER <= m_ImageNumber)
		return;

	QString str(event->mimeData()->data("custom/snap"));
	emit PostIndex(str, this);//
	event->acceptProposedAction();
}

void CustomLabel::paintEvent(QPaintEvent *event)
{
	if (m_ImageNumber == 0)
		draw_blank();
	else
		draw_thumb();
	
}

void CustomLabel::draw_blank()
{
	QPainter painter(this);
	QPen pen(QColor(180, 180, 180));
	pen.setWidth(2);
	painter.setPen(pen);

	float width = this->width();
	float height = this->height();
	//painter.drawPixmap(0, 0, width, height, QPixmap("./images/blank.png"));
	for (int i = 0; i < MAX_THUMB_NUMBER;i++)
		painter.drawPixmap(0, height/4*i, width, height/4, QPixmap("./images/blank.png"));
}

void CustomLabel::draw_thumb()
{
	QPainter painter(this);
	for (int i = 0; i < m_ImageNumber; i++)
	{
		painter.drawPixmap(0, height()/4*i, width(), height() / 4, QPixmap(m_ThumbPathList.at(i)));
	}
	//painter.drawRect(0, 0, width(), height());
	//switch (m_ImageNumber)
	//{
	//case 1:
	//	//painter.drawPixmap(0, 0, width(), height(), QPixmap(m_ThumbPathList.at(0)));
	//	painter.drawPixmap(0, 0, width(), height()/4, QPixmap(m_ThumbPathList.at(0)));
	//	painter.drawRect(0, 0, width(), height());
	//	break;
	//case 2:
	//	painter.drawPixmap(0, 0, width(), height() / 4, QPixmap(m_ThumbPathList.at(0)));
	//	painter.drawPixmap(0, height()/4, width(), height() / 4, QPixmap(m_ThumbPathList.at(1)));
	//	painter.drawRect(0, 0, width(), height());
	//	//painter.drawLine(0, height() / 2, width(), height() / 2);
	//	//painter.drawLine(width() / 2, 0, width() / 2, height());
	//	break;
	//case 3:
	//	painter.drawPixmap(0, 0, width() / 2, height() / 2, QPixmap(m_ThumbPathList.at(0)));
	//	painter.drawPixmap(width() / 2, 0, width()/2, height() / 2, QPixmap(m_ThumbPathList.at(1)));
	//	painter.drawPixmap(0, height()/2, width()/2, height()/2, QPixmap(m_ThumbPathList.at(2)));
	//	painter.drawRect(0, 0, width(), height());
	//	//painter.drawLine(0, height() / 2, width(), height() / 2);
	//	//painter.drawLine(width() / 2, 0, width() / 2, height());
	//	break;
	//case 4:
	//	painter.drawPixmap(0, 0, width() / 2, height() / 2, QPixmap(m_ThumbPathList.at(0)));
	//	painter.drawPixmap(width() / 2, 0, width()/2, height() / 2, QPixmap(m_ThumbPathList.at(1)));
	//	painter.drawPixmap(0, height() / 2, width() / 2, height()/2, QPixmap(m_ThumbPathList.at(2)));
	//	painter.drawPixmap(width()/2, height() / 2, width()/2, height()/2, QPixmap(m_ThumbPathList.at(3)));
	//	painter.drawRect(0, 0, width(), height());
	//	//painter.drawLine(0, height() / 2, width(), height() / 2);
	//	//painter.drawLine(width() / 2, 0, width() / 2, height());
	//	break;
	//default:
	//	break;
	//}


}