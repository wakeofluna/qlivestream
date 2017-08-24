#include "i_profile.h"

#include "config.h"
#include "core/reply_binary.h"

#include <QNetworkRequest>
#include <QSqlQuery>

IProfile::IProfile()
{
	mLoggedIn = false;
	mId = -1;
	mLevel = STREAMER;
	mSelf = nullptr;

	mCanRollupFollowedChannels = true;
	mCanRollupFollowedCategories = true;
	mCanRollupTopCategories = true;
}

IProfile::~IProfile()
{
}

void IProfile::setLastError(QString pError)
{
	mLastError = pError;
}

void IProfile::performPostLogin(DefaultCallback && pCallback)
{
	pCallback();
}

void IProfile::downloadLogo(QUrl const & pUrl, DataCallback && pCallback)
{
	QNetworkRequest lRequest;
	lRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	lRequest.setUrl(pUrl);

	networkGet(lRequest, [this,CAPTURE(pCallback)] (QNetworkReply & pReply)
	{
		ReplyBinary lReply(this, pReply, "Logo");
		pCallback(std::move(lReply.data()));
	});
}

IUser * IProfile::getUserFor(QString pName, bool pCreate)
{
	if (pName.isEmpty())
		return nullptr;

	auto i = mUsers.find(pName);
	if (i != mUsers.end())
		return *i;

	if (!pCreate)
		return nullptr;

	IUser * lUser = newUserFor(pName);
	mUsers.insert(pName, lUser);

	return lUser;
}

ICategory * IProfile::getCategoryFor(QString pName, bool pCreate)
{
	if (pName.isEmpty())
		return nullptr;

	auto i = mCategories.find(pName);
	if (i != mCategories.end())
		return *i;

	if (!pCreate)
		return nullptr;

	ICategory * lCategory = newCategoryFor(pName);
	mCategories.insert(pName, lCategory);

	return lCategory;
}

// XXX -DRY- rollups for Profile
void IProfile::resetFollowedChannels()
{
	mFollowedChannels.clear();
	mCanRollupFollowedChannels = true;
	rollupFollowedChannels();
}

void IProfile::resetFollowedCategories()
{
	mFollowedCategories.clear();
	mCanRollupFollowedCategories = true;
	rollupFollowedCategories();
}

void IProfile::resetTopCategories()
{
	mTopCategories.clear();
	mCanRollupTopCategories = true;
	rollupTopCategories();
}

void IProfile::initProfile()
{
	mSelf = getUserFor(account());
	Q_ASSERT(mSelf != nullptr);
}

void IProfile::updateToken(QString pToken)
{
	QSqlQuery q;
	SQL_PREPARE(q, "update profile set token=? where id=?");
	q.bindValue(0, pToken);
	q.bindValue(1, mId);
	SQL_EXEC(q);
	mToken = pToken;
}
