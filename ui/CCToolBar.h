#ifndef CCTOOLBAR_H
#define CCTOOLBAR_H

#include "globals/CCGlobalStructs.h"
#include <QWidget>
#include <QToolButton>

class CCToolBar : public QWidget
{
	Q_OBJECT

public:
	CCToolBar(QWidget *parent);
	~CCToolBar();
public slots:
	void EnableToolbar(bool);
signals:
	void Operation(OPERATION_TYPE, OPERATION_MODE);

protected:
	void paintEvent(QPaintEvent *e);

private slots: 

private slots:
	void init_window();
	void create_toolbar();

	void function_zoom();
	void function_zoomback();
	void function_brightness();
	void function_linemeasure();
	void function_anglemeasure();
	void function_rectangle();
	void function_ellipse();
	void function_draw();
	void function_arrow();
	void function_text();

private:
	QToolButton* measure_length_btn;
	QToolButton* measure_angle_btn;
	QToolButton* measure_area_btn;
	QToolButton* measure_area2_btn;

	QToolButton* adjust_zoom_btn;
	//QToolButton* adjust_zoomback_btn;
	QToolButton* adjust_window_btn;
	QToolButton* adjust_undo_btn;

	QToolButton* text_pencil_btn;
	QToolButton* text_showhide_btn;
	QToolButton* text_arrow_btn;
	QToolButton* window_layout_btn;

	bool m_DragMode;
	bool m_ZoomMode;
	bool m_BrightnessMode;
	bool m_LengthMode;
	bool m_AngleMode;
	bool m_RectangleMode;
	bool m_EllipseMode;
	bool m_PencilMode;
	bool m_ArrowMode;
	bool m_TextMode;
};

#endif // CCTOOLBAR_H
