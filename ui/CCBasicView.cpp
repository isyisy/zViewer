#include "CCBasicView.h"
#include <SOIL/SOIL.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QDir>
#include <QDate>
#include <QTime>
#include <QDebug>

#define BORDER 0.999

CCBasicView::CCBasicView(QWidget *parent, QGLWidget* sharedWidget)
	: QGLWidget(parent,sharedWidget)
	, DeltaX(0)
	, DeltaY(0)
	, m_DrawMode(DrawingMode::HALT)//未画状态
	, m_LineRender(NULL)
	, m_RectRender(NULL)
	, m_EllipseRender(NULL)
	//, m_TextRender(NULL)
	, m_ActiveAreaObject(NULL)
	, ShowAreaObjects(true)
{
	global_scale_ratio = 1.0f;
	DragMode = true;
	ZoomMode = false;
	BrightnessMode = false;
	EllipseMode = false;
	RectangleMode = false;
	LengthMode = false;
	AngleMode = false;
	ArrowMode = false;
	TextMode = false;
	PencilMode = false;

	m_CornerPoint[0] = glm::vec3(-BORDER, -BORDER, 0);
	m_CornerPoint[1] = glm::vec3(BORDER, -BORDER, 0);
	m_CornerPoint[2] = glm::vec3(BORDER, BORDER, 0);
	m_CornerPoint[3] = glm::vec3(-BORDER, BORDER, 0);

}

CCBasicView::CCBasicView(QGLFormat& format, QWidget* parent, QGLWidget* sharedWidget /* = 0 */)
	: QGLWidget(format,parent,sharedWidget)
	, DeltaX(0)
	, DeltaY(0)
	, m_DrawMode(DrawingMode::HALT)//未画状态
	, m_LineRender(NULL)
	, m_RectRender(NULL)
	, m_EllipseRender(NULL)
	//, m_TextRender(NULL)
	, m_ActiveAreaObject(NULL)
	, ShowAreaObjects(true)
{
	global_scale_ratio = 1.0f;
	DragMode = true;
	ZoomMode = false;
	BrightnessMode = false;
	EllipseMode = false;
	RectangleMode = false;
	LengthMode = false;
	AngleMode = false;
	ArrowMode = false;
	TextMode = false;
	PencilMode = false;

	m_CornerPoint[0] = glm::vec3(-BORDER, -BORDER, 0);
	m_CornerPoint[1] = glm::vec3(BORDER, -BORDER, 0);
	m_CornerPoint[2] = glm::vec3(BORDER, BORDER, 0);
	m_CornerPoint[3] = glm::vec3(-BORDER, BORDER, 0);
}

CCBasicView::~CCBasicView()
{

}

void CCBasicView::initializeGL()
{
	m_LineRender = new CLine;
	m_RectRender = new CRectangle;
	m_EllipseRender = new CEllipse;
	//m_TextRender = new TextRenderer;
	//m_TextRender->init_resources();
}

void CCBasicView::resizeGL(int width, int height)
{
	window_width = width;
	window_height = height;
	window_hw_ratio = (float)height / (float)width;
	if (hw_ratio <= window_hw_ratio)
	{
		float a = -1;
		float b = 1;
		float c = -window_hw_ratio;
		float d = window_hw_ratio;
		P = glm::ortho(a, b, c, d);
		original_window_border = glm::vec4(a,b,c,d);
	}
	else
	{
		float a = -hw_ratio / window_hw_ratio;
		float b = hw_ratio / window_hw_ratio;
		float c = -hw_ratio;
		float d = hw_ratio;
		P = glm::ortho(a, b, c, d);
		original_window_border = glm::vec4(a, b, c, d);
	}
	DeltaX = 0;
	DeltaY = 0;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	updateGL();
}

void CCBasicView::draw()
{
	makeCurrent();
	glPointSize(4);
	//float sx = 2.0 / window_width;
	//float sy = 2.0 / window_height;
	//显示矩形或者椭圆标记
	if (ShowAreaObjects && !m_AreaObjectList.isEmpty())
	{
		QList<AreaObject*>::iterator it = m_AreaObjectList.begin();
		for (it; it != m_AreaObjectList.end(); ++it)
		{
			if (!(*it)->visible) continue;
			if ((*it)->object_type == AreaToolStyle::RECTANGLE_OBJ)
			{
				if ((*it)->isChecked)
					m_RectRender->set_color(0.8 * 255, 0.8 * 255, 0, 1);
				else
					m_RectRender->set_color(0, 0.8 * 255, 0, 1);
				m_RectRender->set_vertices((*it)->points);
				m_RectRender->RenderRec(glm::value_ptr(P*MV));
			}
			else if ((*it)->object_type==AreaToolStyle::ELLIPSE_OBJ)
			{
				if (!(*it)->isChecked)
					m_EllipseRender->set_color(0.8, 0.8, 0, 1);
				else
					m_EllipseRender->set_color(0, 0.8, 0, 1);
				m_EllipseRender->set_center(((*it)->points[0] + (*it)->points[2]) / 2.0f);
				float dist_a = glm::distance(glm::vec2((*it)->points[0]), glm::vec2((*it)->points[3]));
				float dist_b = glm::distance(glm::vec2((*it)->points[0]), glm::vec2((*it)->points[1]));
				m_EllipseRender->set_Radius(dist_a / 2.0f, dist_b / 2.0f);
				m_EllipseRender->computeCirclePoints();
				m_EllipseRender->RenderCircle(glm::value_ptr(P*MV), 1);
			}
		}
	}
	//显示其他
	//m_TextRender->set_color(1, 0, 0, 0.5);
	//m_TextRender->render_text("AAAAAAA", -1 + (window_width / 2)*sx - 4 * sx, -1 + 4 * sy, sx, sy);
}

void CCBasicView::paintGL()
{
	draw();
}

void CCBasicView::showFocus()
{
	makeCurrent();
	m_RectRender->set_color(0.8 * 255, 0.8 * 255, 0, 1);
	m_RectRender->set_vertices(m_CornerPoint);
	m_RectRender->RenderRec(glm::value_ptr(glm::mat4(1)));
}

void CCBasicView::mousePressEvent(QMouseEvent *event)
{
	glm::vec3 point;
	point.x = event->x();
	point.y = height() - event->y();
	point.z = 0;
	point = glm::unProject(point, MV, P, glm::ivec4(0, 0, width(), height()));

	if (DragMode)//拖拽模式
	{
		if (event->button() == Qt::LeftButton)
		{
			dragStartPosition = event->pos();
		}
	}

	if (ZoomMode)//缩放平移模式
	{
		panOldPoint = glm::vec2(point.x, point.y);
	}

	if (RectangleMode || EllipseMode)//Area模式
	{
		if (m_DrawMode == DrawingMode::HALT)
		{
			AreaToolStyle style = RectangleMode ? RECTANGLE_OBJ : ELLIPSE_OBJ;
			createAreaObject(style);
			m_ActiveAreaObject->points[0] = point;
			m_ActiveAreaObject->points[1] = point;
			m_ActiveAreaObject->points[2] = point;
			m_ActiveAreaObject->points[3] = point;
			m_DrawMode = DrawingMode::DRAWING;
		}
		if (!hasMouseTracking())
			setMouseTracking(true);
		else
		{
			m_DrawMode = DrawingMode::HALT;
			setMouseTracking(false);
		}
		updateGL();
	}

}

void CCBasicView::mouseReleaseEvent(QMouseEvent *event)
{

}
void CCBasicView::mouseMoveEvent(QMouseEvent *event)
{
	QPoint p;
	p = event->pos();
	glm::vec3 point;
	point.x = p.x();
	point.y = height() - p.y();
	point.z = 0;

	point = glm::unProject(point, MV, P, glm::ivec4(0, 0, width(), height()));

	if (ZoomMode)
	{
		this->setCursor(Qt::ArrowCursor);
		float deltax, deltay;
		deltax = point.x - panOldPoint.x;
		deltay = point.y - panOldPoint.y;
		DeltaX = DeltaX + deltax;
		DeltaY = DeltaY + deltay;
		P = glm::translate(P, glm::vec3(deltax, deltay, 0));
		panOldPoint.x = point.x - deltax;
		panOldPoint.y = point.y - deltay;
		updateGL();
	}

	if (RectangleMode || EllipseMode)
	{
		m_ActiveAreaObject->visible = true;
		m_ActiveAreaObject->points[2] = point;
		m_ActiveAreaObject->points[1] = glm::vec3(m_ActiveAreaObject->points[0].x, m_ActiveAreaObject->points[2].y, 0);
		m_ActiveAreaObject->points[3] = glm::vec3(m_ActiveAreaObject->points[2].x, m_ActiveAreaObject->points[0].y, 0);
		updateGL();
	}
}

void CCBasicView::wheelEvent(QWheelEvent *event)
{
	if (ZoomMode)//处理缩放
	{
		float margin, scale_ratio;
		margin = event->delta() / 8.0;
		margin = margin / 15.0;
		if (margin<0)
			scale_ratio = 0.9;
		else if (margin>0)
			scale_ratio = 1.1;
		else
			scale_ratio = 1;
		P = glm::translate(P, glm::vec3(-DeltaX, -DeltaY, 0));
		P = glm::scale(P, glm::vec3(scale_ratio, scale_ratio, 1));
		P = glm::translate(P, glm::vec3(DeltaX, DeltaY, 0));
		global_scale_ratio *= scale_ratio;
		updateGL();
	}	
}

unsigned char* CCBasicView::snap(int &w, int &h)
{
	makeCurrent();
	int width = this->width();
	int height = this->height();
	w = width;
	h = height;
	GLubyte *buf = new GLubyte[width*height*3];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buf);
	doneCurrent();
	/*	invert the image	*/
	for (int j = 0; j * 2 < height; ++j)
	{
		int index1 = j * width*3;
		int index2 = (height - 1 - j) * width*3;
		for (int i = width*3 ; i > 0; --i)
		{
			unsigned char temp = buf[index1];
			buf[index1] = buf[index2];
			buf[index2] = temp;
			++index1;
			++index2;
		}
	}
	bool end = SOIL_save_image("test.bmp", SOIL_SAVE_TYPE_BMP, width, height, 3, buf);
	//write2file(width, height, buf);
	return (unsigned char*)buf;
}

QString CCBasicView::snap2()
{
	QString path = QDir::currentPath();
	QDir dir(path);
	if (!dir.exists("snapshot"))
		dir.mkdir("snapshot");
	QString name = QTime::currentTime().toString("hhmmss");

	QString file_path = path + "/snapshot/" + name + ".bmp";
	return screen_shot(file_path);
}

QString CCBasicView::screen_shot(QString path)
{
	/*QRect rect = this->rect();
	QPoint p = this->pos();
	QPoint p2 = this->mapToGlobal(p);

	QScreen* screen = QApplication::primaryScreen();
	QPixmap pixmap = screen->grabWindow(QApplication::desktop()->winId(),
							p2.x(), p2.y(), rect.width(), rect.height());

	if (pixmap.save(path, "png"))
		return path;
	else
		return "";*/
	makeCurrent();
	int width = this->width();
	int height = this->height();
	GLubyte *buf = new GLubyte[width*height * 3];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buf);
	doneCurrent();
	/*	invert the image	*/
	for (int j = 0; j * 2 < height; ++j)
	{
		int index1 = j * width * 3;
		int index2 = (height - 1 - j) * width * 3;
		for (int i = width * 3; i > 0; --i)
		{
			unsigned char temp = buf[index1];
			buf[index1] = buf[index2];
			buf[index2] = temp;
			++index1;
			++index2;
		}
	}
	bool end = SOIL_save_image(path.toLocal8Bit().data(), SOIL_SAVE_TYPE_BMP, width, height, 3, buf);
	if (end)
		return path;
	else
		return "";
}

void CCBasicView::createAreaObject(AreaToolStyle style)
{
	AreaObject* obj = new AreaObject;
	obj->object_type = style;
	obj->visible = true;
	obj->text = "";
	obj->color = glm::vec4(0, 0.8, 0, 1);
	m_ActiveAreaObject = obj;
	m_AreaObjectList.push_back(obj);
}

#include <iostream>
#include <fstream>
void CCBasicView::write2file(int w, int h, unsigned char* pData, int bitdepth /* = 8 */)
{
	std::ofstream ofs("temp.txt");

	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			GLubyte temp = (GLubyte)pData[i*w + j];
			ofs << temp << " ";
		}
		ofs << endl;
	}
}