#include "Configuration.h"
#include <QStringList>
#include <QDebug>

Configuration* Configuration::m_Configuration = NULL;

Configuration::Configuration()
	:settings(new QSettings("settings.ini",QSettings::IniFormat))
{
	read_pacsinfo();
	read_dirs();
	read_local();
}

Configuration* Configuration::instance()
{
	if (m_Configuration == NULL)
		m_Configuration = new Configuration;
	return m_Configuration;
}

void Configuration::init_profile(QString path)
{
	settings->setPath(QSettings::IniFormat, QSettings::Scope::SystemScope, path);
	//read_pacsinfo();
}

void Configuration::read_pacsinfo()
{
	m_PacsServerList.clear();
	int size = settings->beginReadArray("pacs");
	for (int i = 0; i < size; i++)
	{
		settings->setArrayIndex(i);
		PacsServer ps;
		ps.ae_title = settings->value("aetitle").toString();
		ps.ip_address = settings->value("ip_address").toString();
		ps.port = settings->value("port").toString();
		ps.description = settings->value("description").toString();
		m_PacsServerList.push_back(ps);
	}
	settings->endArray();
}

void Configuration::write_pacsinfo()
{
	settings->clear();

	settings->beginWriteArray("pacs");
	QList<PacsServer>::iterator it = m_PacsServerList.begin();
	int index = 0;
	for (it; it != m_PacsServerList.end(); ++it)
	{
	PacsServer ps = (*it);
	settings->setArrayIndex(index);
	settings->setValue("aetitle", ps.ae_title);
	settings->setValue("ip_address", ps.ip_address);
	settings->setValue("port", ps.port);
	settings->setValue("description", ps.description);
	index++;
	}
	settings->endArray();
}

void Configuration::add_pacsinfo(PacsServer ps)
{
	m_PacsServerList.push_back(ps);
}

void Configuration::edit_pacsinfo(int index, PacsServer ps)
{
	m_PacsServerList.removeAt(index);
	m_PacsServerList.insert(index, ps);
}

void Configuration::remove_pacsinfo(int index)
{
	m_PacsServerList.removeAt(index);
}

QList<PacsServer> Configuration::get_pacsinfo()
{
	return m_PacsServerList;
}

void Configuration::read_dirs()
{
	settings->beginGroup("dirs");
	m_DownloadDir = settings->value("download_dir").toString();
	m_UploadDir = settings->value("upload_dir").toString();
	settings->endGroup();
}

void Configuration::set_download_dir(QString str)
{
	m_DownloadDir = str; 
	
	settings->beginGroup("dirs");
	settings->setValue("download_dir", m_DownloadDir);
	settings->endGroup();
}

void Configuration::set_upload_dir(QString str)
{
	m_UploadDir = str;

	settings->beginGroup("dirs");
	settings->setValue("upload_dir", m_UploadDir);
	settings->endGroup();
}

void Configuration::read_local()
{
	settings->beginGroup("local");
	m_MyTitle = settings->value("my_title").toString();
	m_MyPort = settings->value("my_port").toString();
	settings->endGroup();
}

void Configuration::set_title(QString str)
{
	m_MyTitle = str;

	settings->beginGroup("local");
	settings->setValue("my_title", m_MyTitle);
	settings->endGroup();
}

void Configuration::set_listening_port(QString str)
{
	m_MyPort = str;

	settings->beginGroup("local");
	settings->setValue("my_port", m_MyPort);
	settings->endGroup();
}