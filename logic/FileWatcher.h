#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QObject>
#include <QString>
#include <QFileSystemWatcher>

class FileWatcher:public QObject
{
	Q_OBJECT
public:
	static FileWatcher* instance();
	~FileWatcher();
	void set_path(QString path);
signals:
	void NewFileFound(QString);
	void NewDirFound(QString);
public slots:
	void files_changed(QString);

private:
	FileWatcher();
private:
	static FileWatcher* m_Instance;
	QString m_WatcherDir;
	QFileSystemWatcher *m_Watcher;

	QStringList m_OldFileList;
};

#endif

