#include "CCDictionary.h"
#include <QHeaderView>
#include <QGridLayout>
#include <QFile>
#include <QTextCodec>
#include <QDebug>

CCDictionary::CCDictionary(QWidget *parent)
	: QWidget(parent)
	, current_row(0)
{
	init_window();
}

CCDictionary::~CCDictionary()
{

}

void CCDictionary::init_window()
{
	m_DictTable = new QTableWidget(this);
	m_DictTable->setObjectName("dict_table");
	m_DictTable->setColumnCount(1);
	QStringList header;
	header << QString::fromStdWString(L"常用表述");
	m_DictTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_DictTable->setHorizontalHeaderLabels(header);
	m_DictTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_DictTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_DictTable->verticalHeader()->setVisible(false);
	m_DictTable->horizontalHeader()->setVisible(false);

	LoadDictionary("dict.dat");
	QHBoxLayout* vLayout = new QHBoxLayout;
	vLayout->addWidget(m_DictTable);
	vLayout->setSpacing(0);
	vLayout->setMargin(0);
	setLayout(vLayout);

	connect(m_DictTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(OnDictSelected(QTableWidgetItem*)));
}

void CCDictionary::LoadDictionary(QString filePath)
{
	QTextCodec *codec = QTextCodec::codecForName("GBK");//指定QString的编码方式
	QFile *file = new QFile(filePath);
	file->open(QIODevice::ReadOnly | QIODevice::Text);
	while (!file->atEnd())
	{
		QByteArray array = file->readLine();
		QString str = codec->toUnicode(array);
		m_DictTable->insertRow(current_row);
		m_DictTable->setItem(current_row, 0, new QTableWidgetItem(str));
		current_row++;
	}
}

void CCDictionary::OnDictSelected(QTableWidgetItem* item)
{
	QString str = m_DictTable->item(item->row(), 0)->text();
	emit AppendEntries(str);
}

void CCDictionary::resizeEvent(QResizeEvent* event)
{
	int window_width = width();
	int window_height = height();
	QHeaderView* headerView = m_DictTable->horizontalHeader();
	headerView->resizeSection(0, window_width);
	headerView->setStretchLastSection(true);
	headerView->setDefaultAlignment(Qt::AlignCenter);
}