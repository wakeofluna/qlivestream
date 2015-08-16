#include "config.h"
#include "application.h"
#include "core/exception.h"
#include "core/network_access.h"
#include "core/storage_access.h"
#include "forms/debug_network_messages.h"
#include "forms/proxy_authentication.h"
#include "forms/ssl_errors.h"

#include <QDebug>
#include <QIcon>
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
#include "core/profile_factory.h"

#ifdef Q_OS_WIN
#include <QLibrary>
// https://msdn.microsoft.com/en-gb/library/windows/desktop/ms724435%28v=vs.85%29.aspx
#define WIN32_LEAN_AND_MEAN
#define SECURITY_WIN32
#include <windows.h>
#include <security.h>
#include <secext.h>
#endif

Application::Application(int & argc, char ** argv) : QApplication(argc, argv)
{
	qsrand(QDateTime::currentDateTimeUtc().toTime_t());

	mGotUsername = false;
	mDebugMessages = new forms::DebugNetworkMessages();
	mLastAuthMethod = 0;
	mLastStatusMessage = 0;

	setWindowIcon(QIcon(":/icons/application"));

	QObject::connect(NetworkAccess::mNetworkAccessManager, &QNetworkAccessManager::proxyAuthenticationRequired, this, &Application::proxyAuthenticationRequired);
	QObject::connect(NetworkAccess::mNetworkAccessManager, &QNetworkAccessManager::sslErrors, this, &Application::sslErrors);
}

Application::~Application()
{
	delete mDebugMessages;
}

QString Application::username() const
{
	if (!mGotUsername)
	{
		const_cast<Application*>(this)->mUsername = determineUsername();
		const_cast<Application*>(this)->mGotUsername = true;
		qDebug() << "Desktop username:" << mUsername;
	}

	return mUsername;
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

void Application::logNetworkMessage(QString const & pTag, QVariant const & pMessage)
{
	if (!mDebugMessages->isCapturing()) return;
	mDebugMessages->addMessage(pTag, pMessage);
}

void Application::logNetworkError(QString const & pTag, QString const & pError, QVariant const & pMessage)
{
	if (!mDebugMessages->isCapturing()) return;
	mDebugMessages->addError(pTag, pError, pMessage);
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

void Application::logStatusUpdate(int & pIdent, QString const & pMessage)
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

QString Application::determineUsername()
{
	QString lUsername;

#ifdef Q_OS_WIN
	typedef BOOLEAN (*f_GetUserNameExW)(EXTENDED_NAME_FORMAT,LPWSTR,PULONG);

	QLibrary lSecur32("secur32.dll");
	f_GetUserNameExW lGetUserNameExW = (f_GetUserNameExW) lSecur32.resolve("GetUserNameExW");
	if (lGetUserNameExW)
	{
		WCHAR lBuffer[256];
		ULONG lSize = sizeof(lBuffer) / sizeof(TCHAR);
		if ((*lGetUserNameExW)(NameSamCompatible, lBuffer, &lSize) != 0)
			lUsername = QString::fromUtf16((const ushort*)lBuffer, lSize);
	}
#endif

	if (lUsername.isEmpty())
		lUsername = qgetenv("USER");
	if (lUsername.isEmpty())
		lUsername = qgetenv("USERNAME");

	return lUsername;
}

void Application::statusBarDestroyed(QObject * pObject)
{
	popStatusBar(static_cast<QStatusBar*>(pObject));
}

void Application::proxyAuthenticationRequired(QNetworkProxy const & proxy, QAuthenticator * authenticator)
{
	while (true)
	{
		switch (++mLastAuthMethod)
		{
			case 1:
				// Attempt stored credentials. I actually dont want to do this.. store a password? No way!
				if (false)
				{
					// Access settings
					return;
				}
				break;

			case 2:
				// Empty username = try the local single signon feature
				authenticator->setUser(QString());
				return;

			case 3:
			{
				// Try just a username and no password
				QString lUsername = username();
				if (!lUsername.isEmpty())
				{
					authenticator->setUser(lUsername);
					return;
				}

				break;
			}

			case 4:
			{
				// Popup a dialog to enter credentials
				forms::ProxyAuthentication * lDialog = new forms::ProxyAuthentication();
				lDialog->setUsername(username());

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
					return;
				}

				break;
			}

			case 5:
				qCritical() << "Ran out of options, failed to authenticate to proxy";
				break;

			default:
				return;
		}
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

