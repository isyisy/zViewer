#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include "globals/CCGlobalStructs.h"
#include "ui/CCImageView.h"
#include "ui/ViewReportWidget.h"
#include "ui/ThumbWidget.h"
#include "ui/MainView.h"
#include "ui/ReportEditView.h"
#include <QtWidgets/QGridLayout>
#include <QWidget>

class ViewWidget : public QWidget
{
	Q_OBJECT

public:
	ViewWidget(QWidget *parent);
	~ViewWidget();
public slots:
	void relay_window();
protected:
	//bool eventFilter(QObject *, QEvent *);
signals:
	void OpenDicomFile(const char*);
	void OpenDicomDir(const char* dirPath);
	void ImportImages2View(QList<QUrl>);
	void ImportImages2View2(QList<QString>);
	void GotoOtherTab(int);
	void ConvertPdf2Png();

	void NewImagePathUpdate(QString);

private:
	void init_window();
	

private:
	MainView* mainWidget;
	ThumbWidget* thumbWidget;
	ReportEditView* reportEditWidget;
	ViewReportWidget* view2ReportWidget;
	QGridLayout* grid;

	bool report_hide;
};

#endif // VIEWWIDGET_H
