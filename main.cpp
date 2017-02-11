#include "ui/zViewer.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QSurfaceFormat>
#include <QRect>
#include <QFile>
#include <QTextCodec>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//obtain desktop available size
	QDesktopWidget* desktop = QApplication::desktop();
	QRect windowRect = QApplication::desktop()->availableGeometry();
	QRect clientRect(windowRect.left(), windowRect.top(), windowRect.width() - 8, windowRect.height() - 8);

	//main window
	zViewer w(clientRect.width(),clientRect.height(),clientRect.left(),clientRect.top());
	w.setWindowFlags(Qt::CustomizeWindowHint);
	w.move( - 2, clientRect.top() - 2);
	w.resize(clientRect.size());
	w.show();

	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));

	//load custome style sheet file
	QFile styleFile("style.qss");
	styleFile.open(QIODevice::ReadOnly);
	QString setStyleSheet(styleFile.readAll());
	a.setStyleSheet(setStyleSheet);

	return a.exec();
}
