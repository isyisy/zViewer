#include "Scu.h"
#include <QDebug>

CCScu::CCScu()
{
	init_presentation_context();
	init_association_parameter();
	init_dataset();
}


CCScu::~CCScu()
{
}

void CCScu::init_presentation_context()
{
	OFList<OFString> supportedTransferSyntaxes;
	supportedTransferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
	supportedTransferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
	supportedTransferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
	this->addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, supportedTransferSyntaxes);
	this->addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, supportedTransferSyntaxes);
	this->addPresentationContext(UID_VerificationSOPClass, supportedTransferSyntaxes);
	this->addPresentationContext(UID_CTImageStorage, supportedTransferSyntaxes);
	this->addPresentationContext(UID_MRImageStorage, supportedTransferSyntaxes);
	this->addPresentationContext(UID_DigitalXRayImageStorageForProcessing, supportedTransferSyntaxes);
	this->addPresentationContext(UID_DigitalXRayImageStorageForPresentation, supportedTransferSyntaxes);

}

void CCScu::init_association_parameter(OFString aet, OFString paet, OFString phost, UINT16 pport)
{
	this->setAETitle(aet);
	this->setPeerAETitle(paet);
	this->setPeerHostName(phost);
	this->setPeerPort(pport);
}

void CCScu::init_association_parameter(const char* file)
{
	if (file == NULL)
	{
		this->setAETitle(m_DefaultAssociationParams.myAETitle);
		this->setPeerAETitle(m_DefaultAssociationParams.peerAETitle);
		this->setPeerHostName(m_DefaultAssociationParams.peerHostName);
		this->setPeerPort(m_DefaultAssociationParams.peerPort);
	}
	else
	{
		//....

	}
}

void CCScu::init_dataset()
{
	m_QueryDataset.putAndInsertString(DCM_PatientID, "");
	m_QueryDataset.putAndInsertString(DCM_PatientName, "");
	m_QueryDataset.putAndInsertString(DCM_PatientSex, "");
	m_QueryDataset.putAndInsertString(DCM_PatientBirthDate, "");
	m_QueryDataset.putAndInsertString(DCM_PatientAge, "");

	m_QueryDataset.putAndInsertString(DCM_StudyInstanceUID, "");
	m_QueryDataset.putAndInsertString(DCM_AccessionNumber, "");
	m_QueryDataset.putAndInsertString(DCM_StudyID, "");
	m_QueryDataset.putAndInsertString(DCM_StudyDate, "");
	m_QueryDataset.putAndInsertString(DCM_StudyDescription, "");
	m_QueryDataset.putAndInsertString(DCM_BodyPartExamined, "");

	m_QueryDataset.putAndInsertString(DCM_SeriesInstanceUID, "");
	m_QueryDataset.putAndInsertString(DCM_SeriesNumber, "");
	m_QueryDataset.putAndInsertString(DCM_Modality, "");
	m_QueryDataset.putAndInsertString(DCM_SeriesDescription, "");
	m_QueryDataset.putAndInsertString(DCM_ImageOrientationPatient, "");

	m_QueryDataset.putAndInsertString(DCM_SOPInstanceUID, "");
	m_QueryDataset.putAndInsertString(DCM_InstanceNumber, "");
	m_QueryDataset.putAndInsertString(DCM_Columns, "");
	m_QueryDataset.putAndInsertString(DCM_Rows, "");
}

OFCondition CCScu::EchoPacs()
{
	OFCondition cond = initNetwork();
	if (!cond.good())  
		return cond;
	cond = negotiateAssociation();
	if (cond.bad()) 
		return cond;
	T_ASC_PresentationContextID presID = find_uncompressed_presentation_context(UID_VerificationSOPClass);
	if (presID == 0) 
		return cond;
	cond = sendECHORequest(presID);
	if (cond.good())
		closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
	return cond;
}

void CCScu::SetQueryLevel(const char* level/* ="STUDY" */)
{
	m_QueryDataset.putAndInsertString(DCM_QueryRetrieveLevel, level);
}

void CCScu::AddQueryCondition(const DcmTag& tag,const char* value0, const char* value1)
{
	m_QueryDataset.putAndInsertString(tag, value0);
}


OFCondition CCScu::QueryPacs(const OFString &sopClass, const char* level, DcmDataset *dataset)
{
	m_DcmdatasetList.clear();//clear result list

	if (dataset == 0)//load default
		dataset = &m_QueryDataset;
	OFCondition cond = initNetwork();
	if (!cond.good())
	{
		std::cout << "init network failed!" << std::endl;
		emit ErrorSignal(ErrorCode::INIT_NETWORK);
		return cond;
	}
	cond = this->negotiateAssociation();
	if (!cond.good())
	{
		std::cout << "negotiate association failed!" << std::endl;
		emit ErrorSignal(ErrorCode::ASSOCIATION);
		return cond;
	}
	T_ASC_PresentationContextID presID = find_uncompressed_presentation_context(sopClass);
	if (presID == 0)
	{
		std::cout << "no uncompressed presentation context found!" << std::endl;
		return cond;
	}
	OFList<QRResponse *> responses;
	cond = this->sendFINDRequest(presID, dataset, &responses);
	Uint32 responseNums = responses.size();

	if (cond.good())
	{
		this->closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
	}

	for (OFIterator<QRResponse*> it = responses.begin(); it != responses.end(); it++)
	{
		DcmDataset *dataset = (*it)->m_dataset;
		if (dataset != NULL)
		{
			m_DcmdatasetList.push_back(dataset);
		}
	}
	if (level == "STUDY")
		emit StudyListResults(m_DcmdatasetList);
	else if (level == "SERIES")
		emit SeriesListResults(m_DcmdatasetList);

	//清空查询条件设置,remember
	init_dataset();

	return cond;
}

OFCondition CCScu::MovePacs(const OFString& destinationTitle, const OFString &sopClass, DcmDataset *dataset)
{
	if (dataset == 0)//load default
		dataset = &m_QueryDataset;
	OFCondition cond = initNetwork();
	if (!cond.good())
	{
		std::cout << "init network failed!" << std::endl;
		return cond;
	}
	cond = this->negotiateAssociation();
	if (!cond.good())
	{
		std::cout << "negotiate association failed!" << std::endl;
		return cond;
	}
	T_ASC_PresentationContextID presID = find_uncompressed_presentation_context(sopClass);
	if (presID == 0)
	{
		std::cout << "no uncompressed presentation context found!" << std::endl;
		return cond;
	}

	OFList<RetrieveResponse *> responses;
	cond = this->sendMOVERequest(presID, destinationTitle, dataset, &responses);
	//Uint32 responseNums = responses.size();

	if (cond.good())
	{
		this->closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
	}
	return cond;
}

OFCondition CCScu::StorePacs(const OFString & sopClass, DcmDataset* dataset/* =0 */)  
{
	if (dataset == 0)//load default
		dataset = &m_QueryDataset;
	OFCondition cond = initNetwork();
	if (!cond.good())
	{
		std::cout << "init network failed!" << std::endl;
		return cond;
	}
	cond = this->negotiateAssociation();
	if (!cond.good())
	{
		std::cout << "negotiate association failed!" << std::endl;
		return cond;
	}
	T_ASC_PresentationContextID presID = find_uncompressed_presentation_context(sopClass);
	if (presID == 0)
	{
		std::cout << "no uncompressed presentation context found!" << std::endl;
		return cond;
	}

	OFString transferUID;
	dataset->findAndGetOFString(DCM_TransferSyntaxUID, transferUID);
	if ((strcmp(transferUID.c_str(), "1.2.840.10008.1.2.2")) && (strcmp(transferUID.c_str(), "1.2.840.10008.1.2")) &&
		(strcmp(transferUID.c_str(), "1.2.840.10008.1.2.1")))
	{
		cond = this->DecompressFile(dataset);
		//return result;
	}

	UINT16 status;
	cond = this->sendSTORERequest(presID, NULL, dataset, status);

	if (cond.good())
	{
		this->closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
	}
	return cond;
}

T_ASC_PresentationContextID CCScu::find_uncompressed_presentation_context(const OFString& sopClass)
{
	Uint8 pc;
	pc = findPresentationContextID(sopClass, UID_LittleEndianExplicitTransferSyntax);
	if (pc == 0)
		pc = findPresentationContextID(sopClass, UID_BigEndianExplicitTransferSyntax);
	if (pc == 0)
		pc = findPresentationContextID(sopClass, UID_LittleEndianImplicitTransferSyntax);

	return pc;
}

OFCondition CCScu::DecompressFile(DcmDataset* dataset)
{
	DJDecoderRegistration::registerCodecs();
	DJEncoderRegistration::registerCodecs();
	DcmRLEEncoderRegistration::registerCodecs();
	DcmRLEDecoderRegistration::registerCodecs();

	/* make sure data dictionary is loaded */
	if (!dcmDataDict.isDictionaryLoaded())
	{
		//do something
	}

	OFCondition error = EC_Normal;

	E_TransferSyntax opt_oxfer;
	if (gLocalByteOrder == EBO_LittleEndian)
	{
		opt_oxfer = EXS_LittleEndianExplicit;
	}
	else{
		opt_oxfer = EXS_BigEndianExplicit;
	}

	error = dataset->chooseRepresentation(opt_oxfer, NULL);

	if (error.bad())
	{
		if (error == EJ_UnsupportedColorConversion)
			;//do something
		else if (error == EC_CannotChangeRepresentation)
			;//do something
		return error;
	}

	if (!dataset->canWriteXfer(opt_oxfer))
	{
		//do something
		return error;
	}

	DJDecoderRegistration::cleanup();
	DJEncoderRegistration::cleanup();
	DcmRLEDecoderRegistration::cleanup();
	DcmRLEEncoderRegistration::cleanup();

	return error;
}

void CCScu::notifyInstanceStored(const OFString &filename,
	const OFString &sopClassUID,
	const OFString &sopInstanceUID) const
{
	//qDebug() << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
}

void CCScu::notifySENDProgress(const unsigned long byteCount)
{
	emit CurrentByteSended(byteCount);
}
//
//OFCondition CCScu::releaseAssociation()
//{
//	OFCondition status = DIMSE_ILLEGALASSOCIATION;
//	// check whether there is an active association
//	if (isConnected())
//	{
//		closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
//		status = EC_Normal;
//	}
//	qDebug() << "*************************************************";
//	return status;
//}