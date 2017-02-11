#ifndef CCDICOMUTILS_H
#define CCDICOMUTILS_H

#include <QObject>
#include <QString>
#include <QOpenGLWidget>

#include "dcmtk/config/osconfig.h"  
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmjpeg/djdecode.h"    /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"    /* for dcmimage JPEG plugin */
#include "dcmtk/dcmimage/diregist.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/digsdfn.h"

class CCDicomUtils : public QObject
{
	Q_OBJECT

public:
	CCDicomUtils(QObject *parent=0);
	CCDicomUtils(QOpenGLWidget* caller);
	~CCDicomUtils();

	int    width(){ return m_Width; }
	int    alignWidth(){ return m_AlignWidth; }
	int    height(){ return m_Height; }
	int    depth(){ return m_Depth; }
	short  minPixelValue(){ return m_MinPixelValue; }
	short  maxPixelValue(){ return m_MaxPixelValue; }
	int    windowCenter(){ return m_WindowCenter; }
	int    windowWidth(){ return m_WindowWidth; }

	QString patientName(){ return m_PatientName; }
	QString patientId(){ return m_PatientId; }
	QString patientSex(){ return m_PatientGender; }
	QString patientBirthday(){ return m_PatientBirth; }

	QString studyInstanceId(){ return m_StudyInstanceId; }
	QString studyAccessionNumber(){ return m_AccessionNumber; }
	QString studyDate(){ return m_StudyDate; }
	QString studyDescription(){ return m_StudyDescription; }
	QString studyBodyPart(){ return m_StudyBodyPart; }
	QString studyDateTime(){ return m_StudyDateTime; }

	QString seriesInstanceId(){ return m_SeriesInstanceId; }
	QString seriesNumber(){ return m_SeriesNumber; }
	QString seriesDescription(){ return m_SeriesDescription; }
	QString seriesModality(){ return m_SeriesModality; }
	QString seriesImageOrientation(){ return m_SeriesImageOrientation; }
	
	short* data(){ return m_PixelData; }
	unsigned char* thumbData(){ return m_ThumeData; }

	short* readDicomFile(const char* filepath, int fileCount=1);
	void readSlice(const char* filename);
	void loadDicomDir(const char* dirPath);

	void setIndex();
	QString getIndex();
	QString getAbsoluteImagePath(){ return m_CurrentImagePath; }
private:
	void convertTo8BitImage();

protected:
	OFCondition decompress_file(DcmDataset *dataset);

private:

	int m_Width;
	int m_Height;
	int m_Depth;
	int m_AlignWidth;

	short m_MinPixelValue;
	short m_MaxPixelValue;
	int m_WindowWidth;
	int m_WindowCenter;

	//patient
	QString m_PatientName;
	QString m_PatientId;
	QString m_PatientGender;
	QString m_PatientBirth;

	//study
	QString m_StudyInstanceId;
	QString m_AccessionNumber;
	QString m_StudyDate;
	QString m_StudyDateTime;
	QString m_StudyDescription;
	QString m_StudyBodyPart;

	//series
	QString m_SeriesInstanceId;
	QString m_SeriesNumber;
	QString m_SeriesModality;
	QString m_SeriesDescription;
	QString m_SeriesImageOrientation;

	//images
	unsigned short m_SamplesPerPixel;
	unsigned short m_BitsAllocated;
	long m_ImageSizeInShort;//单帧图像的大小(单位是short)
	long m_TotalSizeInShort;//体数据图像大小

	short* m_PixelData;
	Uint16* pData;//临时储存
	Uint8* pData_8Bit;

	int m_SizeofImage;//单张图像大小
	int m_CurrentSlice;
	unsigned char* m_ThumeData;

	E_TransferSyntax opt_oxfer;

	QString m_CurrentImagePath;
};

#endif // CCDICOMUTILS_H
