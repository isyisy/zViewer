#pragma once
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/libi2d/i2d.h"
#include "dcmtk/dcmdata/libi2d/i2djpgs.h"
#include "dcmtk/dcmdata/libi2d/i2dbmps.h"
#include "dcmtk/dcmdata/libi2d/i2dplsc.h"
#include "dcmtk/dcmdata/libi2d/i2dplvlp.h"
#include "dcmtk/dcmdata/libi2d/i2dplnsc.h"

#include <QString>
#include <QList>

class Img2DicomUtils
{
public:
	Img2DicomUtils();
	~Img2DicomUtils();

public:
	//void setPaths(const OFString& inputPath,const OFString &outputPath, const OFString& format="JPEG");
	void setPaths(const QString& inputPath, const QString &outputPath="", const QString& format = "JPEG");
	void setKeys(const QList<QString> &overkeys);
	void write(const QString &outputPath = "");
private:
	// Main class for controlling conversion
	Image2Dcm i2d;
	// Output plugin to use (i.e. SOP class to write)
	I2DOutputPlug *outPlug;
	// Input plugin to use (i.e. file format to read)
	I2DImgSource *inputPlug;
	// Group length encoding mode for output DICOM file
	E_GrpLenEncoding grpLengthEnc;
	// Item and Sequence encoding mode for output DICOM file
	E_EncodingType lengthEnc;
	// Padding mode for output DICOM file
	E_PaddingEncoding padEnc;
	// File pad length for output DICOM file
	OFCmdUnsignedInt filepad;
	// Item pad length for output DICOM file
	OFCmdUnsignedInt itempad;
	// Write only pure dataset, i.e. without meta header
	E_FileWriteMode writeMode;
	// Override keys are applied at the very end of the conversion "pipeline"
	OFList<OFString> overrideKeys;
	// The transfer syntax proposed to be written by output plugin
	E_TransferSyntax writeXfer;


	OFString pixDataFile, outputFile, formatStr;
	OFCondition cond;
};

