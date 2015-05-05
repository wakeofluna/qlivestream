#ifndef CORE_NETWORK_ACCESS_H_
#define CORE_NETWORK_ACCESS_H_

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>

class Profile;

class NetworkAccess
{
public:
	NetworkAccess();
	~NetworkAccess();

protected:
	QNetworkAccessManager * network() const;
	QNetworkRequest networkRequest(Profile * pProfile) const;
	QUrl networkUrl() const;

private:
	static QNetworkAccessManager * mNetwork;
};

#endif // CORE_NETWORK_ACCESS_H_
