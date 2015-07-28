#include "config.h"
#include "storage_access.h"
#include "core/exception.h"

#include <QByteArray>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QString>

namespace
{
	QDir mStorageCache;
	QSqlDatabase mStorageDatabase;
}

void StorageAccess::accessCache(QString const & pCacheString, CacheMissCallback && pMiss, DataCallback && pHit)
{
	QByteArray lData;
	if (fromCache(pCacheString, lData))
	{
		pHit(lData);
	}
	else
	{
		pMiss([this, pCacheString, CAPTURE(pHit)] (QByteArray const& pData)
		{
			insertInCache(pCacheString, pData);
			pHit(pData);
		});
	}
}

bool StorageAccess::fromCache(QString const & pCacheString, QByteArray & pData) const
{
	QFile lFile(mStorageCache.filePath(pCacheString));
	if (!lFile.exists())
		return false;

	lFile.open(QFile::ReadOnly | QFile::Unbuffered);
	pData = lFile.readAll();
	lFile.close();

	return !pData.isEmpty();
}

void StorageAccess::insertInCache(QString const & pCacheString, QByteArray const & pData)
{
	if (pData.isEmpty())
		return;

	QFile lFile(mStorageCache.filePath(pCacheString));
	lFile.open(QFile::WriteOnly | QFile::Truncate);
	lFile.write(pData);
	lFile.close();
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

void StorageAccess::checkDatabaseStructure()
{
	QStringList lTables = mStorageDatabase.tables();

	QSqlQuery q;
	if (!lTables.contains("profile"))
	{
		SQL_PREPARE(q,
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
		SQL_EXEC(q);
	}

	if (!lTables.contains("sessions"))
	{
		SQL_PREPARE(q,
				"create table sessions ("
				"id INTEGER PRIMARY KEY AUTOINCREMENT,"
				"profile_id INTEGER REFERENCES profile (id) ON DELETE CASCADE ON UPDATE CASCADE,"
				"started INTEGER,"
				"finished INTEGER,"
				"stream_title VARCHAR(200),"
				"UNIQUE (profile_id, started)"
				")"
		);
		SQL_EXEC(q);
	}

	if (!lTables.contains("statistics"))
	{
		SQL_PREPARE(q,
				"create table statistics ("
				"session_id INTEGER REFERENCES sessions (id) ON DELETE CASCADE ON UPDATE CASCADE,"
				"current_time INTEGER,"
				"total_subscribers INTEGER,"
				"total_followers INTEGER,"
				"total_viewers INTEGER,"
				"total_chatters INTEGER,"
				"num_lines_chatted INTEGER,"
				"num_words_chatted INTEGER,"
				"PRIMARY KEY (session_id, current_time)"
				")"
		);
		SQL_EXEC(q);
	}

	if (!lTables.contains("followers"))
	{
		SQL_PREPARE(q,
				"create table followers ("
				"profile_id INTEGER REFERENCES profile (id) ON DELETE CASCADE ON UPDATE CASCADE,"
				"created INTEGER,"
				"name VARCHAR(32)"
				")"
		);
		SQL_EXEC(q);

		SQL_PREPARE(q, "create index followers_idx1 on followers (profile_id, created)");
		SQL_EXEC(q);

		SQL_PREPARE(q, "create index followers_idx2 on followers (profile_id, name, created)");
		SQL_EXEC(q);
	}

	if (!lTables.contains("subscribers"))
	{
		SQL_PREPARE(q,
				"create table subscribers ("
				"profile_id INTEGER REFERENCES profile (id) ON DELETE CASCADE ON UPDATE CASCADE,"
				"created INTEGER,"
				"name VARCHAR(32)"
				")"
		);
		SQL_EXEC(q);

		SQL_PREPARE(q, "create index subscribers_idx1 on subscribers (profile_id, created)");
		SQL_EXEC(q);

		SQL_PREPARE(q, "create index subscribers_idx2 on subscribers (profile_id, name, created)");
		SQL_EXEC(q);
	}

	if (!lTables.contains("chatters"))
	{
		SQL_PREPARE(q,
				"create table chatters ("
				"session_id INTEGER REFERENCES sessions (id) ON DELETE CASCADE ON UPDATE CASCADE,"
				"name VARCHAR(32),"
				"joined INTEGER,"
				"parted INTEGER,"
				"num_lines INTEGER,"
				"num_highlights INTEGER"
				")"
		);
		SQL_EXEC(q);

		SQL_PREPARE(q, "create index chatters_idx1 on chatters (session_id, name, joined)");
		SQL_EXEC(q);
	}
}
