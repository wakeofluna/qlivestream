#include "reply_binary.h"

#include <QVariant>
#include "logger.h"

ReplyBinary::ReplyBinary(IProfile * pProfile, QNetworkReply & pReply, QString pTag) : ReplyBase(pProfile, pReply, pTag)
{
	if (checkNetworkStatus())
		mData = readByteArray();

	log();
}

ReplyBinary::~ReplyBinary()
{

}

void ReplyBinary::log() const
{
	if (!hasError())
		Logger::get()->logNetworkMessage(tag(), QString("(%1 bytes)").arg(mData.size()));
	else
		Logger::get()->logNetworkError(tag(), lastError(), QVariant());
}
