#pragma  once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <QString>
#include <QList>

struct Series
{
	QString series_instance_uid;
	QString series_number;
	QString series_modality;
	QString series_description;
	QString series_image_orientation;
	QString series_image_number;//不是来自DIcom头
};

struct Study
{
	QString study_instance_uid;
	QString study_accession_number;
	QString study_date;
	QString study_description;
	QString study_bodypart;
};


struct Patient
{
	QString patient_id;
	QString patient_name;
	QString patient_sex;
	QString patient_birthday;
};

struct Texture2D
{
	GLuint textureId;
	short* pData;
	int width;
	int height;
	float spacingX;
	float spacingY;
	float maxgray;
	float mingray;
};

struct ImageInfo
{
	int width;
	int height;
	int depth;//图像数目
	float mingray;
	float maxgray;
	float window_center;
	float window_width;
	short* pData;
};

struct BasicInfo
{
	QString id;
	QString name;
	QString sex;
	int age;
	QString body;
	QString date;
	QString modality;
	BasicInfo(){
		id = "";
		name = "";
		date = "";
		age = 0;
		body = "";
		sex = "";
		modality = "";
	}
};

enum WindowMode
{
	SingleWindow = 0,
	FourWindow,
	TWOHORIZONAL,
	TWOVERTICAL
};

//截屏信息
struct SnapCache
{
	QString pixpath;
	QString diagnosis;//诊断
};

enum OPERATION_MODE
{
	OFF = 0,
	ON
};

enum OPERATION_TYPE
{
	DRAG = 0,
	BRIGHTNESS,//亮度、对比度
	ZOOMPAN,//缩放
	ZOOMBACK,
	LINEMEASURE,
	ANGLEMEASURE,
	RECTANGLE,
	ELLIPSE,
	DRAW,
	ARROW,
	TEXT,
};

enum AreaToolStyle
{
	ELLIPSE_OBJ,
	RECTANGLE_OBJ
};

enum DrawingMode
{
	HALT,
	DRAWING
};

struct AreaObject
{
	AreaToolStyle object_type;
	bool visible;
	bool isChecked;
	glm::vec4 color;
	glm::vec3 points[4];
	std::string text;
};

enum ErrorCode{
	INIT_NETWORK=0,
	ASSOCIATION
};