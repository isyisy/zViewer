
//contains commenly used words

#ifndef CCDICTIONARY_H
#define CCDICTIONARY_H

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QTableWidget>

class CCDictionary : public QWidget
{
	Q_OBJECT

public:
	CCDictionary(QWidget *parent=0);
	~CCDictionary();
public:
	void LoadDictionary(QString filePath);
signals:
	void AppendEntries(QString);
public slots: 
	void OnDictSelected(QTableWidgetItem*);
protected:
	void resizeEvent(QResizeEvent*);
private:
	void init_window();
private:
	QGroupBox* m_DictGroup;
	QTableWidget* m_DictTable;
	int current_row;
};

#endif // CCDICTIONARY_H
