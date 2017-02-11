#include "PacsSetDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>

#include "logic/Configuration.h"

PacsSetDlg::PacsSetDlg()
	: m_mytitle("VET")
	, m_listening_port("2015")
	, m_peer_title("COMMON")
	, m_peer_ip("127.0.0.1")
	, m_peer_port("104")
	, m_peer_description("")
{
	init_window();
	init_content();
}

PacsSetDlg::~PacsSetDlg()
{
	delete m_Info;
	delete m_MyTitile;
	delete m_ListenPort;
	delete m_PeerTitle;
	delete m_PeerIp;
	delete m_PeerPort;
	delete m_PeerDescription;
	delete m_Edit_MyTitile;
	delete m_Edit_ListenPort;
	delete m_Edit_PeerTitle;
	delete m_Edit_PeerIp;
	delete m_Edit_PeerPort;
	delete m_Edit_PeerDescription;

	delete m_PacsTable;
	delete m_OkBtn;
	delete m_CancelBtn;
	delete m_AddBtn;
}

void PacsSetDlg::init_window()
{
	m_MyTitile = new QLabel(QString::fromStdWString(L"本机Title:"));
	m_ListenPort = new QLabel(QString::fromStdWString(L"监听端口:"));
	m_Edit_MyTitile = new QLineEdit;
	m_Edit_ListenPort = new QLineEdit;
	QHBoxLayout* hy1 = new QHBoxLayout;
	hy1->addWidget(m_MyTitile, 1, Qt::AlignRight);
	hy1->addWidget(m_Edit_MyTitile, 2, Qt::AlignLeft);
	hy1->addWidget(m_ListenPort, 1, Qt::AlignRight);
	hy1->addWidget(m_Edit_ListenPort, 7, Qt::AlignLeft);

	m_PacsTable = new QTableWidget;
	m_PacsTable->setColumnCount(4);
	QStringList liststr;
	liststr << QString::fromStdWString(L"AETitle") << QString::fromStdWString(L"IP地址") << 
			QString::fromStdWString(L"端口号") << QString::fromStdWString(L"描述");
	m_PacsTable->setHorizontalHeaderLabels(liststr);
	m_PacsTable->verticalHeader()->setVisible(false);
	m_PacsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_PacsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_PacsTable->setObjectName("pacs_table");

	m_PeerTitle = new QLabel(QString::fromStdWString(L"服务器Title:"));
	m_Edit_PeerTitle = new QLineEdit;
	m_PeerIp = new QLabel(QString::fromStdWString(L"IP地址:"));
	m_Edit_PeerIp = new QLineEdit;
	m_PeerPort = new QLabel(QString::fromStdWString(L"端口号:"));
	m_Edit_PeerPort = new QLineEdit;
	m_PeerDescription = new QLabel(QString::fromStdWString(L"描述:"));
	m_Edit_PeerDescription = new QLineEdit;
	m_AddBtn = new QPushButton(QString::fromStdWString(L"修改"));
	m_AddBtn->setDisabled(true);

	QHBoxLayout* hy2 = new QHBoxLayout;
	hy2->addWidget(m_PeerTitle, 1,Qt::AlignRight);
	hy2->addWidget(m_Edit_PeerTitle, 2,Qt::AlignLeft);
	hy2->addWidget(m_PeerIp, 1,Qt::AlignRight);
	hy2->addWidget(m_Edit_PeerIp, 2,Qt::AlignLeft);
	hy2->addWidget(m_PeerPort, 1,Qt::AlignRight);
	hy2->addWidget(m_Edit_PeerPort, 2,Qt::AlignLeft);
	hy2->addWidget(m_PeerDescription, 1,Qt::AlignRight);
	hy2->addWidget(m_Edit_PeerDescription, 2,Qt::AlignLeft);
	hy2->addWidget(m_AddBtn, 1);

	m_Info = new QLabel;
	
	m_OkBtn = new QPushButton(QString::fromStdWString(L"确定"));
	m_CancelBtn = new QPushButton(QString::fromStdWString(L"取消"));
	QHBoxLayout* hy3 = new QHBoxLayout;
	hy3->addWidget(m_Info, 6);
	hy3->addWidget(m_CancelBtn, 1, Qt::AlignCenter);
	hy3->addWidget(m_OkBtn, 1, Qt::AlignCenter);

	QVBoxLayout* vl = new QVBoxLayout;
	vl->addLayout(hy1, 1);
	vl->addWidget(m_PacsTable, 5);
	vl->addLayout(hy2, 1);
	vl->addLayout(hy3, 1);

	setStyleSheet("QLineEdit{border-width:1px;border-style:solid;border-color:#999999;}"
		"QLabel{font: normal 13pt Micosoft YaHei; color:white;}");

	connect(m_CancelBtn, SIGNAL(clicked()), this, SLOT(_cancel()));
	connect(m_OkBtn, SIGNAL(clicked()), this, SLOT(_ok()));
	//connect(m_AddBtn, SIGNAL(clicked()), this, SLOT(_add()));
	connect(m_AddBtn, SIGNAL(clicked()), this, SLOT(_edit()));
	connect(m_PacsTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(server_selected()));

	this->setLayout(vl);
	this->setFixedSize(900, 400);
}

void PacsSetDlg::init_content()
{
	m_Edit_MyTitile->setText(Configuration::instance()->get_title());
	m_Edit_ListenPort->setText(Configuration::instance()->get_listening_port());
	QList<PacsServer> list = Configuration::instance()->get_pacsinfo();
	for (int i = 0; i < list.size(); ++i)
	{
		PacsServer ps = list.at(i);
		m_PacsTable->insertRow(i);
		m_PacsTable->setItem(i, 0, new QTableWidgetItem(ps.ae_title));
		m_PacsTable->setItem(i, 1, new QTableWidgetItem(ps.ip_address));
		m_PacsTable->setItem(i, 2, new QTableWidgetItem(ps.port));
		m_PacsTable->setItem(i, 3, new QTableWidgetItem(ps.description));
	}
}

void PacsSetDlg::server_selected()
{
	PacsServer ps;
	int index_row = m_PacsTable->currentRow();
	if (index_row < 0)
		return;
	m_AddBtn->setEnabled(true);
	ps.ae_title = m_PacsTable->item(index_row, 0)->text();
	ps.ip_address = m_PacsTable->item(index_row, 1)->text();
	ps.port = m_PacsTable->item(index_row, 2)->text();
	ps.description = m_PacsTable->item(index_row, 3)->text();
	
	m_Edit_PeerTitle->setText(ps.ae_title);
	m_Edit_PeerIp->setText(ps.ip_address);
	m_Edit_PeerPort->setText(ps.port);
	m_Edit_PeerDescription->setText(ps.description);

}

void PacsSetDlg::_cancel()
{
	QDialog::reject();
}

void PacsSetDlg::_ok()
{
	Configuration::instance()->set_title(m_Edit_MyTitile->text());
	Configuration::instance()->set_listening_port(m_Edit_ListenPort->text());

	if (m_Edit_MyTitile->text() == "")
	{
		if (QMessageBox::information(NULL, QString::fromStdWString(L"提示"),
			QString::fromStdWString(L"确定放弃修改？"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
			return;
	}

	int index = m_PacsTable->currentRow();
	if (index < 0)
	{
		//m_Info->setText(QString::fromStdWString(L"当前未选中服务器，请从列表中选择！"));
		//m_Info->setStyleSheet("QLabel{color:red;}");
		index = 0;
		//return;
	}
	
	m_mytitle = m_Edit_MyTitile->text();
	m_listening_port = m_Edit_ListenPort->text();
	m_peer_title = m_PacsTable->item(index, 0)->text();
	m_peer_ip = m_PacsTable->item(index, 1)->text();
	m_peer_port = m_PacsTable->item(index, 2)->text();
	m_peer_description = m_PacsTable->item(index, 3)->text();

	QDialog::accept();
}

void PacsSetDlg::_add()
{
	PacsServer ps;
	ps.ae_title = m_Edit_PeerTitle->text();
	ps.ip_address = m_Edit_PeerIp->text();
	ps.port = m_Edit_PeerPort->text();
	ps.description = m_Edit_PeerDescription->text();

	Configuration::instance()->add_pacsinfo(ps);
	Configuration::instance()->write_pacsinfo();

	int index = m_PacsTable->rowCount();
	m_PacsTable->insertRow(index);
	m_PacsTable->setItem(index, 0, new QTableWidgetItem(ps.ae_title));
	m_PacsTable->setItem(index, 1, new QTableWidgetItem(ps.ip_address));
	m_PacsTable->setItem(index, 2, new QTableWidgetItem(ps.port));
	m_PacsTable->setItem(index, 3, new QTableWidgetItem(ps.description));
}

void PacsSetDlg::_edit()
{
	int index = m_PacsTable->currentRow();
	if (index < 0)
	{
		QMessageBox::information(NULL, QString::fromStdWString(L"提示"),
			QString::fromStdWString(L"请选中待修改服务器条目！"),
			QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	m_PacsTable->item(index, 0)->setText(m_Edit_PeerTitle->text());
	m_PacsTable->item(index, 1)->setText(m_Edit_PeerIp->text());
	m_PacsTable->item(index, 2)->setText(m_Edit_PeerPort->text());
	m_PacsTable->item(index, 3)->setText(m_Edit_PeerDescription->text());

	PacsServer ps;
	ps.ae_title = m_Edit_PeerTitle->text();
	ps.ip_address = m_Edit_PeerIp->text();
	ps.port = m_Edit_PeerPort->text();
	ps.description = m_Edit_PeerDescription->text();

	Configuration::instance()->edit_pacsinfo(index,ps);
	Configuration::instance()->write_pacsinfo();

	m_AddBtn->setDisabled(true);
	m_Edit_PeerTitle->clear();
	m_Edit_PeerIp->clear();
	m_Edit_PeerPort->clear();
	m_Edit_PeerDescription->clear();
}


void PacsSetDlg::resizeEvent(QResizeEvent* event)
{
	int widget_width = m_PacsTable->width();
	int per_item_width = widget_width / 5;
	QHeaderView* headerView = m_PacsTable->horizontalHeader();
	headerView->resizeSection(0, per_item_width);
	headerView->resizeSection(1, per_item_width);
	headerView->resizeSection(2, per_item_width);
	headerView->setStretchLastSection(true);
	headerView->setDefaultAlignment(Qt::AlignCenter);

}