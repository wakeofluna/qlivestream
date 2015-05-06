#ifndef CORE_NETWORK_ACCESS_H_
#define CORE_NETWORK_ACCESS_H_

class Profile;
class QNetworkAccessManager;
class QNetworkRequest;
class QObject;
class QUrl;

class NetworkAccess
{
public:
	NetworkAccess();
	~NetworkAccess();

protected:
	QNetworkRequest networkRequest(Profile * pProfile) const;
	QUrl networkUrl() const;
	void networkGet(QNetworkRequest const & pRequest, QObject * pReceiver, char const * pSlot) const;

private:
	static QNetworkAccessManager * mNetwork;
};

#endif // CORE_NETWORK_ACCESS_H_
