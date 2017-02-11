#include "CCDicomUtils.h"
#include "dcmtk/dcmjpeg/djdecode.h"  /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/djencode.h"  /* for dcmjpeg encoders */
#include "dcmtk/dcmdata/dcrledrg.h"  /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmdata/dcrleerg.h"  /* for DcmRLEEncoderRegistration */
#include "dcmtk/dcmjpeg/dipijpeg.h"  /* for dcmimage JPEG plugin */

#include <QDebug>

static int RandomIndex = 0;

#define  PIXEL_OFFSET 2000 //最小pixel value值绝对值

CCDicomUtils::CCDicomUtils(QObject *parent)
	: QObject(parent)
	, opt_oxfer(EXS_LittleEndianExplicit)
	, m_Height(0)
	, m_Width(0)
	, m_Depth(1)
	, m_MinPixelValue(32767)
	, m_MaxPixelValue(-32768)
	, m_PixelData(NULL)
	, m_ThumeData(NULL)
	, pData(NULL)
	, pData_8Bit(NULL)
	, m_CurrentSlice(0)
	, m_CurrentImagePath("")
{

}

CCDicomUtils::CCDicomUtils(QOpenGLWidget* caller)
	: m_Height(0)
	, m_Width(0)
	, m_Depth(1)
	, m_MinPixelValue(32767)
	, m_MaxPixelValue(-32768)
	, m_PixelData(NULL)
	, m_ThumeData(NULL)
	, pData(NULL)
	, pData_8Bit(NULL)
	, opt_oxfer(EXS_LittleEndianExplicit)
	, m_CurrentSlice(0)
	, m_CurrentImagePath("")
{
	
}

CCDicomUtils::~CCDicomUtils()
{
	if (m_ThumeData != NULL)
	{
		delete m_ThumeData;
		m_ThumeData = NULL;
	}
	if (m_PixelData != NULL)
	{
		delete m_PixelData;
		m_PixelData = NULL;
	}
	/*if (pData_8Bit != NULL)
	{
	delete pData_8Bit;
	pData_8Bit = NULL;
	}
	if (pData != NULL)
	{
	delete pData;
	pData = NULL;
	}*/

}

QString CCDicomUtils::getIndex()
{
	QString name = QString("%1").arg(RandomIndex);
	return name;
}

void CCDicomUtils::setIndex()
{
	RandomIndex++;
}

short* CCDicomUtils::readDicomFile(const char* filePath, int fileCount /*=1*/)
{
	short* data_buffer = NULL;
	OFCondition cond = EC_Normal;
	DcmFileFormat format;
	cond = format.loadFile(filePath);
	if (cond.bad())
		return NULL;
	DcmDataset* dataset = format.getDataset();
	DcmXfer xfer(dataset->getOriginalXfer());
	const char* xfer_id = xfer.getXferID();
	if ((strcmp(xfer_id, "1.2.840.10008.1.2.2")) && (strcmp(xfer_id, "1.2.840.10008.1.2")) &&
		(strcmp(xfer_id, "1.2.840.10008.1.2.1")))
	{
		cond = decompress_file(dataset);
	}
	if (cond.bad())
		return NULL;

	m_CurrentImagePath = QString(filePath);

	//Patient related information
	OFString pid, pname, psex, pbirth;
	dataset->findAndGetOFString(DCM_PatientID, pid);
	//m_PatientId = QString(pid.c_str());
	m_PatientId = QString::fromLocal8Bit(pid.c_str());
	dataset->findAndGetOFString(DCM_PatientName, pname);
	//m_PatientName = QString(pname.c_str());
	m_PatientName = QString::fromLocal8Bit(pname.c_str());
	dataset->findAndGetOFString(DCM_PatientBirthDate, pbirth);
	m_PatientBirth = QString(pbirth.c_str());
	dataset->findAndGetOFString(DCM_PatientSex, psex);
	//m_PatientGender = QString(psex.c_str());
	m_PatientGender = QString::fromLocal8Bit(psex.c_str());
	//study related information
	OFString snumber, sid, sdate, stime, sdescription, sbodypart;
	dataset->findAndGetOFString(DCM_AccessionNumber, snumber);
	m_AccessionNumber = QString(snumber.c_str());
	dataset->findAndGetOFString(DCM_StudyInstanceUID, sid);//unique
	m_StudyInstanceId = QString(sid.c_str());
	dataset->findAndGetOFString(DCM_StudyDate, sdate);
	dataset->findAndGetOFString(DCM_StudyTime, stime);
	m_StudyDate = QString(sdate.c_str());
	m_StudyDateTime = QString(sdate.c_str()) + " " + QString(stime.c_str()).left(6);

	dataset->findAndGetOFString(DCM_StudyDescription, sdescription);
	//m_StudyDescription = QString(sdescription.c_str());
	m_StudyDescription = QString::fromLocal8Bit(sdescription.c_str());
	dataset->findAndGetOFString(DCM_BodyPartExamined, sbodypart);
	//m_StudyBodyPart = QString(sbodypart.c_str());
	m_StudyBodyPart = QString::fromLocal8Bit(sbodypart.c_str());
	//Series related information
	OFString id, number, time, date, description, modality, imageorient;
	dataset->findAndGetOFString(DCM_SeriesInstanceUID, id);
	m_SeriesInstanceId = QString(id.c_str());
	dataset->findAndGetOFString(DCM_SeriesNumber, number);
	m_SeriesNumber = QString(number.c_str());
	dataset->findAndGetOFString(DCM_Modality, modality);
	m_SeriesModality = QString(modality.c_str());
	dataset->findAndGetOFString(DCM_SeriesDescription, description);
	//m_SeriesDescription = QString(description.c_str());
	m_SeriesDescription = QString::fromLocal8Bit(description.c_str());
	dataset->findAndGetOFString(DCM_ImageOrientationPatient, imageorient);
	m_SeriesImageOrientation = QString(imageorient.c_str());

	//图像信息
	dataset->findAndGetUint16(DCM_SamplesPerPixel, m_SamplesPerPixel);
	dataset->findAndGetUint16(DCM_BitsAllocated, m_BitsAllocated);
	
	Uint16 nRows = 0;
	dataset->findAndGetUint16(DCM_Rows, nRows);
	this->m_Height = (int)nRows;
	Uint16 nCols = 0;
	dataset->findAndGetUint16(DCM_Columns, nCols);
	this->m_Width = (int)nCols;
	this->m_Depth = fileCount;

	this->m_SizeofImage = m_Height*m_Width;
	this->m_AlignWidth = (m_Width + 3) / 4 * 4; //4字节对齐

	unsigned short bytes_per_pixel = m_BitsAllocated*m_SamplesPerPixel / 8;
	m_ImageSizeInShort = (m_Width*m_Height*bytes_per_pixel)/2;
	m_TotalSizeInShort = m_ImageSizeInShort*m_Depth;

	OFString window_width, window_center;
	dataset->findAndGetOFString(DCM_WindowWidth, window_width);
	dataset->findAndGetOFString(DCM_WindowCenter, window_center);
	const char* c = window_center.data();
	this->m_WindowCenter = atoi(c);
	const char* w = window_width.data();
	this->m_WindowWidth = atoi(w);

	////////////////////////////////////////////////////////////////
	DcmElement* element = NULL;
	dataset->findAndGetElement(DCM_PixelData, element);
	if (m_SamplesPerPixel == 1 && m_BitsAllocated == 16)
	{
		pData = new Uint16[m_ImageSizeInShort]();
		element->getUint16Array(pData);
		data_buffer = new short[m_TotalSizeInShort]();
		m_WindowCenter += PIXEL_OFFSET;
		
	}
	else if (m_SamplesPerPixel == 1 && m_BitsAllocated == 8)
	{
		data_buffer = new short[m_TotalSizeInShort * 2]();
		pData_8Bit = new Uint8[m_ImageSizeInShort * 2]();
		element->getUint8Array(pData_8Bit);
		//
		m_WindowWidth = 255;
		m_WindowCenter = 128+PIXEL_OFFSET;
	}
	else if (m_SamplesPerPixel == 3 && m_BitsAllocated == 8)
	{
		data_buffer = new short[m_Height*m_Width]();
		pData_8Bit = new Uint8[m_Height*m_Width * 3]();
		element->getUint8Array(pData_8Bit);
		//
		m_WindowWidth = 255;
		m_WindowCenter = 128+PIXEL_OFFSET;
	}
	
	//赋值
	if (m_SamplesPerPixel == 1 && m_BitsAllocated == 16)
	{
		for (int i = 0; i < m_Height; i++)
		{
			for (int j = 0; j < m_Width; j++)
			{
				short temp = (short)pData[i*m_Width + j];
				data_buffer[(m_Height - i - 1)*m_Width + j] = temp+PIXEL_OFFSET;
			}
		}
	}
	else if (m_SamplesPerPixel == 3 && m_BitsAllocated == 8)
	{
		//Uint8* pData_handle = (Uint8*)pData;
		for (int i = 0; i < m_Height; i++)
		{
			for (int j = 0; j < m_Width; j++)
			{
				//short temp = (short)pData_handle[i*m_Width + j];
				short temp0 = (short)pData_8Bit[(i*m_Width + j)*3+0];
				short temp1 = (short)pData_8Bit[(i*m_Width + j) * 3 + 1];
				short temp2 = (short)pData_8Bit[(i*m_Width + j) * 3 + 2];
				short temp = 0.299*temp0 + 0.587*temp1 + 0.114*temp2;
				data_buffer[(m_Height - i - 1)*m_Width + j] = temp+PIXEL_OFFSET;
			}
		}
	}
	else if (m_SamplesPerPixel == 1 && m_BitsAllocated == 8)
	{
		for (int i = 0; i < m_Height; i++)
		{
			for (int j = 0; j < m_Width; j++)
			{
				//short temp = (short)pData_handle[i*m_Width + j];
				short temp = (short)pData_8Bit[i*m_Width + j];
				data_buffer[(m_Height - i - 1)*m_Width + j] = temp+PIXEL_OFFSET;
			}
		}
	}

	//delete[] pData;
	//pData = NULL;
	m_PixelData = data_buffer;// do not forget it

	//获得最大最小值
	if (m_SamplesPerPixel == 1 && m_BitsAllocated == 16)
	{
		for (int i = 0; i < m_Width*m_Height; i++)
		{
			short temp = (short)pData[i];
			if (temp>m_MaxPixelValue)
				m_MaxPixelValue = temp;
			if (temp < m_MinPixelValue)
				m_MinPixelValue = temp;
		}
		m_MinPixelValue = m_MinPixelValue + PIXEL_OFFSET;
		m_MaxPixelValue = m_MaxPixelValue + PIXEL_OFFSET;
	}
	else if (m_SamplesPerPixel == 1 && m_BitsAllocated == 8)
	{
		m_MaxPixelValue = 255+PIXEL_OFFSET;
		m_MinPixelValue = 0+PIXEL_OFFSET;
	}
	else if (m_SamplesPerPixel == 3 && m_BitsAllocated == 8)
	{
		m_MaxPixelValue = 255+PIXEL_OFFSET;
		m_MinPixelValue = 0+PIXEL_OFFSET;
	}

	if (m_WindowWidth == 0)//没有获取到窗宽窗位字段
	{
		m_WindowCenter = (m_MinPixelValue+m_MaxPixelValue)/2;
		m_WindowWidth = m_MaxPixelValue-m_MinPixelValue;
	}

	convertTo8BitImage();

	return data_buffer;
}

void CCDicomUtils::convertTo8BitImage()
{
	if (m_PixelData == NULL)
		return;
	if (m_Width == 0 || m_Height == 0)
		return;

	short lowerValue = m_WindowCenter - m_WindowWidth / 2;
	short upperValue = m_WindowCenter + m_WindowWidth / 2;
	if (m_WindowWidth == 0)//没有获取到窗宽窗位字段
	{
		lowerValue = m_MinPixelValue;
		upperValue = m_MaxPixelValue;
	}
	m_ThumeData = new unsigned char[m_AlignWidth*m_Height]();
	for (int i = 0; i < m_Height; i++)
	{
		for (int j = 0; j < m_Width; j++)
		{
			short temp = m_PixelData[i*m_Width + j];
			temp = (temp>upperValue) ? upperValue : temp;
			temp = (temp<lowerValue) ? lowerValue : temp;
			unsigned char value = (unsigned char)((temp - lowerValue)*255 / (upperValue - lowerValue));
			m_ThumeData[(m_Height-1-i)*m_AlignWidth + j] = value;
		}
	}
}

void CCDicomUtils::loadDicomDir(const char* filename)
{

}

void CCDicomUtils::readSlice(const char* filename)
{
	m_CurrentSlice++;//当前层+1

	DcmFileFormat fileformat;
	DcmDataset *dataset;
	OFCondition cond = EC_Normal;
	fileformat.loadFile(filename);
	dataset = fileformat.getDataset();
	DcmXfer xfer(dataset->getOriginalXfer());
	const char* xfer_id = xfer.getXferID();
	if ((strcmp(xfer_id, "1.2.840.10008.1.2.2")) && (strcmp(xfer_id, "1.2.840.10008.1.2")) &&
		(strcmp(xfer_id, "1.2.840.10008.1.2.1")))
	{
		cond = decompress_file(dataset);
	}
	if (cond.bad())
		return;

	DcmElement *element = NULL;
	dataset->findAndGetElement(DCM_PixelData, element);
	element->getUint16Array(pData);

	for (int i = 0; i < m_Height; i++)
	{
		for (int j = 0; j < m_Width; j++)
		{
			short temp = (short)pData[i*m_Width + j];
			temp += PIXEL_OFFSET;
			if (temp>m_MaxPixelValue)
				m_MaxPixelValue = temp;
			if (temp < m_MinPixelValue)
				m_MinPixelValue = temp;
			m_PixelData[(m_CurrentSlice)*m_SizeofImage+(m_Height - i - 1)*m_Width + j] = temp;
		}
	}

}

OFCondition CCDicomUtils::decompress_file(DcmDataset *dataset)
{
	DJDecoderRegistration::registerCodecs();
	DJEncoderRegistration::registerCodecs();
	DcmRLEEncoderRegistration::registerCodecs();
	DcmRLEDecoderRegistration::registerCodecs();

	/* make sure data dictionary is loaded */
	if (!dcmDataDict.isDictionaryLoaded())
	{
		printf("no data dictionary loaded, check environment variable!\n");
	}

	OFCondition error = EC_Normal;

	error = dataset->chooseRepresentation(opt_oxfer, NULL);

	if (error.bad())
	{
		if (error == EJ_UnsupportedColorConversion)
			printf("Try --conv-never to disable color space conversion\n");
		else if (error == EC_CannotChangeRepresentation)
			printf("Input transfer syntax %s not supported\n");
		return error;
	}

	if (!dataset->canWriteXfer(opt_oxfer))
	{
		std::cout << "cannot write xfer!" << std::endl;
		return error;
	}

	DJDecoderRegistration::cleanup();
	DJEncoderRegistration::cleanup();
	DcmRLEDecoderRegistration::cleanup();
	DcmRLEEncoderRegistration::cleanup();

	return error;
}
