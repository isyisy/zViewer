#ifndef PACSSETDLG_H
#define PACSSETDLG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>

class PacsSetDlg : public QDialog
{
	Q_OBJECT

public:
	PacsSetDlg();
	~PacsSetDlg();
public slots:
	void _add();
	void _edit();
	void _ok();
	void _cancel();
	void server_selected();
protected:
	void resizeEvent(QResizeEvent *);
private:
	void init_window();
	void init_content();
public:
	//pacsœ‡πÿ
	QString m_mytitle;
	QString m_listening_port;
	QString m_peer_title;
	QString m_peer_ip;
	QString m_peer_port;
	QString m_peer_description;

private:
	QLabel* m_Info;

	QLabel* m_MyTitile;
	QLabel* m_ListenPort;
	QLabel* m_PeerTitle;
	QLabel* m_PeerIp;
	QLabel* m_PeerPort;
	QLabel* m_PeerDescription;

	QLineEdit* m_Edit_MyTitile;
	QLineEdit* m_Edit_ListenPort;
	QLineEdit* m_Edit_PeerTitle;
	QLineEdit* m_Edit_PeerIp;
	QLineEdit* m_Edit_PeerPort;
	QLineEdit* m_Edit_PeerDescription;

	QTableWidget* m_PacsTable;
	QPushButton* m_OkBtn;
	QPushButton* m_CancelBtn;
	QPushButton* m_AddBtn;

	
};

#endif // PACSSETDLG_H
