#ifndef ZVIEWER_H
#define ZVIEWER_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolTip>
#include <QtWidgets/QToolBar>
#include <QPalette>
#include <QMouseEvent>
#include <QFileSystemWatcher>
#include "ui/CCMainWidget.h"

class zViewer : public QMainWindow
{
	Q_OBJECT

public:
	zViewer(int w = 0, int h = 0, int l = 0, int t = 0, QWidget* parent = 0, Qt::WindowFlags = 0);
	~zViewer();
public slots:
	void close();
protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	bool eventFilter(QObject *obj, QEvent *ev);

private:
	void createTools();
	bool removeFolderContent(const QString &folderDir);
	private slots:
	void ResetApplicationSkin(QString);

private:

	QToolBar* file_tools;
	QToolBar* measure_tools;
	QToolBar* adjust_tools;
	QToolBar* text_tools;
	QToolBar* tool_tools;
	QToolBar* software_tools;

	QToolButton* file_open_btn;
	QToolButton* file_close_btn;
	QToolButton* file_save_btn;
	QToolButton* file_print_btn;
	QToolButton* file_report_btn;

	QToolButton* measure_length_btn;
	QToolButton* measure_angle_btn;
	QToolButton* measure_area_btn;
	QToolButton* measure_area2_btn;

	QToolButton* adjust_zoomin_btn;
	QToolButton* adjust_zoomout_btn;
	QToolButton* adjust_window_btn;
	QToolButton* adjust_undo_btn;

	QToolButton* text_pencil_btn;
	QToolButton* text_showhide_btn;
	QToolButton* text_arrow_btn;

	QToolButton* tools_screenshot_btn;
	QToolButton* tools_connect_btn;
	QToolButton* tools_user_btn;

	QToolButton* others_setting_btn;
	QToolButton* others_about_btn;

	QPushButton* window_close_btn;
	QPushButton* window_max_btn;
	QPushButton* window_min_btn;

	QFileSystemWatcher* watcher;

	int screenWidth, screenHeight, clientLeft, clientTop;
	bool windowSizeNormal, windowIsMin, dragMode;//窗口是否最小化
	QRect normalRect, maxsizeRect;
	QPoint last;

	CCMainWidget* m_mainWidget;
};

#endif // ZVIEWER_H
