#ifndef CCMAINWIDGET_H
#define CCMAINWIDGET_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QStackedWidget>
#include "ui/DataWidget.h"
#include "ui/ViewWidget.h"
#include "ui/CCTabHeaderWidget.h"

class CCMainWidget : public QWidget
{
	Q_OBJECT

public:
	CCMainWidget(QWidget *parent);
	~CCMainWidget();
	QObject* getWindowInstance(){ return pWindow; }
signals:
	void CloseWindow();

public slots:
	void UpdateStackView(int index=0);
	
private:
	void init_window();

private:
	QStackedWidget* m_StackWidget;
	CCTabHeaderWidget* m_TabHeader;
	DataWidget* m_DataWidget;
	ViewWidget* m_ViewWidget;

	QObject* pWindow;

};

#endif // CCMAINWIDGET_H
