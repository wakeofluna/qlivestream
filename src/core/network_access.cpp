#include "config.h"
#include "network_access.h"
#include "core/profile.h"

#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSignalMapper>
#include <QUrl>

namespace
{
	QNetworkAccessManager * mNetwork = nullptr;
}

void NetworkAccess::networkGet(QNetworkRequest const & pRequest, Receiver && pReceiver) const
{
	QNetworkReply * lReply = mNetwork->get(pRequest);

	QSignalMapper * lMapper = new QSignalMapper(lReply);
	lMapper->setMapping(lReply, lReply);
	QObject::connect(lReply, SIGNAL(finished()), lMapper, SLOT(map()));

	QObject::connect(lMapper, static_cast<void (QSignalMapper::*)(QObject*)>(&QSignalMapper::mapped), [CAPTURE(pReceiver)] (QObject * o)
	{
		QNetworkReply * lReply = qobject_cast<QNetworkReply*>(o);
		lReply->deleteLater();
		pReceiver(*lReply);
	});
}

void NetworkAccess::initialize()
{
	Q_ASSERT_X(mNetwork == nullptr, "NetworkAccess::initialize", "network was already initialised!");
	mNetwork = new QNetworkAccessManager();
}

void NetworkAccess::finalize()
{
	delete mNetwork;
}
