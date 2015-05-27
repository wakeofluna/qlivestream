#ifndef CORE_APPLICATION_H_
#define CORE_APPLICATION_H_

#include <QApplication>
#include "core/initializer.h"
#include "core/logger.h"

class NetworkAccess;
class ProfileFactory;
class StorageAccess;
class QAuthenticator;
class QNetworkProxy;
class QNetworkReply;
class QSslError;
template <typename T> class QList;

namespace forms
{
	class DebugNetworkMessages;
};

class Application : public QApplication, public Logger
{
Q_OBJECT

public:
	Application(int & argc, char ** argv);
	~Application();

	QString username() const;

	bool notify(QObject * receiver, QEvent * e) override;

	QWidget * networkCaptureWindow() const override;
	void logNetworkMessage(QString pTag, QVariant const & pMessage) override;
	void logNetworkError(QString pTag, QString const & pMessage) override;

	void pushStatusBar(QStatusBar * pStatusBar) override;
	void popStatusBar(QStatusBar * pStatusBar) override;
	void logStatusUpdate(int & pIdent, QString pMessage) override;
	void logStatusClear(int pIdent) override;

private:
	static QString determineUsername();
	void statusBarDestroyed(QObject * pObject);
	void proxyAuthenticationRequired(QNetworkProxy const & proxy, QAuthenticator * authenticator);
	void sslErrors(QNetworkReply * reply, QList<QSslError> const & errors);

private:
	Initializer<ProfileFactory> mProfileFactory;
	Initializer<NetworkAccess> mNetworkAccess;
	Initializer<StorageAccess> mStorageAccess;

	forms::DebugNetworkMessages * mDebugMessages;

	QString mUsername;
	bool mGotUsername;

	QList<QStatusBar*> mStatusBars;
	int mLastAuthMethod;
	int mLastStatusMessage;
};

#endif // CORE_APPLICATION_H_
