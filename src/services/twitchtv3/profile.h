#ifndef SERVICES_TWITCHTV3_PROFILE_H_
#define SERVICES_TWITCHTV3_PROFILE_H_

#include <algorithm>
#include <QByteArray>
#include <QMutex>
#include <QNetworkRequest>
#include <QQueue>
#include <QString>
#include <QVariant>
#include "auth_scope.h"
#include "chat_server.h"

#include "../../core/class_bitset.h"
#include "../../core/i_profile.h"
#include "../../core/network_access.h"

namespace twitchtv3
{

class Category;
class Channel;
class Profile : public IProfile
{
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
		inline PendingRequest(QNetworkRequest const& pRequest, QByteArray const& pData, Receiver && pReceiver, Type pType) : mRequest(pRequest), mData(pData), mCallback(std::move(pReceiver)), mType(pType) {}
		QNetworkRequest mRequest;
		QByteArray mData;
		Receiver mCallback;
		Type mType;
	};

public:
	Profile();
	~Profile();

	inline AuthScopes scopes() const { return mScopes; }
	inline bool hasScope(AuthScope::Scope pScope) const { return mScopes.test(pScope); }

	QUrl acquireTokenUrl() const override;
	void performLogin(DefaultCallback && pCallback) override;

	void rollupFollowedChannels() override;
	void rollupFollowedCategories() override;
	void rollupTopCategories() override;

	// Networking
	QUrl apiUrl(QString pAppend) const;
	QUrl krakenUrl(QString pAppend = QString()) const;
	QNetworkRequest serviceRequest(bool pAuthed = true) const;

	void throttledGet(QNetworkRequest const & pRequest, Receiver && pReceiver);
	void throttledPost(QNetworkRequest const & pRequest, QByteArray const& pData, Receiver && pReceiver);
	void throttledPut(QNetworkRequest const & pRequest, QByteArray const& pData, Receiver && pReceiver);

	inline ChatServerPtr chatServer() const { return mChatServer; }

	Channel * processChannel(QVariant pValue);
	Category * processCategory(QVariant pValue);

private:
	IUser * newUserFor(QString pName) override;
	ICategory * newCategoryFor(QString pName) override;
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
