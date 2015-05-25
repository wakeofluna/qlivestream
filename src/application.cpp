#include "config.h"
#include "application.h"
#include "core/exception.h"
#include "core/network_access.h"
#include "core/profile_factory.h"
#include "core/storage_access.h"
#include "forms/debug_network_messages.h"
#include "forms/proxy_authentication.h"
#include "forms/ssl_errors.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QStatusBar>
#include <QString>
#include <QTextStream>
#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>

Application::Application(int & argc, char ** argv) : QApplication(argc, argv)
{
	mDebugMessages = new forms::DebugNetworkMessages();
	mLastAuthMethod = 0;
	mLastStatusMessage = 0;

	QObject::connect(NetworkAccess::mNetworkAccessManager, &QNetworkAccessManager::proxyAuthenticationRequired, this, &Application::proxyAuthenticationRequired);
	QObject::connect(NetworkAccess::mNetworkAccessManager, &QNetworkAccessManager::sslErrors, this, &Application::sslErrors);
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

void Application::pushStatusBar(QStatusBar * pStatusBar)
{
	if (!mStatusBars.contains(pStatusBar))
	{
		mStatusBars.append(pStatusBar);
		connect(pStatusBar, &QWidget::destroyed, this, &Application::statusBarDestroyed);
	}
}

void Application::popStatusBar(QStatusBar * pStatusBar)
{
	mStatusBars.removeAll(pStatusBar);
}

void Application::logStatusUpdate(int & pIdent, QString pMessage)
{
	pIdent = ++mLastStatusMessage;

	if (!mStatusBars.empty())
	{
		QStatusBar * lBar = mStatusBars.back();
		lBar->setProperty("last_ident", pIdent);
		lBar->showMessage(pMessage);
	}
}

void Application::logStatusClear(int pIdent)
{
	for (QStatusBar * lBar : mStatusBars)
	{
		if (lBar->property("last_ident").toInt() == pIdent)
			lBar->clearMessage();
	}
}

void Application::statusBarDestroyed(QObject * pObject)
{
	popStatusBar(qobject_cast<QStatusBar*>(pObject));
}

void Application::proxyAuthenticationRequired(QNetworkProxy const & proxy, QAuthenticator * authenticator)
{
	switch (++mLastAuthMethod)
	{
		case 1:
			// Attempt stored credentials. I actually dont want to do this.. store a password? No way!
			if (false)
			{
				// Access settings
				break;
			}

			++mLastAuthMethod;
			// no break

		case 2:
			// Empty username = try the local single signon feature
			authenticator->setUser(QString());
			break;

		case 3:
		{
			// Popup a dialog to enter credentials
			forms::ProxyAuthentication * lDialog = new forms::ProxyAuthentication();

			int lResult = lDialog->exec();
			QString lUsername = lDialog->username();
			QString lPassword = lDialog->password();
			lDialog->deleteLater();

			if (lResult == QDialog::Accepted)
			{
				authenticator->setUser(lUsername);
				authenticator->setPassword(lPassword);

				// Try this method again if it failed
				--mLastAuthMethod;
			}

			break;
		}

		case 4:
			qCritical() << "Ran out of options, failed to authenticate to proxy";
			break;

		default:
			break;
	}

}

void Application::sslErrors(QNetworkReply * reply, QList<QSslError> const & errors)
{
	forms::SslErrors * lDialog = new forms::SslErrors();
	lDialog->setMessages(errors);

	int lResult = lDialog->exec();
	lDialog->deleteLater();

	if (lResult == QDialog::Accepted)
		reply->ignoreSslErrors();
}

