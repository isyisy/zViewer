#include "CCImageView.h"
#include <iostream>
#include <QApplication>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QDrag>
#include <QGLFormat>

//#include <FTGL/ftgl.h>
//#include <FTGL/FTGLPixmapFont.h>

//FTGLPixmapFont  *freeTypeFont = NULL;

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

CCImageView::CCImageView(QWidget *parent, int ind)
	: CCBasicView(parent)
	, inFocus(false)
	, index(ind)
	, current_image_index(0)
	, texture(NULL)
	, textureExists(false)
	, shaderExists(false)
	, diagnosis_string("")
{
	setAcceptDrops(true);
	setFocusPolicy(Qt::StrongFocus);
}

CCImageView::CCImageView(QGLFormat& format, QWidget* parent, int ind, QGLWidget* sharedWidget)
	: CCBasicView(format,parent,sharedWidget)
	, index(ind)
	, inFocus(false)
	, current_image_index(0)
	, texture(NULL)
	, textureExists(false)
	, shaderExists(false)
	, diagnosis_string("")
{
	setAcceptDrops(true);
	setFocusPolicy(Qt::StrongFocus);
}

CCImageView::CCImageView(QWidget* parent, int ind, QGLWidget* sharedWidget /* = 0 */)
	:CCBasicView(parent,sharedWidget)
	, index(ind)
	, inFocus(ind == 0 ? true : false)
	//, inFocus(false)
	, texture(NULL)
	, current_image_index(0)
	, textureExists(false)
	, shaderExists(false)
	, diagnosis_string("")
{
	setAcceptDrops(true);
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
}

CCImageView::~CCImageView()
{
	if (textureExists)
		recycle_texture();
	if (shaderExists)
	{
		shader.DeleteShaderProgram();
		glDeleteBuffers(1, &vboVerticesID);
		glDeleteBuffers(1, &vboIndicesID);
		glDeleteBuffers(1, &vboTextureID);
		glDeleteVertexArrays(1, &vaoID);
	}
}


void CCImageView::initializeGL()
{
	glewExperimental = GL_TRUE;
	GLenum errorcode = glewInit();
	if (GLEW_OK != errorcode)
	{
		std::cout << errorcode << std::endl;
	}
	else
	{
		const GLubyte* version = glGetString(GL_VERSION);
		if (GLEW_VERSION_3_3)
		{
		}
	}
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	errorcode = glGetError();
	init_image();
	shaderExists = true;

	CCBasicView::initializeGL();

	//////////////////////////////////////////////////////////
	/*char* filePath = "C:\\WINDOWS\\Fonts\\times.ttf";
	freeTypeFont = new FTGLPixmapFont(filePath);
	if (freeTypeFont == NULL)
	cout << "failed to init font" << endl;
	else
	{
	freeTypeFont->FaceSize(32);
	freeTypeFont->CharMap(ft_encoding_unicode);
	}*/
}

void CCImageView::resizeGL(int w, int h)
{
	CCBasicView::resizeGL(w, h);
}

void CCImageView::draw()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!textureExists)
		return;
	glBindVertexArray(vaoID);//do not forget
	shader.Use();
	glUniform1f(shader("minvalue"),GLfloat(pImage->window_center-pImage->window_width/2));
	glUniform1f(shader("maxvalue"),GLfloat(pImage->window_center+pImage->window_width/2));
	glUniformMatrix4fv(shader("vMVP"), 1, GL_FALSE, glm::value_ptr(P*MV));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	shader.UnUse();
	glBindVertexArray(0);

	if (inFocus)
	{
		showFocus();
	}

	//ShowCornerInfo();

}

void CCImageView::paintGL()
{
	draw();
	CCBasicView::draw();
}

//slots
void CCImageView::UpdateImageInfo(ImageInfo* info,int ind)
{
	if (this->index != ind)
		return;
	if (textureExists)
		recycle_texture();
	ShowAreaObjects = true;

	pImage = info;
	hw_ratio = (float)pImage->height / (float)pImage->width;
	window_hw_ratio = (float)height() / (float)width();

	makeCurrent();// do not forget

	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//GL_CHECK_ERRORS//暂时注掉
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R16I, pImage->width, pImage->height, 0, GL_RED_INTEGER, GL_SHORT, pImage->pData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16,pImage->width, pImage->height, 0, GL_RED, GL_SHORT, pImage->pData);

	//reset vertex and texture tcoord
	vertices[0] = glm::vec2(-1, -hw_ratio);
	vertices[1] = glm::vec2(1, -hw_ratio);
	vertices[2] = glm::vec2(1, hw_ratio);
	vertices[3] = glm::vec2(-1, hw_ratio);
	tcoord[0] = glm::vec2(0, 0);
	tcoord[1] = glm::vec2(1, 0);
	tcoord[2] = glm::vec2(1, 1);
	tcoord[3] = glm::vec2(0, 1);

	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboTextureID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tcoord), tcoord);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (hw_ratio <= window_hw_ratio)
	{
		float a = -1;
		float b = 1;
		float c = -window_hw_ratio;
		float d = window_hw_ratio;
		P = glm::ortho(a, b, c, d);
		original_window_border = glm::vec4(a, b, c, d);
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
	textureExists = true;

	updateGL();
}

void CCImageView::ShowCornerInfo()
{
	float sx = 2.0 / width();
	float sy = 2.0 / height();
	int ww = (int)pImage->window_width;
	int wl = (int)pImage->window_center;
	
	//qDebug() << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&";
	//freeTypeFont->Render("YYASHDUIH");


	//m_TextRender->set_color(1, 1, 1, 1);
	//m_TextRender->render_text("ABCDE", 0, 0, sx, sy);
}

//用于自身更新当前层序
void CCImageView::updateSlice(bool next, int step)
{
	if (next)
	{
		pImage->pData += (pImage->width*pImage->height)*step;
	}
	else
	{
		pImage->pData -= (pImage->width*pImage->height)*step;
	}

	makeCurrent();
	glBindTexture(GL_TEXTURE_2D, textureID);
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, pImage->width, pImage->height, 0, GL_RED, GL_SHORT, pImage->pData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, pImage->width, pImage->height, 0, GL_RED, GL_SHORT, pImage->pData);
	doneCurrent();

	updateGL();
}

void CCImageView::init_image()
{
	makeCurrent();

	GL_CHECK_ERRORS
	//load shader
	shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/imageview.vert");
	shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/imageview.frag");
	//compile and link shader
	shader.CreateAndLinkProgram();
	shader.Use();
	//add attributes and uniforms
	shader.AddAttribute("vVertex");
	shader.AddAttribute("vTcoord");
	shader.AddUniform("textureMap");
	shader.AddUniform("vMVP");
	shader.AddUniform("minvalue");
	shader.AddUniform("maxvalue");
	shader.AddUniform("window_center");
	shader.AddUniform("window_width");
	//pass values of constant uniforms at initialization
	glUniform1i(shader("textureMap"), 0);
	shader.UnUse();

	GL_CHECK_ERRORS

	vertices[0] = glm::vec2(-1.0, -1.0);
	vertices[1] = glm::vec2(1.0, -1.0);
	vertices[2] = glm::vec2(1.0, 1.0);
	vertices[3] = glm::vec2(-1.0, 1.0);

	tcoord[0] = glm::vec2(0.0, 0.0);
	tcoord[1] = glm::vec2(1.0, 0.0);
	tcoord[2] = glm::vec2(1.0, 1.0);
	tcoord[3] = glm::vec2(0.0, 1.0);

	//fill quad indices array
	GLushort* id = &indices[0];
	*id++ = 0;
	*id++ = 1;
	*id++ = 2;
	*id++ = 0;
	*id++ = 2;
	*id++ = 3;

	GL_CHECK_ERRORS

		//setup quad vao and vbo stuff
		glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboVerticesID);
	glGenBuffers(1, &vboTextureID);
	glGenBuffers(1, &vboIndicesID);

	glBindVertexArray(vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	GL_CHECK_ERRORS
		//enable vertex attribute array for position
		glEnableVertexAttribArray(shader["vVertex"]);
	glVertexAttribPointer(shader["vVertex"], 2, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK_ERRORS

		glBindBuffer(GL_ARRAY_BUFFER, vboTextureID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tcoord), &tcoord[0], GL_STATIC_DRAW);
	GL_CHECK_ERRORS
		//enable vertex attribute array for position
		glEnableVertexAttribArray(shader["vTcoord"]);
	glVertexAttribPointer(shader["vTcoord"], 2, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK_ERRORS

		//pass quad indices to element array buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
	GL_CHECK_ERRORS

	glBindVertexArray(0);

	
}

void CCImageView::mousePressEvent(QMouseEvent *event)
{
	if (!inFocus)
	{
		emit FocusChanged(index, diagnosis_string);
	}
	if (!textureExists)
		return;
	if (BrightnessMode)//窗宽窗位
	{
		adjust_grayscale_point.x = event->x();
		adjust_grayscale_point.y = this->height()-event->y();
	}
	
	
	CCBasicView::mousePressEvent(event);
}

void CCImageView::mouseReleaseEvent(QMouseEvent *event)
{
	CCBasicView::mouseReleaseEvent(event);
}

void CCImageView::mouseMoveEvent(QMouseEvent *event)
{
	if (!textureExists)
		return;
	if (DragMode)
	{
		if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
			return;
		QDrag *drag = new QDrag(this);
		QMimeData *mimeData = new QMimeData;
		QByteArray info = QString("%1").arg(index).toLocal8Bit();
		mimeData->setData("custom/snap", info);
		drag->setPixmap(QPixmap(":/images/drag.png"));
		drag->setMimeData(mimeData);

		drag->start(Qt::MoveAction);
	}
	if (BrightnessMode)
	{
		glm::vec2 point = glm::vec2(event->x(), height() - event->y());
		float deltax = point.x - adjust_grayscale_point.x;
		float deltay = point.y - adjust_grayscale_point.y;
		pImage->window_width += deltax * 3;
		pImage->window_center += deltay * 3;
		pImage->window_width = (pImage->window_width < 1) ? 1 : pImage->window_width;
		adjust_grayscale_point = point;
		updateGL();
	}
	CCBasicView::mouseMoveEvent(event);
}

void CCImageView::wheelEvent(QWheelEvent *event)
{
	if (!textureExists)
		return;
	if (!ZoomMode)
	{
		float margin;
		int step = 1;
		margin = event->delta() / 8.0;
		margin = margin / 15.0;
		if (margin < 0)
		{
			current_image_index += step;
			if (current_image_index >= pImage->depth)
				current_image_index = pImage->depth - 1;
			else
				updateSlice(true);
		}
		else
		{
			current_image_index -= step;
			if (current_image_index <= 0)
				current_image_index = 0;
			else
				updateSlice(false);
		}			
	}
	CCBasicView::wheelEvent(event);
}

void CCImageView::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("custom/thumb"))
	{
		event->acceptProposedAction();
	}
}

void CCImageView::dropEvent(QDropEvent *event)
{
	QString str(event->mimeData()->data("custom/thumb"));
	emit NotifyAcceptThumb(str, this->index);
	emit FocusChanged(index, diagnosis_string);
	event->acceptProposedAction();
}

void CCImageView::SetOperationMode(OPERATION_TYPE type, OPERATION_MODE mode)
{
	//全部恢复默认值
	DragMode = true;
	ZoomMode = false;
	BrightnessMode = false;
	RectangleMode = false;
	EllipseMode = false;
	LengthMode = false;
	AngleMode = false;
	ArrowMode = false;
	TextMode = false;
	PencilMode = false;
	switch (type)
	{
	case DRAG://暂不设置，默认
		break;
	case BRIGHTNESS:
		BrightnessMode = mode;
		DragMode = !mode;
		break;
	case ZOOMPAN:
		ZoomMode = mode;
		DragMode = !mode;
		break;
	case ZOOMBACK:
		ZoomMode = false;
		DragMode = true;
		DeltaX = 0;
		DeltaY = 0;
		P = glm::ortho( original_window_border.x, 
						original_window_border.y, 
						original_window_border.z,
						original_window_border.w);
		updateGL();
		break;
	case LINEMEASURE:
		LengthMode = mode;
		DragMode = !mode;
		break;
	case ANGLEMEASURE:
		AngleMode = mode;
		DragMode = !mode;
		break;
	case RECTANGLE:
		RectangleMode = mode;
		DragMode = !mode;
		break;
	case ELLIPSE:
		EllipseMode = mode;
		DragMode = !mode;
		break;
	case DRAW:
		PencilMode = mode;
		DragMode = !mode;
		break;
	case ARROW:
		ArrowMode = mode;
		DragMode = !mode;
		break;
	case TEXT:
		break;
	default:
		break;
	}
}

void CCImageView::recycle_texture()
{
	delete pImage;
	pImage = NULL;
	textureExists = false;
	textureID = -1;
	hw_ratio = 1.0;
	inFocus = false;
	diagnosis_string = "";//清空诊断信息
	ShowAreaObjects = false;//放在此处不一定合适
	updateGL();
}