#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QSettings>

class PacsServer
{
public:
	QString ae_title;
	QString ip_address;
	QString port;
	QString description;
public:
	PacsServer(){}
	PacsServer(QString aet,QString ip,QString p,QString explain)
	{
		ae_title = aet;
		ip_address = ip;
		port = p;
		description = explain;
	}
	PacsServer(const PacsServer& ps)
	{
		ae_title = ps.ae_title;
		ip_address = ps.ip_address;
		port = ps.port;
		description = ps.description;
	}
	const PacsServer& operator=(const PacsServer& ps)
	{
		ae_title = ps.ae_title;
		ip_address = ps.ip_address;
		port = ps.port;
		description = ps.description;
		return *this;
	}
};



class Configuration
{
public:
	~Configuration(){}
	static Configuration* instance();
	void init_profile(QString path);
	//pacs
	void read_pacsinfo();
	void edit_pacsinfo(int index,PacsServer ps);
	void write_pacsinfo();
	void add_pacsinfo(PacsServer ps);
	void remove_pacsinfo(int index);
	QList<PacsServer> get_pacsinfo();//interface for user

	//local
	void read_local();
	void set_title(QString);
	QString get_title(){ return m_MyTitle; }
	void set_listening_port(QString);
	QString get_listening_port(){ return m_MyPort; }

	//directory
	void read_dirs();
	void set_download_dir(QString str);
	QString get_download_dir(){ return m_DownloadDir; }
	void set_upload_dir(QString str);
	QString get_upload_dir(){ return m_UploadDir; }

private:
	Configuration();

private:
	static Configuration* m_Configuration;
	QSettings *settings;
	QList<PacsServer> m_PacsServerList;

	QString m_DownloadDir;
	QString m_UploadDir;

	QString m_MyTitle;
	QString m_MyPort;
};


#endif