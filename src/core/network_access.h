#ifndef CORE_NETWORK_ACCESS_H_
#define CORE_NETWORK_ACCESS_H_

#include <functional>
#include <QList>

class Profile;
class QAuthenticator;
class QNetworkAccessManager;
class QNetworkProxy;
class QNetworkReply;
class QNetworkRequest;
class QObject;
class QSslError;
class QString;
class QUrl;
class QVariant;

template <typename T> class Initializer;

namespace forms
{
	class DebugNetworkMessages;
}

class NetworkAccess
{
public:
	typedef std::function<void (QNetworkReply &)> Receiver;

protected:
	inline NetworkAccess() {}
	inline ~NetworkAccess() {}

	void networkGet(QNetworkRequest const & pRequest, Receiver && pReceiver, int pRedirection = 0) const;

private:
	static void proxyAuthenticationRequired(QNetworkProxy const & proxy, QAuthenticator * authenticator);
	static void sslErrors(QNetworkReply * reply, QList<QSslError> const & errors);

private:
	friend class Initializer<NetworkAccess>;
	static void initialize();
	static void finalize();

	friend class Application;
	static QNetworkAccessManager * mNetworkAccessManager;
};

#endif // CORE_NETWORK_ACCESS_H_
