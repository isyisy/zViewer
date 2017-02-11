#ifndef THUMBWIDGET_H
#define THUMBWIDGET_H

#include "globals/CCGlobalStructs.h"
#include "logic/CCDicomUtils.h"
#include "ui/CCSeriesWidget.h"
#include "logic/CCDataLoader.h"
#include <QGLWidget>
#include <QGLFormat>
#include <QPainter>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QPoint>
#include <QMap>
#include <QThread>
#include <QScrollArea>
#include <QMultiMap>
#include <QMenu>
#include <QContextMenuEvent>
#include <QCursor>
#include <QAction>

class ViewWidget;

class ThumbWidget : public QGLWidget
{
	Q_OBJECT

public:
	ThumbWidget(QWidget *parent=0);
	ThumbWidget(QGLFormat &format, QWidget *parent=0);
	~ThumbWidget();
signals:
	void UpdateImageInfo(ImageInfo*,int index=0);
	void BeginLoading(QList<QUrl>);
	void BeginLoading2(QList<QString>);
	void ClearGLImageView(QList<int>);//:需要清除的imageview序号
	void EnableToolBar(bool);

	void NewImagePathUpdate(QString);//发送到报告界面
	void RelayWindow();//发送到ViewWidget隐藏报告编辑模块
	void PostBasicInfo(BasicInfo*);

public slots:
	void UpdateCheckedSeries(QString name);
	void PrepareImageInfo(QString para1, int para2);//para1: series id; para2: view id
	void NewSeriesWindow(CCDicomUtils*);
	void UpdateProgress(QString, int, int, int);
	void FinishLoading(QString);
	void SetWindowMode(WindowMode);
	void DeleteSeries(QString name);

	void CheckReadyForUpdate(QList<QUrl>);
	void CheckReadyForUpdate2(QList<QString>);

	void relay_window();

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void mousePressEvent(QMouseEvent *);
	void dragEnterEvent(QDragEnterEvent *);
	void dropEvent(QDropEvent *);
	void contextMenuEvent(QContextMenuEvent *);
private:
	void init_window();
	void init_glew();
	void add_series(QString name);
	void importImages(QStringList fileList);
	void update_texture(QString name, int index=0);//index显示窗口id
	void get_petinfo(QString name);
	void relayout();

private:
	WindowMode m_WindowMode;
	CCSeriesWidget* pWorkingWidgetHandle;
	CCDicomUtils* pDicomUtilsHandle;
	CCDataLoader* pLoader;
	QThread* pLoaderThread;

	short* pData;

	QVBoxLayout* mainLayout;
	QPoint dragStartPosition;

	QMap<QString, CCDicomUtils*>   m_dicomUtilsMap; 
	QMap<QString, CCSeriesWidget*> m_seriesWidgetMap;

	QMultiMap<QString, int> m_ModelViewMap;//series与view映射,1:n

	bool textureUploaded;//
	QString m_NewImagePath;//新导入图像

	QAction* contextAction;
	bool report_hide;
};

#endif // THUMBWIDGET_H
