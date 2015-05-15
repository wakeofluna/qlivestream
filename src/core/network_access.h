#ifndef CORE_NETWORK_ACCESS_H_
#define CORE_NETWORK_ACCESS_H_

#include <functional>

class Profile;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QObject;
class QUrl;

template <typename T> class Initializer;

class NetworkAccess
{
public:
	typedef std::function<void (QNetworkReply &)> Receiver;

protected:
	inline NetworkAccess() {}
	inline ~NetworkAccess() {}

	QNetworkRequest networkRequest(Profile & pProfile) const;
	QUrl networkUrl(Profile & pProfile) const;
	void networkGet(QNetworkRequest const & pRequest, Receiver && pReceiver) const;

private:
	friend class Initializer<NetworkAccess>;
	static void initialize();
	static void finalize();
};

#endif // CORE_NETWORK_ACCESS_H_
