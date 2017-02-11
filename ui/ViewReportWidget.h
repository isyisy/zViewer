#ifndef VIEWREPORTWIDGET_H
#define VIEWREPORTWIDGET_H

#include "globals/CCGlobalStructs.h"
#include <QWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QList>
#include <QMap>
#include <QLabel>
#include <QProcess>
#include <QVBoxLayout>
#include "CCCaheWidget.h"

struct CacheWidget
{
	QLabel* pImage;
	SnapCache* pInfo;
};

class ViewReportWidget : public QWidget
{
	Q_OBJECT

public:
	ViewReportWidget(QWidget *parent);
	~ViewReportWidget();
signals:
	void NotifyAcceptSnapshot(QString name);
	void GotoOtherTab(int index);//跳转到其他Tab页
	void ConvertPdf2Png();
public slots:
	void AddCache(SnapCache*);//添加截屏cache
	void DeleteCache(QString);
	void CreateReportPDF();
protected:
	void paintEvent(QPaintEvent*);
	void dragEnterEvent(QDragEnterEvent *);
	void dropEvent(QDropEvent *);
private:
	void init_window();
private:
	QGroupBox* m_ReportGroup;
	QPushButton* m_ConvertReportBtn;

	QList<CacheWidget*> m_CacheList;
	QMap<QString, CCCaheWidget*> m_CacheWidgetMap;
	QVBoxLayout* mainLayout;
	QVBoxLayout* cacheLayout;

	QProcess *process;
};

#endif // VIEWREPORTWIDGET_H
