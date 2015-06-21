#ifndef SERVICES_TWITCHTV3_PROFILE_H_
#define SERVICES_TWITCHTV3_PROFILE_H_

#include "core/profile.h"
#include "auth_scope.h"
#include "chat_server.h"

#include <QMutex>
#include <QNetworkRequest>
#include <QQueue>
#include <QSharedData>
class CategoryObject;
class QTimer;
class QUrl;

namespace twitchtv3
{

class Channel;
class Profile : public ::Profile
{
Q_OBJECT

private:
	typedef ChatServer::Ptr ChatServerPtr;

	struct PendingRequest
	{
		enum Type
		{
			GET,
			POST,
			PUT
		};
		inline PendingRequest(QNetworkRequest const& pRequest, Receiver && pReceiver, Type pType = GET) : mRequest(pRequest), mCallback(std::move(pReceiver)), mType(pType) {}
		inline PendingRequest(QNetworkRequest const& pRequest, QByteArray pData, Receiver && pReceiver, Type pType) : mRequest(pRequest), mData(pData), mCallback(std::move(pReceiver)), mType(pType) {}
		QNetworkRequest mRequest;
		QByteArray mData;
		Receiver mCallback;
		Type mType;
	};

public:
	Profile();
	~Profile();

	QUrl acquireTokenUrl() const override;
	void performLogin(DefaultCallback && pCallback) override;
	void getFollowedCategories(int pStart, int pLimit, CategoryCallback && pCallback) override;
	void getTopCategories(int pStart, int pLimit, CategoryCallback && pCallback) override;
	void getFollowedChannels(int pStart, int pLimit, ChannelCallback && pCallback) override;
	void getCategoryChannels(CategoryObject * pCategory, int pStart, int pLimit, ChannelCallback && pCallback) override;
	ChannelObject * getChannelFor(QString pName) override;

	// Networking
	QUrl apiUrl(QString pAppend) const;
	QUrl krakenUrl(QString pAppend = QString()) const;
	QNetworkRequest serviceRequest(bool pAuthed = true) const;

	void throttledGet(QNetworkRequest const & pRequest, Receiver && pReceiver);
	void throttledPost(QNetworkRequest const & pRequest, QByteArray const& pData, Receiver && pReceiver);
	void throttledPut(QNetworkRequest const & pRequest, QByteArray const& pData, Receiver && pReceiver);

	inline ChatServerPtr chatServer() const { return mChatServer; }

private:
	void throttlePing();

	AuthScopes mScopes;

	ChatServerPtr mChatServer;

	QQueue<PendingRequest*> mPendingRequests;
	QTimer * mPendingTimer;
	QMutex mPendingMutex;
	int mPendingPoints;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_PROFILE_H_
