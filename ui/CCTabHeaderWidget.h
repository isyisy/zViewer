#ifndef CCTABHEADERWIDGET_H
#define CCTABHEADERWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include "ui/CCTabButtonWidget.h"

class CCMainWidget;

class CCTabHeaderWidget : public QWidget
{
	Q_OBJECT

public:
	CCTabHeaderWidget(QWidget *parent);
	~CCTabHeaderWidget();
public slots:
	void UpdateTab(int);

signals:
	void UpdataStackView(int);
	void CloseWindow();
protected:
	void paintEvent(QPaintEvent *e);
private:
	void init_window();

private:
	CCTabButtonWidget* m_Data;
	CCTabButtonWidget* m_View;

	QLabel* m_LogoLabel;
	QToolButton* m_LogoBtn;
	QToolButton* m_CloseWindow;

	QObject* pWindow;//´°¿Ú¾ä±ú
};

#endif // CCTABHEADERWIDGET_H
