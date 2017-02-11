#ifndef REPORTEDITVIEW_H
#define REPORTEDITVIEW_H

#include "globals/CCGlobalStructs.h"
#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QRadioButton>
#include <QProcess>
#include "CustomLabel.h"

class ReportEditView : public QWidget
{
	Q_OBJECT

public:
	ReportEditView(QWidget *parent);
	~ReportEditView();
signals:
	void GotoOtherTab(int index);//跳转到其他Tab页
	void ConvertPdf2Png();
public slots:
	void ShowInformation(BasicInfo*);
	void RadioChanged();
	void ReportPreview();
protected:
	void paintEvent(QPaintEvent *);
private:
	void init_window();
	void set_style();
	void draw_baiei_report(QPainter* p, int WIDTH, int HEIGHT, int &start_x, int &start_y);
	void draw_zhenduan_report(QPainter* p, int WIDTH, int HEIGHT, int &start_x, int &start_y);

private:
	QGroupBox* m_BasicInfoGrp;
	QGroupBox* m_BaiweiGrp;
	QGroupBox* m_ZhenduanGrp;
	QGroupBox* m_ReportInfoGrp;

	QLabel* m_id_label;
	QLabel* m_name_label;
	QLabel* m_type_label;//种类
	QLabel* m_breed_label;//品种
	QLabel* m_sex_label;
	QLabel* m_age_label;
	QLabel* m_body_label;
	QLabel* m_date_label;
	QLabel* m_modality_label;

	QLineEdit* m_id_edit;
	QLineEdit* m_name_edit;
	QLineEdit* m_type_edit;//种类
	QLineEdit* m_breed_edit;//品种
	QLineEdit* m_sex_edit;
	QLineEdit* m_age_edit;
	QLineEdit* m_body_edit;
	QLineEdit* m_date_edit;
	QLineEdit* m_modality_edit;

	//
	QTextEdit* m_BaiweiEdit;
	QTextEdit* m_ZhenduanEdit;
	CustomLabel* m_BaiweiLabel;
	CustomLabel* m_ZhenduanLabel;

	//
	QLineEdit* m_report_date_edit;
	QLineEdit* m_report_doctor1_edit;
	QLineEdit* m_report_doctor2_edit;
	QLabel* m_report_date_label;
	QLabel* m_report_doctor1_label;
	QLabel* m_report_doctor2_label;
	QPushButton* m_preview_btn;
	QRadioButton* m_Radio0;
	//QRadioButton* m_Radio1;
	QRadioButton* m_Radio2;

	QProcess *process;

	int reportMode;

};

#endif // REPORTEDITVIEW_H
