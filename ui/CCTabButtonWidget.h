#ifndef CCTABBUTTONWIDGET_H
#define CCTABBUTTONWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QStaticText>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>

class CCTabButtonWidget : public QWidget
{
	Q_OBJECT

public:
	CCTabButtonWidget(QWidget *parent, int index=0);//index:±Í«©–Ú∫≈
	~CCTabButtonWidget();

	inline QString getTitleName(){ return titleName; }
	inline QString getImageUrls(){ return imageUrls; }
	inline bool getCheckedState(){ return isChecked; }
	void setTitleName(QString name);
	void setImageUrls(QString urls);
	void setCheckedState(bool b);
signals:
	void UpdateTab(int);

protected:
	bool eventFilter(QObject *, QEvent *);

private:
	QLabel* image;
	QLabel* title;
	QGroupBox* grpBox;
	bool isChecked;
	QString titleName;
	QString imageUrls;
	int mTabIndex;
};

#endif // CCTABBUTTONWIDGET_H
