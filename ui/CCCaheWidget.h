#ifndef CCCAHEWIDGET_H
#define CCCAHEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>

class CCCaheWidget : public QWidget
{
	Q_OBJECT

public:
	CCCaheWidget(QWidget *parent=0);
	~CCCaheWidget();

	void setTitle(QString);
	void setImage(QString path="");
	void setDiagnosis(QString content="");
	inline QString getDiagnosis(){ return str_diagnosis; }
signals:
	void DeleteCache(QString);//´«³öpath
public slots:
	void deleteCurretCache();
protected:
	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent *);
	bool eventFilter(QObject *, QEvent *);
private:
	void init_window();

private:
	QString uid;

	QString str_diagnosis;

	QLabel* m_TitleLabel;
	QLabel* m_ImageLabel;
	QLabel* m_DiagnosisLabel;
	QPushButton* m_CloseButton;
	QGroupBox* m_TitleGroup;
	QGroupBox* m_imageGroup;
	QGroupBox* m_DiagnosisGroup;
};

#endif // CCCAHEWIDGET_H
