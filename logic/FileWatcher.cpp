#include "FileWatcher.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>

FileWatcher* FileWatcher::m_Instance = NULL;

FileWatcher::FileWatcher()
	: m_WatcherDir("")
	, m_Watcher(new QFileSystemWatcher())
{
	connect(m_Watcher, SIGNAL(directoryChanged(QString)), this, SLOT(files_changed(QString)));
}

FileWatcher::~FileWatcher()
{
}

FileWatcher* FileWatcher::instance()
{
	if (m_Instance == NULL)
		m_Instance = new FileWatcher;
	return m_Instance;
}

void FileWatcher::set_path(QString path)
{
	m_WatcherDir = path; 
	m_Watcher->addPath(m_WatcherDir);
	
	m_OldFileList.clear();
	QDir dir(m_WatcherDir);
	m_OldFileList = dir.entryList();
}

void FileWatcher::files_changed(QString path)
{
	QDir dir(m_WatcherDir);
	QStringList newList = dir.entryList();
	for (int i = 0; i < newList.size(); ++i)
	{
		QString name = newList.at(i);
		if (m_OldFileList.contains(name))
			continue;
		QFileInfo info(m_WatcherDir + "/" + name);
		if (info.isDir())
		{
			emit NewDirFound(m_WatcherDir + "/" + name);
		}
		else if (info.isFile())
		{
			emit NewFileFound(m_WatcherDir + "/" + name);
		}
	}

	m_OldFileList = newList;
}