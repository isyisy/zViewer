#include "CCMultiView.h"

CCMultiView::CCMultiView(QWidget *parent, int r, int c)
	: QWidget(parent)
	, rows(r)
	, cols(c)
	, m_FocusWindowIndex(0)
{
	QGLFormat format;
	init_window(format);
}

CCMultiView::CCMultiView(QGLFormat& format, QWidget *parent, int r, int c, QGLWidget* sharedWidget)
	: QWidget(parent)
	, rows(r)
	, cols(c)
	, m_FocusWindowIndex(0)
{
	init_window(format, sharedWidget);
	m_format = format;
}

CCMultiView::~CCMultiView()
{

}

void CCMultiView::init_window(QGLFormat &format, QGLWidget* sharedWidget)
{
	m_grid = new QGridLayout(this);
	int index = 0;
	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			index = r*cols + c;
			CCImageView* phw = new CCImageView(format, this, index, sharedWidget);
			mViewList.push_back(phw);
			m_grid->addWidget(phw, r, c);
			connect(this, SIGNAL(UpdateImageInfo(ImageInfo*, int)), phw, SLOT(UpdateImageInfo(ImageInfo*, int)));
			connect(phw, SIGNAL(NotifyAcceptThumb(QString, int)), this, SIGNAL(NotifyAcceptThumb(QString, int)));
			connect(this, SIGNAL(Operation(OPERATION_TYPE, OPERATION_MODE)), phw, SLOT(SetOperationMode(OPERATION_TYPE, OPERATION_MODE)));
			connect(phw, SIGNAL(FocusChanged(int,QString)), this, SLOT(FocusChanged(int,QString)));
		}
	}

	m_grid->setSpacing(3);
	m_grid->setContentsMargins(0, 0, 0, 0);
	this->setLayout(m_grid);
}

void CCMultiView::resetWindow(int r, int c, QGLWidget* sharedWidget)
{
	if (rows == r && cols == c)
		return;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			m_grid->removeWidget(mViewList.at(i*cols + j));
		}
	}
	rows = r;
	cols = c;
	std::vector<CCImageView*>::iterator it = mViewList.begin();
	for (it; it != mViewList.end(); ++it)
	{
		if (*it != NULL) delete (*it);
	}
	if (!mViewList.empty()) mViewList.clear();
	mViewList.clear();
	//重新生成
	int index = 0;
	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			index = r*cols + c;
			CCImageView* phw = new CCImageView(m_format, this, index,sharedWidget);
			mViewList.push_back(phw);
			m_grid->addWidget(phw, r, c);
			connect(this, SIGNAL(UpdateImageInfo(ImageInfo*, int)), phw, SLOT(UpdateImageInfo(ImageInfo*, int)));
			connect(phw, SIGNAL(NotifyAcceptThumb(QString, int)), this, SIGNAL(NotifyAcceptThumb(QString, int)));
			connect(this, SIGNAL(Operation(OPERATION_TYPE, OPERATION_MODE)), phw, SLOT(SetOperationMode(OPERATION_TYPE, OPERATION_MODE)));
			connect(phw, SIGNAL(FocusChanged(int,QString)), this, SLOT(FocusChanged(int,QString)));
		}
	}
	m_grid->setSpacing(2);
	m_grid->setContentsMargins(0, 0, 0, 0);

	//重置
	m_FocusWindowIndex = 0;
}

void CCMultiView::ClearGLImageView(QList<int> list)
{
	QList<int>::iterator it = list.begin();
	for (it; it != list.end(); ++it)
	{
		CCImageView* pView = mViewList.at(*it);
		pView->recycle_texture();
	}
}

unsigned char* CCMultiView::getSnapDataFromImageView(int index, int& w, int& h)
{
	if (mViewList.empty())
		return NULL;
	if (index >= mViewList.size())
		return NULL;
	CCImageView* civ = mViewList.at(index);
	return civ->snap(w, h);
}

QString CCMultiView::getSnapDataFromImageView(int index)
{
	if (mViewList.empty())
		return NULL;
	if (index >= mViewList.size())
		return NULL;
	CCImageView* civ = mViewList.at(index);
	return civ->snap2();
}

void CCMultiView::FocusChanged(int ind, QString str)
{
	int temp = m_FocusWindowIndex;
	m_FocusWindowIndex = ind;
	emit RetrieveDiagnosis(temp,str);//注意,此时的ind和str一定是不匹配的！！！
}

void CCMultiView::DiagnosisOK(int index, QString str)
{
	if (index >= mViewList.size())
		return;
	CCImageView* civ_old = mViewList.at(index);
	civ_old = mViewList.at(index);
	civ_old->setDiagnosis(str);
	civ_old->setFocus(false);
	CCImageView* civ_new = mViewList.at(m_FocusWindowIndex);
	civ_new = mViewList.at(m_FocusWindowIndex);
	civ_new->setFocus(true);
}