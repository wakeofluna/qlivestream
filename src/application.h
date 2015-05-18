#ifndef CORE_APPLICATION_H_
#define CORE_APPLICATION_H_

#include <QApplication>
#include "core/initializer.h"

class NetworkAccess;
class ProfileFactory;
class StorageAccess;

namespace forms
{
	class DebugNetworkMessages;
};

class Application : public QApplication, public Logger
{
public:
	Application(int & argc, char ** argv);
	~Application();

	bool notify(QObject * receiver, QEvent * e) override;

	QWidget * networkCaptureWindow() const override;
	void logNetworkMessage(QString pTag, QVariant const & pMessage) override;
	void logNetworkError(QString pTag, QString const & pMessage) override;

private:
	Initializer<ProfileFactory> mProfileFactory;
	Initializer<NetworkAccess> mNetworkAccess;
	Initializer<StorageAccess> mStorageAccess;

	forms::DebugNetworkMessages * mDebugMessages;
};

#endif // CORE_APPLICATION_H_
