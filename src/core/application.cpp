#include "config.h"
#include "application.h"
#include "core/exception.h"
#include "core/network_access.h"
#include "core/profile_factory.h"
#include "core/storage_access.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QTextStream>

Application::Application(int & argc, char ** argv) : QApplication(argc, argv)
{
}

Application::~Application()
{
}

bool Application::notify(QObject * receiver, QEvent * e)
{
	try
	{
		return QApplication::notify(receiver, e);
	}
	catch (Exception & e)
	{
		qCritical() << e.title() << ':' << e.description();
		QMessageBox::critical(nullptr, e.title(), e.description());
	}
	catch (QSqlQuery & q)
	{
		QString s;
		QTextStream str(&s);
		QSqlError e = q.lastError();

		str << "Query: " << q.lastQuery() << '\n';
		str << "Driver text: " << e.driverText() << '\n';
		str << "Database text: " << e.databaseText() << '\n';
		str << "Native error code: " << e.nativeErrorCode();
		qCritical() << "Database error:" << s;
		QMessageBox::critical(nullptr, "Database error", s);
	}

	return false;
}
