#pragma once

#include <QObject>
#include "globals\CCGlobalStructs.h"

#include "dcmtk\dcmnet\scu.h"
#include "dcmtk\dcmnet\diutil.h"
#include "dcmtk\dcmdata\dcfilefo.h"

#include "dcmtk/dcmjpeg/djdecode.h"  /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/djencode.h"  /* for dcmjpeg encoders */
#include "dcmtk/dcmdata/dcrledrg.h"  /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmdata/dcrleerg.h"  /* for DcmRLEEncoderRegistration */
#include "dcmtk/dcmjpeg/dipijpeg.h"  /* for dcmimage JPEG plugin */

class CCScu : public QObject, public DcmSCU
{
	Q_OBJECT
public:
	struct DefaultAssociationParameter
	{
		OFString myAETitle;
		OFString peerAETitle;
		OFString peerHostName;
		UINT16 peerPort;
		DefaultAssociationParameter()
		{
			myAETitle    = "VET";
			peerAETitle  = "COMMON";
			peerHostName = "192.168.0.120";
			//peerHostName = "127.0.0.1";
			peerPort     = 104;
		}
	}m_DefaultAssociationParams;

public:
	CCScu();
	~CCScu();
	void init_association_parameter(OFString aet, OFString paet, OFString phost, UINT16 pport);
	void init_association_parameter(const char* file = NULL);

	void init_dataset();
	void SetQueryLevel(const char* level="STUDY");
	void AddQueryCondition(const DcmTag& tag, const char* value0 = "*", const char* value1 = "*");


	/*OFCondition sendSTORERequest(const T_ASC_PresentationContextID presID,
		const OFString &dicomFile,
		DcmDataset *dataset,
		Uint16 &rspStatusCode,
		const OFString &moveOriginatorAETitle = "",
		const Uint16 moveOriginatorMsgID = 0);
	Uint16 nextMessageID();
	/// Dataset conversion mode (default: disabled)
	OFBool m_datasetConversionMode;
	/// Association of this SCU. This class only handles 1 association at a time.
	T_ASC_Association *m_assoc;*/

public slots:
	OFCondition EchoPacs();
	OFCondition QueryPacs(const OFString &sopClass, const char* level= "STUDY", DcmDataset *dataset=0);
	OFCondition MovePacs(const OFString& destinationTitle, const OFString &sopClass, DcmDataset *dataset=0);
	OFCondition StorePacs(const OFString & sopClass, DcmDataset* dataset=0);

signals:
	void StudyListResults(OFList<DcmDataset*>);
	void SeriesListResults(OFList<DcmDataset*>);

	void CurrentByteSended(unsigned long);
	void ErrorSignal(ErrorCode code);
protected:
	void notifyInstanceStored(const OFString &filename,
		const OFString &sopClassUID,
		const OFString &sopInstanceUID) const;
	void notifySENDProgress(const unsigned long byteCount);
	//OFCondition releaseAssociation();
private:
	void init_presentation_context();
	T_ASC_PresentationContextID find_uncompressed_presentation_context(const OFString& sopClass);
	OFCondition DecompressFile(DcmDataset* dataset);
private:
	OFList<DcmDataset*> m_DcmdatasetList;//query result
	DcmDataset m_QueryDataset;;//query condition

};

