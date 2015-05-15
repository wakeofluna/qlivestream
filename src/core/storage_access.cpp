#include "config.h"
#include "storage_access.h"
#include "core/exception.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

QDir StorageAccess::mStorageCache;
QSqlDatabase StorageAccess::mStorageDatabase;

QDir StorageAccess::cache() const
{
	return mStorageCache;
}

void StorageAccess::initialize()
{
	QSettings lSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationName(), "settings");

	// Access the settings file first to make sure the directory exists
	lSettings.beginGroup("database");
	if (!lSettings.contains("type"))
	{
		lSettings.setValue("type", "sqlite");
		lSettings.sync();
	}

	// Get path of settings (with userscope it should be somewhere in the users home)
	QFileInfo lSettingsFile = lSettings.fileName();
	QDir lConfigPath = lSettingsFile.path();

	// Determine path to local storage cache
	mStorageCache = lConfigPath;
	bool lPathOk = mStorageCache.cd("cache");
	if (!lPathOk)
	{
		mStorageCache.mkdir("cache");
		lPathOk = mStorageCache.cd("cache");
	}
	if (!lPathOk)
		throw Exception("Invalid local storage", "Cannot access/create local storage cache");

	// Access database
	QString lDBType = lSettings.value("type").toString();
	if (lDBType == "sqlite")
	{
		QString lDBName = lConfigPath.filePath("appdata.sqlite");
		mStorageDatabase = QSqlDatabase::addDatabase("QSQLITE");
		mStorageDatabase.setDatabaseName(lDBName);
	}
	else
		throw Exception("Error initializing database", "Unknown database type: " + lDBType);

	if (!mStorageDatabase.open())
		throw Exception("Error opening database", mStorageDatabase.lastError().databaseText());

	// Database post initialization
	if (lDBType == "sqlite")
	{
		QSqlQuery q("PRAGMA foreign_keys = ON;");
		q.finish();
	}

	checkDatabaseStructure();
}

#define SQLERR Exception("Error creating database", q.lastError().text())
void StorageAccess::checkDatabaseStructure()
{
	QStringList lTables = mStorageDatabase.tables();

	QSqlQuery q;
	if (!lTables.contains("profile"))
	{
		q.prepare(
				"create table profile ("
				"id INTEGER PRIMARY KEY AUTOINCREMENT,"
				"service VARCHAR(16),"
				"name VARCHAR(32),"
				"level INTEGER,"
				"token VARCHAR(64),"
				"last_access INTEGER,"
				"UNIQUE (service,name)"
				")"
		);
		if (!q.exec()) throw SQLERR;
	}
}
#undef SQLERR

void StorageAccess::finalize()
{
	mStorageDatabase.close();
}
