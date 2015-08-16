#include "reply_text.h"

#include <QRegExp>
#include <QVariant>
#include "logger.h"

ReplyText::ReplyText(IProfile * pProfile, QNetworkReply & pReply, QString pTag) : ReplyBase(pProfile, pReply, pTag)
{
	if (checkNetworkStatus())
	{
		QByteArray lBytes = readByteArray();
		QString lData = QString::fromUtf8(lBytes);

		mData = lData.split(QRegExp("\r\n|\n"));

		if (!mData.isEmpty() && mData.last().isEmpty())
			mData.pop_back();
	}

	log();
}

ReplyText::~ReplyText()
{

}

void ReplyText::log() const
{
	if (!hasError())
		Logger::get()->logNetworkMessage(tag(), mData);
	else
		Logger::get()->logNetworkError(tag(), lastError(), QVariant());
}
