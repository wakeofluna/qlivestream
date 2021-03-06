#ifndef SERVICES_TWITCHTV_PROFILE_H_
#define SERVICES_TWITCHTV_PROFILE_H_

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

namespace twitchtv
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
			PUT,
			DELETE
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
	void performPostLogin(DefaultCallback && pCallback) override;

	void rollupFollowedChannels() override;
	void rollupFollowedCategories() override;
	void rollupTopCategories() override;
	void getFollowedStreams();
	void getUserInfo(QStringList pName);

	inline QString userID() const { return mUserID; }

	// Networking
	QUrl apiUrl(QString pAppend) const;
	QUrl krakenUrl(QString pAppend = QString()) const;
	QUrl usherUrl(QString pAppend) const;
	QNetworkRequest serviceRequest(bool pAuthed = true, bool pJson = true) const;

	QNetworkReply * synchronisedGet(QNetworkRequest const & pRequest);
	void throttledGet(QNetworkRequest & pRequest, Receiver && pReceiver);
	void throttledPost(QNetworkRequest & pRequest, QByteArray const& pData, Receiver && pReceiver);
	void throttledPut(QNetworkRequest & pRequest, QByteArray const& pData, Receiver && pReceiver);
	void throttledDelete(QNetworkRequest & pRequest, Receiver && pReceiver);

	inline ChatServerPtr chatServer() const { return mChatServer; }

	Channel * processChannel(QVariant pValue);
	Category * processCategory(QVariant pValue);

protected:
	void initProfile() override;

private:
	IUser * newUserFor(QString pName) override;
	ICategory * newCategoryFor(QString pName) override;
	void throttlePing();

	AuthScopes mScopes;
	QString mClientID;
	QString mUserID;

	ChatServerPtr mChatServer;

	QQueue<PendingRequest*> mPendingRequests;
	QTimer * mPendingTimer;
	QMutex mPendingMutex;
	int mPendingPoints;
};

} // namespace twitchtv

#endif // SERVICES_TWITCHTV_PROFILE_H_
