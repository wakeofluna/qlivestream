#ifndef CORE_APPLICATION_H_
#define CORE_APPLICATION_H_

#include <QApplication>
#include "core/initializer.h"

class NetworkAccess;
class ProfileFactory;
class StorageAccess;

class Application : public QApplication
{
public:
	Application(int & argc, char ** argv);
	~Application();

	bool notify(QObject * receiver, QEvent * e) override;

private:
	Initializer<ProfileFactory> mProfileFactory;
	Initializer<NetworkAccess> mNetworkAccess;
	Initializer<StorageAccess> mStorageAccess;
};

#endif // CORE_APPLICATION_H_
