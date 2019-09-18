#include "xml.h"
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent)
{
	setWindowTitle(tr("XML Stream Reader"));

	m_pBookTreeWidget = new QTreeWidget(this);
	m_pStaffTreeWidget = new QTreeWidget(this);
	QStringList bookHeader,staffHeader;
	bookHeader << "Items" << "Pages";
	staffHeader << "name";
	m_pBookTreeWidget->setHeaderLabels(bookHeader);
	m_pStaffTreeWidget->setHeaderLabels(staffHeader);
	QWidget *widget = new QWidget(this);
	QHBoxLayout *layout = new QHBoxLayout(widget);
	layout->addWidget(m_pBookTreeWidget);
	layout->addWidget(m_pStaffTreeWidget);
	widget->setLayout(layout);
	setCentralWidget(widget);
}

MainWindow::~MainWindow()
{
}

bool MainWindow::readBookInfoFile()
{
	QFile file(":/XML/res/books.xml");
	if (!file.open(QFile::ReadOnly | QFile::Text)) 
	{
		QMessageBox::critical(this, tr("Error"),tr("Cannot read file"));
		return false;
	}
	m_xmlReader.setDevice(&file);
	while (!m_xmlReader.atEnd()) 
	{
		qDebug() << "name = " << m_xmlReader.name();
		if (m_xmlReader.isStartElement()) 
		{
			if (m_xmlReader.name() == "bookindex") 
			{
				readBookindexElement();
			}
			else 
			{
				m_xmlReader.raiseError(tr("Not a valid book file"));
			}
		}
		else 
		{
			m_xmlReader.readNext();
		}
	}
	file.close();
	if (m_xmlReader.hasError()) 
	{
		QMessageBox::critical(this, tr("Error"),tr("Failed to parse file"));
		return false;
	}
	else if (file.error() != QFile::NoError) 
	{
		QMessageBox::critical(this, tr("Error"),tr("Cannot read file"));
		return false;
	}
	return true;
}

void MainWindow::readBookindexElement()
{
	Q_ASSERT(m_xmlReader.isStartElement() && m_xmlReader.name() == "bookindex");
	m_xmlReader.readNext();
	while (!m_xmlReader.atEnd()) 
	{
		qDebug() << "name = " << m_xmlReader.name();
		if (m_xmlReader.isEndElement()) 
		{
			m_xmlReader.readNext();
			break;//跳出bookindex块
		}

		if (m_xmlReader.isStartElement()) 
		{
			if (m_xmlReader.name() == "entry") 
			{
				readEntryElement(m_pBookTreeWidget->invisibleRootItem());
			}
			else 
			{
				skipUnknownElement();
			}
		}
		else 
		{
			qDebug()<<"name = "<<m_xmlReader.name();
			m_xmlReader.readNext();
		}
	}
}

void MainWindow::readEntryElement(QTreeWidgetItem *parent)
{
	QTreeWidgetItem *item = new QTreeWidgetItem(parent);
	qDebug() << "attribute = " << m_xmlReader.attributes().value("term").toString();
	item->setText(0, m_xmlReader.attributes().value("term").toString());

	m_xmlReader.readNext();
	while (!m_xmlReader.atEnd()) 
	{
		qDebug() << "name = " << m_xmlReader.name();//每读取完一个标签/一对标签后reader.name()都是""
		if (m_xmlReader.isEndElement()) 
		{
			m_xmlReader.readNext();
			break;
		}

		if (m_xmlReader.isStartElement()) 
		{
			if (m_xmlReader.name() == "entry") 
			{
				readEntryElement(item);
			}
			else if (m_xmlReader.name() == "page") 
			{
				readPageElement(item);
			}
			else 
			{
				skipUnknownElement();
			}
		}
		else 
		{
			m_xmlReader.readNext();
		}
	}
}

void MainWindow::readPageElement(QTreeWidgetItem *parent)
{
	QString page = m_xmlReader.readElementText();
	if (m_xmlReader.isEndElement()) 
	{
		qDebug() << "name = " << m_xmlReader.name();
		m_xmlReader.readNext();
	}

	QString allPages = parent->text(1);
	if (!allPages.isEmpty()) 
	{
		allPages += ", ";
	}
	allPages += page;
	parent->setText(1, allPages);
}

void MainWindow::skipUnknownElement()
{
	m_xmlReader.readNext();
	while (!m_xmlReader.atEnd()) 
	{
		if (m_xmlReader.isEndElement()) 
		{
			m_xmlReader.readNext();
			break;
		}

		if (m_xmlReader.isStartElement()) 
		{
			skipUnknownElement();
		}
		else 
		{
			m_xmlReader.readNext();
		}
	}
}

bool MainWindow::readStaffInfoFile()
{
	QFile file(":/XML/res/staff.xml");
	if (!file.open(QFile::Text | QFile::ReadOnly))
	{
		QMessageBox::critical(this, "Error", QString("Can not open this file"));
		return false;
	}
	QTreeWidgetItem *item = m_pStaffTreeWidget->invisibleRootItem();
	m_xmlReader.setDevice(&file);
	while (!m_xmlReader.atEnd())
	{
		qDebug() << "name = " << m_xmlReader.name();
		if(m_xmlReader.isStartElement())
		{
			if (m_xmlReader.name() == "Department")
			{
				readDepartmentElement(item);
			}
		}
		else
		{
			m_xmlReader.readNext();
		}
	}
	file.close();
	if (m_xmlReader.hasError())
	{
		QMessageBox::critical(this, "Error", "Has analysis in this file!");
		return false;
	}
	return true;
}

void MainWindow::readDepartmentElement(QTreeWidgetItem *parent)
{
	QStringList itemText;
	itemText << m_xmlReader.attributes().value("Name").toString();
	QTreeWidgetItem *item = new QTreeWidgetItem(parent, itemText);

	m_xmlReader.readNext();
	while (!m_xmlReader.atEnd())
	{
		qDebug() << "name = " << m_xmlReader.name();
		if (m_xmlReader.isEndElement())//如果此条件为真，reader.name()为</Department>，此条件用于结束Department标签内的内容
		{
			m_xmlReader.readNext();
			break;
		}//结束
		if (m_xmlReader.isStartElement())
		{
			if (m_xmlReader.name() == "Employee")
			{
				readEmployeeElement(item);
			}
			else if (m_xmlReader.name() == "Department")
			{
				readDepartmentElement(item);
			}
		}
		else
		{
			m_xmlReader.readNext();
		}
	}
}

void MainWindow::readEmployeeElement(QTreeWidgetItem *parent)
{
	QStringList itemText;
	itemText << m_xmlReader.attributes().value("EName").toString();
	QTreeWidgetItem *item = new QTreeWidgetItem(parent, itemText);

	qDebug() << "token = " << m_xmlReader.tokenType();//4
	m_xmlReader.readNext();
	qDebug() << "name=" << m_xmlReader.name();//"Employee"
	qDebug() << "token = " << m_xmlReader.tokenType();//5
	m_xmlReader.readNext();
	qDebug() << "token = " << m_xmlReader.tokenType();//4，执行readDepartmentElement()的break语句
}