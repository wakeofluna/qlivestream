#ifndef CORE_NETWORK_ACCESS_H_
#define CORE_NETWORK_ACCESS_H_

#include <functional>

class Profile;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QObject;
class QString;
class QUrl;
class QVariant;

template <typename T> class Initializer;

namespace forms
{
	class DebugNetworkMessages;
}

class NetworkAccess
{
public:
	typedef std::function<void (QNetworkReply &)> Receiver;

public:
	static forms::DebugNetworkMessages * networkCaptureWindow();
	static void networkLogMessage(QString pTag, QVariant const & pMessage);

protected:
	inline NetworkAccess() {}
	inline ~NetworkAccess() {}

	void networkGet(QNetworkRequest const & pRequest, Receiver && pReceiver) const;

private:
	friend class Initializer<NetworkAccess>;
	static void initialize();
	static void finalize();
};

#endif // CORE_NETWORK_ACCESS_H_
