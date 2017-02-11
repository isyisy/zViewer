#ifndef CCDICOMINFODLG_H
#define CCDICOMINFODLG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class CCDicomInfoDlg : public QDialog
{
	Q_OBJECT

public:
	CCDicomInfoDlg();
	~CCDicomInfoDlg();
public slots:
	void _ok();
	void _cancel();
private:
	void init_window();
	void init_controls();
private:

	QPushButton* m_CancelBtn;
	QPushButton* m_OkBtn;

	QLabel* m_PatientId;
	QLineEdit* m_Edit_PatientId;
	QLabel* m_PatientName;
	QLineEdit* m_Edit_PatientName;
	QLabel* m_PatientSex;
	QLineEdit* m_Edit_PatientSex;
	QLabel* m_PatientBirth;
	QLineEdit* m_Edit_PatientBirth;

	QLabel* m_StudyDate;
	QLineEdit* m_Edit_StudyDate;
	QLabel* m_AccessionNumber;
	QLineEdit* m_Edit_AccessionNumber;
	QLabel* m_StudyDescription;
	QLineEdit* m_Edit_StudyDescription;
	QLabel* m_BodyPart;
	QLineEdit* m_Edit_BodyPart;

	QLabel* m_SeriesNumber;
	QLineEdit* m_Edit_SeriesNumber;
	QLabel* m_SeriesDesctiption;
	QLineEdit* m_Edit_SeriesDesctiption;
	QLabel* m_SeriesModality;
	QLineEdit* m_Edit_SeriesModality;

public:
	QString p_name;
	QString p_id;
	QString p_sex;
	QString p_birth;

	QString s_acce;
	QString s_date;
	QString s_desc;
	QString s_body;

	QString r_numb;
	QString r_desc;
	QString r_moda;
};

#endif // CCDICOMINFODLG_H
