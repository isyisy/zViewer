#include "Scp.h"
#include <QDebug>

CCScp::CCScp()
	:rootDir("D:\\Data")
{
	init();
}


CCScp::~CCScp()
{
}

void CCScp::init()
{
	setAETitle(m_DefaultAssociationParams.myAETitle);
	setPort(m_DefaultAssociationParams.listeningPort);
	
	setACSETimeout(-1);
	setDIMSETimeout(10000);
	setDIMSEBlockingMode(DIMSE_NONBLOCKING);
	setFilenameGenerationMode(DcmStorageSCP::FGM_CurrentSystemTime);
	setFilenameExtension(".dcm");
	
	transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
	transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
	transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
	transferSyntaxes.push_back(UID_JPEGProcess1TransferSyntax);
	addPresentationContext(UID_SecondaryCaptureImageStorage, transferSyntaxes);//jpg
	addPresentationContext(UID_EncapsulatedPDFStorage, transferSyntaxes);//pdf
	addPresentationContext(UID_CTImageStorage, transferSyntaxes);
	addPresentationContext(UID_MRImageStorage, transferSyntaxes);
	addPresentationContext(UID_UltrasoundImageStorage, transferSyntaxes);
	addPresentationContext(UID_ComputedRadiographyImageStorage, transferSyntaxes);
	addPresentationContext(UID_DigitalXRayImageStorageForProcessing, transferSyntaxes);
	addPresentationContext(UID_DigitalXRayImageStorageForPresentation, transferSyntaxes);
	addPresentationContext(UID_XRayRadiofluoroscopicImageStorage, transferSyntaxes);
	
	
}

void CCScp::init_association_parameter(OFString& title, UINT16 port)
{
	setAETitle(title);
	setPort(port);
}

void CCScp::SetSubDir(const OFString& dir)
{
	subDir = dir;
	absoluteDir = rootDir + "\\" + subDir;
	setOutputDirectory(absoluteDir);

}

//OFBool CCScp::stopAfterCurrentAssociation()
//{
//	qDebug() << "%%%%%%%%%%%%%%%hsakfhlanfklng%%%%%%%%%%%%%%%";
//	return true;
//}


void CCScp::notifyRECEIVEProgress(const unsigned long byteCount)
{

}

void CCScp::notifyReleaseRequest()
{
	//emit AssociationReleased();
	//qDebug() << "##############################";
}

void CCScp::notifyInstanceStored(const OFString &filename,
								const OFString &sopClassUID,
								const OFString &sopInstanceUID,
								DcmDataset *dataset) const
{
	
	emit OneInstanceStored();
}