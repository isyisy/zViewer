#ifndef CCMULTIVIEW_H
#define CCMULTIVIEW_H

#include "globals/CCGlobalStructs.h"
#include <QWidget>
#include <QGridLayout>
#include <QList>
#include <vector>
#include "ui/CCImageView.h"

class CCMultiView : public QWidget
{
	Q_OBJECT

public:
	CCMultiView(QWidget *parent, int r=1, int c=1);//默认一行一列
	CCMultiView(QGLFormat& format, QWidget *parent, int r = 1, int c = 1, QGLWidget* sharedWidget=0);//默认一行一列
	~CCMultiView();

	unsigned char* getSnapDataFromImageView(int index, int& w, int& h);//w,h为传出
	QString getSnapDataFromImageView(int index);
	inline int getRowNumber(){ return rows; }
	inline int getColNumber(){ return cols; }
signals:
	void UpdateImageInfo(ImageInfo*,int index=0);//向ImageView发送
	void NotifyAcceptThumb(QString name, int index);//从ImageView接受
	void Operation(OPERATION_TYPE, OPERATION_MODE);
	void RetrieveDiagnosis(int ind=0,QString str="");//获取诊断信息，发送至MainView
public slots:
	void resetWindow(int r, int c, QGLWidget* sharedWidget = 0);
	void FocusChanged(int ind=0,QString str="");//由imageview发送而来
	void DiagnosisOK(int ind=0, QString str="");//由MainView发送而来
	void ClearGLImageView(QList<int>);
private:
	void init_window(QGLFormat& format, QGLWidget* sharedWidget = 0);

private:
	int cols;
	int rows;
	int m_FocusWindowIndex;
	QGLFormat m_format;
	QGridLayout* m_grid;
	std::vector<CCImageView*> mViewList;
};

#endif // CCMULTIVIEW_H
