#include "config.h"
#include "storage_access.h"
#include "core/exception.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDebug>

namespace
{
	QDir mStorageCache;
	QSqlDatabase mStorageDatabase;
}

QDir StorageAccess::cache() const
{
	return mStorageCache;
}

void StorageAccess::initialize()
{
	bool lPathOk;

	// Check local data location
	QDir lCacheLocation = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
	lPathOk = lCacheLocation.exists();
	if (!lPathOk)
		lPathOk = lCacheLocation.mkpath(".");
	if (!lPathOk)
		throw Exception("Error accessing path", QString("No access to cache data path : %1").arg(lCacheLocation.path()));

	// Save cache path
	mStorageCache = lCacheLocation;

	// Check roaming data location
	QDir lAppDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	lPathOk = lAppDataLocation.exists();
	if (!lPathOk)
		lPathOk = lAppDataLocation.mkpath(".");
	if (!lPathOk)
		throw Exception("Error accessing path", QString("No access to roaming data path : %1").arg(lAppDataLocation.path()));

	// Open/create database
	mStorageDatabase = QSqlDatabase::addDatabase("QSQLITE");
	mStorageDatabase.setDatabaseName(lAppDataLocation.filePath(APP_NAME_LC ".sqlite"));
	if (!mStorageDatabase.open())
		throw Exception("Error opening database", mStorageDatabase.lastError().databaseText());

	// Database post initialization
	QSqlQuery("PRAGMA foreign_keys = ON;");

	checkDatabaseStructure();
}

void StorageAccess::finalize()
{
	mStorageDatabase.close();
	mStorageDatabase = QSqlDatabase();
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
