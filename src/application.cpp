#include "config.h"
#include "application.h"
#include "core/exception.h"
#include "core/network_access.h"
#include "core/profile_factory.h"
#include "core/storage_access.h"
#include "forms/debug_network_messages.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QTextStream>


Application::Application(int & argc, char ** argv) : QApplication(argc, argv)
{
	mDebugMessages = new forms::DebugNetworkMessages();
}

Application::~Application()
{
	delete mDebugMessages;
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

QWidget * Application::networkCaptureWindow() const
{
	return mDebugMessages;
}

void Application::logNetworkMessage(QString pTag, QVariant const & pMessage)
{
	if (!mDebugMessages->isCapturing()) return;
	mDebugMessages->addMessage(pTag, pMessage);
}

void Application::logNetworkError(QString pTag, QString const & pMessage)
{
	if (!mDebugMessages->isCapturing()) return;
	mDebugMessages->addError(pTag, pMessage);
}
