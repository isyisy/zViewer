#ifndef CCSERIESWIDGET_H
#define CCSERIESWIDGET_H

#include "globals/CCGlobalStructs.h"
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QImage>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QPoint>
#include <QPushButton>

class CCSeriesWidget : public QWidget
{
	Q_OBJECT

public:
	CCSeriesWidget(QWidget *parent, QString name, WindowMode mode=WindowMode::SingleWindow);
	~CCSeriesWidget();
public:
	void setSeriesInfo(QString patientName, QString seriesTime, QString seriesMode);
	void setImageData(unsigned char* pData, int w, int h);
	void setLoadProgress(int range0=0, int range1=100, int pos=0);
	void cancelCheckState();//取消
	void resetCheckState();//选中
	void resetWindowMode(WindowMode);
	inline bool getCheckedState(){ return m_IsChecked; }
	inline void setDataReady(){ m_IsReady = true; }//数据准备好

	inline QString getUniqueName(){ return m_UniqueName; }

signals:
	void SetCheckedState(QString);
	void DeleteSeries(QString);
public slots:
	void deleteCurretSeries();

protected:
	void paintEvent(QPaintEvent *e);
	void dragEnterEvent(QDragEnterEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
	bool eventFilter(QObject *, QEvent *);
private:
	void init_window();

private:
	WindowMode m_WindowMode;
	QString m_UniqueName;
	QPoint dragStartPosition;
	bool m_IsChecked;
	bool m_IsReady;


	QGroupBox* m_infoGroup;
	QLabel* m_TimeInfoLabel;//采集时间
	QLabel* m_SeriesInfoLabel;//Series模态、个数

	QString m_PatientName;
	QString m_SeriesTime;
	QString m_SeriesMode;
	unsigned char* m_ImageData;
	int image_width;
	int image_height;

	QPushButton* m_CloseButton;
	QGroupBox* m_imageGroup;
	QLabel* m_ImageLabel;
	QProgressBar* m_LoadProgress;//加载进度条
};

#endif // CCSERIESWIDGET_H
