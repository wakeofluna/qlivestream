#ifndef CORE_NETWORK_ACCESS_H_
#define CORE_NETWORK_ACCESS_H_

#include <functional>

class Profile;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QObject;
class QUrl;

class NetworkAccess
{
public:
	typedef std::function<void (QNetworkReply &)> Receiver;

public:
	NetworkAccess();
	~NetworkAccess();

	QNetworkRequest networkRequest(Profile & pProfile) const;
	QUrl networkUrl(Profile & pProfile) const;
	void networkGet(QNetworkRequest const & pRequest, Receiver && pReceiver) const;

private:
	static QNetworkAccessManager * mNetwork;
};

#endif // CORE_NETWORK_ACCESS_H_
