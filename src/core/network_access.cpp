#include "config.h"
#include "network_access.h"
#include "core/profile.h"
#include "forms/debug_network_messages.h"

#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSignalMapper>
#include <QString>
#include <QUrl>
#include <QVariantMap>

namespace
{
	QNetworkAccessManager * mNetwork = nullptr;
	forms::DebugNetworkMessages * mDebugMessages = nullptr;
}

forms::DebugNetworkMessages * NetworkAccess::networkCaptureWindow()
{
	return mDebugMessages;
}

void NetworkAccess::networkLogMessage(QString pTag, QVariant const & pMessage)
{
	if (!mDebugMessages->isCapturing())
		return;

	mDebugMessages->addMessage(pTag, pMessage);
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
	mDebugMessages = new forms::DebugNetworkMessages();
}

void NetworkAccess::finalize()
{
	delete mDebugMessages;
	mDebugMessages = nullptr;

	delete mNetwork;
	mNetwork = nullptr;
}
