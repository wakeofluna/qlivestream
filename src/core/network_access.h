#ifndef CORE_NETWORK_ACCESS_H_
#define CORE_NETWORK_ACCESS_H_

#include "config.h"
#include <functional>

class QAuthenticator;
class QNetworkAccessManager;
class QNetworkProxy;
class QNetworkReply;
class QNetworkRequest;
class QSslError;

template <typename T> class Initializer;
template <typename T> class QList;

namespace forms
{
	class DebugNetworkMessages;
}

class COREDLL NetworkAccess
{
public:
	typedef std::function<void (QNetworkReply &)> Receiver;

public:
	inline ~NetworkAccess() {}


	QNetworkReply * networkGetSync(QNetworkRequest const & pRequest) const;
	QNetworkReply * networkGetAsync(QNetworkRequest const & pRequest) const;
	void networkGet(QNetworkRequest const & pRequest, Receiver && pReceiver, int pRedirection = 0) const;
	void networkPost(QNetworkRequest const & pRequest, QByteArray const & pBytes, Receiver && pReceiver) const;
	void networkPut(QNetworkRequest const & pRequest, QByteArray const & pBytes, Receiver && pReceiver) const;
	void networkDelete(QNetworkRequest const & pRequest, Receiver && pReceiver) const;

protected:
	inline NetworkAccess() {}

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
