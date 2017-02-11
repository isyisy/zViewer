#include "Img2DicomUtils.h"


Img2DicomUtils::Img2DicomUtils()
	:formatStr("JPEG")
	, grpLengthEnc(EGL_recalcGL)
	, lengthEnc(EET_ExplicitLength)
	, padEnc(EPD_noChange)
	, filepad(0)
	, itempad(0)
	, writeMode(EWM_fileformat)
{
	outPlug = NULL;
	inputPlug = NULL;
}


Img2DicomUtils::~Img2DicomUtils()
{
	if (!outPlug)
		delete outPlug;
	if (!inputPlug)
		delete inputPlug;
}

void Img2DicomUtils::setPaths(const QString& inputPath, const QString &outputPath, const QString& format)
{
	pixDataFile = OFString(inputPath.toLocal8Bit().data());
	outputFile = OFString(outputPath.toLocal8Bit().data());
	formatStr = OFString(format.toLocal8Bit().data());

	if (formatStr=="JPEG")
		inputPlug = new I2DJpegSource();
	else if (formatStr == "BMP")
		inputPlug = new I2DBmpSource();

	outPlug = new I2DOutputPlugSC();//暂时不做选择

}

void Img2DicomUtils::setKeys(const QList<QString> &overkeys)
{
	overrideKeys.clear();
	QList<QString>::const_iterator it = overkeys.begin();
	for (it; it != overkeys.end(); it++)
	{
		OFString tempstr = OFString((*it).toLocal8Bit().data());
		overrideKeys.push_back(tempstr);
	}

	i2d.setOverrideKeys(overrideKeys);
}

void Img2DicomUtils::write(const QString &outputPath /* = "" */)
{
	outputFile = OFString(outputPath.toLocal8Bit().data());

	OFBool insertLatin1 = OFTrue;
	i2d.setISOLatin1(insertLatin1);

	// evaluate validity checking options
	OFBool insertType2 = OFTrue;
	OFBool inventType1 = OFTrue;
	OFBool doChecks = OFTrue;
	i2d.setValidityChecking(doChecks, insertType2, inventType1);
	outPlug->setValidityChecking(doChecks, insertType2, inventType1);

	if (inputPlug->inputFormat() == "JPEG")
	{
		I2DJpegSource *jpgSource = OFstatic_cast(I2DJpegSource*, inputPlug);
	}
	inputPlug->setImageFile(pixDataFile);
	if (!dcmDataDict.isDictionaryLoaded())
	{
		//
		std::cout << "dictionary load failed." << std::endl;
	}

	DcmDataset *resultObject = NULL;

	cond = i2d.convert(inputPlug, outPlug, resultObject, writeXfer);

	// Save
	if (cond.good())
	{
		DcmFileFormat dcmff(resultObject);
		cond = dcmff.saveFile(outputFile.c_str(), writeXfer, lengthEnc, grpLengthEnc, padEnc, OFstatic_cast(Uint32, filepad), OFstatic_cast(Uint32, itempad), writeMode);
	}
}