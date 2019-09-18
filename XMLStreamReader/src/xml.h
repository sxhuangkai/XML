#ifndef XML_H
#define XML_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QXmlStreamReader>

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	bool readBookInfoFile();
	bool readStaffInfoFile();
private:
	void readBookindexElement();
	void readEntryElement(QTreeWidgetItem *parent);
	void readPageElement(QTreeWidgetItem *parent);

	void readDepartmentElement(QTreeWidgetItem *parent);
	void readEmployeeElement(QTreeWidgetItem *parent);

	void skipUnknownElement();

	QTreeWidget *m_pBookTreeWidget;
	QTreeWidget *m_pStaffTreeWidget;
	QXmlStreamReader m_xmlReader;
};

#endif // XML_H
