#include "reply_binary.h"

#include <QVariant>
#include "logger.h"

ReplyBinary::ReplyBinary(IProfile & pProfile, QNetworkReply & pReply, QString pTag) : ReplyBase(pProfile, pReply), mTag(pTag)
{
	if (!checkNetworkStatus())
		return;

	mData = readByteArray();
}

ReplyBinary::~ReplyBinary()
{

}

QString ReplyBinary::tag() const
{
	return mTag;
}

void ReplyBinary::log() const
{
	if (!hasError())
		Logger::get()->logNetworkMessage(tag(), QString("(%1 bytes)").arg(mData.size()));
	else
		Logger::get()->logNetworkError(tag(), lastError(), QVariant());
}
