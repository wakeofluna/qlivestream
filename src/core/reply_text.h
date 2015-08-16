#ifndef SERVICES_REPLY_TEXT_H_
#define SERVICES_REPLY_TEXT_H_

#include <QStringList>
#include "reply_base.h"

#include "../config.h"

class COREDLL ReplyText : public ReplyBase
{
public:
	ReplyText(IProfile * pProfile, QNetworkReply & pReply, QString pTag);
	~ReplyText();

	inline QStringList const & data() const { return mData; }

protected:
	void log() const override;

	QStringList mData;
};

#endif // SERVICES_REPLY_TEXT_H_
