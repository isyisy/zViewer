#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include "globals/CCGlobalConsts.h"
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QVector>
#include <QPainter>
#include <QPen>

class CustomLabel : public QLabel
{
	Q_OBJECT

public:
	CustomLabel(QWidget *parent=0);
	~CustomLabel();
	int image_number(){ return m_ImageNumber; }
	QVector<QString> image_set(){ return m_ThumbPathList; }

public slots:
	void GetImage(QString path, CustomLabel*);
	
signals:
	void PostIndex(QString, CustomLabel*);
protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void paintEvent(QPaintEvent *);
private:
	void draw_blank();
	void draw_thumb();
private:
	int m_ImageNumber;
	QVector<QString> m_ThumbPathList;
};

#endif // CUSTOMLABEL_H
