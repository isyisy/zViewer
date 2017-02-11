#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "globals/CCGlobalStructs.h"
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include "ui/CCMultiView.h"
#include "ui/CCToolBar.h"
#include "ui/CCDictionary.h"
#include <QToolButton>

class CustomLabel;

class MainView : public QWidget
{
	Q_OBJECT

public:
	MainView(QWidget *parent);
	MainView(QGLFormat& format, QWidget* parent, QGLWidget* sharedWidget);
	~MainView();
signals:
	void Operation(OPERATION_TYPE, OPERATION_MODE);
	void LayoutChanged(int r, int c);
	void SetWindowMode(WindowMode);//设置窗口模式,与上面信号有点重复，以后再改
	void UpdateImageInfo(ImageInfo*,int index=0);//index表示发送的显示窗口序号
	void NotifyAcceptThumb(QString, int);
	void Cache4Report(SnapCache*);//发送截屏缓存
	void DiagnosisOK(int,QString);//在RetrieveDiagnosis中调用,发送回MultiVIew
	void ClearGLImageView(QList<int>);
	void EnableToolBar(bool);

	void PostImage(QString, CustomLabel*);//发送截图位置

public slots:
	void AcceptSnapshot(QString);
	void change_window_layout();
	void RetrieveDiagnosis(int ind=0,QString str="");//获取诊断信息,返回至MultiView
	void AppendEntries(QString);//将选中词条添加到诊断信息
	void GetImageIndex(QString, CustomLabel*);//获得截图序号
protected:
	void paintEvent(QPaintEvent *e);
	//void resizeEvent(QResizeEvent *);
private:
	void init_window();
private:
	CCToolBar* m_ToolBar;
	CCMultiView* m_MuliView;
	QTextEdit* reportEdit;
	CCDictionary* m_Dictionary;

	QToolButton* m_WindwowChange;
	QToolButton* m_ToolsDropdown;

	WindowMode m_WindowMode;
};

#endif // MAINVIEW_H
