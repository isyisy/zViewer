#ifndef CCGLOBALCONSTS_H
#define CCGLOBALCONSTS_H

#include <QString>

namespace CCGLobalConsts
{
	//TabButton state
	const QString QML_NORMAL_TAB_BUTTON = "QGroupBox{background-color:#212121;border-style:none;border-top-left-radius:2px;"
		"border-top-right-radius:2px;}";
	/*const QString QML_HOVER_TAB_BUTTON = "QGroupBox{background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
		"stop: 0 #78aaff, stop: 0.5 #a8bfec,stop: 1.0 #d8d8d8);border-style:none;border-top-left-radius:2px;border-top-right-radius:2px;}";*/
	/*const QString QML_CHECKED_TAB_BUTTON = "QGroupBox{background-color:#00162c;"
		"border-top-right-radius:2px;border-top-left-radius:2px;}";*/
	const QString QML_HOVER_TAB_BUTTON = "QGroupBox{background-color:#213366;border-style:none;border-top-left-radius:2px;border-top-right-radius:2px;}";
	const QString QML_CHECKED_TAB_BUTTON = "QGroupBox{background-color:#121212;"
		"border-top-right-radius:2px;border-top-left-radius:2px;}";

	//TabTitle state
	const QString QML_NORMAL_TAB_TITLE = "QLabel{background:transparent;text-align:center;color:white;font:normal 16pt Microsoft YaHei;}";
	const QString QML_HOVER_TAB_TITLE = "QLabel{background:transparent;text-align:center;color:#22cc44;font:normal 16pt Microsoft YaHei;}";
	const QString QML_CHECKED_TAB_TITLE = "QLabel{background:transparent;text-align:center;color:#7e4027;font:normal 16pt Microsoft YaHei;}";

	//Progress bar
	const QString QML_PROGRESSBAR = "QProgressBar{border:solid;border-width:1px;border-color:#444444;background: #222222;"
		"border-radius:3px;font: normal bold 9pt Micosoft YaHei;text-align:center;color:white;height:6px;}"
		"QProgressBar::chunk{background: #00aa00; border-radius:3px;color:black;}";

	//common button style
	const QString QML_NORMAL_COMMON_BUTTON = "QPushButton{background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
		"stop: 0 #888, stop: 0.35 #222, stop: 0.65 #222,stop: 1.0 #888);border-style:inset;border-radius:10px;color:white;}";

	const QString NORMAL_BUTTON_STYLE = "QPushButton{font-family:Microsoft YaHei;text-align:center;}";

	//
	const QString NORMAL_LINEEDIT = "QLineEdit{border-width:1px;border-style:solid;border-color:#999999;font-family:Microsoft YaHei;font-size:12pt;color:white;}";

	const QString NORMAL_GROUPBOX = "QGroupBox{font-family:Microsoft YaHei; font-size:13pt;color:#999911;text-align:center;border-color:lightGray;border-width:1px;}";

	const QString NORMAL_LABEL_IMAGE = "QLabel{background:#666666;font-family:Microsoft YaHei;font-size:12pt; color:white;}";
	const QString NORMAL_LABEL = "QLabel{font-family:Microsoft YaHei;font-size:12pt; color:#aaaaaa;}";

	const QString NORMAL_RADIO = "QRadioButton{font-family:Microsoft YaHei;font-size:12pt;color:#aaaaaa;}";

	//z自定义拖拽类型
	const QString DRAG_SNAP_TAG = "custom/snap";
	const QString DRAG_THUMB_TAG = "custom/thumb";
};


#endif