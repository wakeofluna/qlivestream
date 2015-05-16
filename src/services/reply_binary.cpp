#include "config.h"
#include "reply_binary.h"

ReplyBinary::ReplyBinary(QNetworkReply & pReply, QString pTag) : ReplyBase(pReply), mTag(pTag)
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
