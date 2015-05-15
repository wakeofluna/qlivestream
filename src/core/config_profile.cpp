#include "config.h"
#include "config_profile.h"
#include <QDateTime>
#include <QSqlQuery>
#include <QVariant>
#include <QVector>

ConfigProfile::ConfigProfile()
{
	mLastAccess = 0;
}

ConfigProfile::~ConfigProfile()
{

}

ConfigProfile::List ConfigProfile::listProfiles()
{
	List lProfiles;

	QSqlQuery q;
	q.prepare("select id,service,name,level,last_access from profile order by name");
	SQL_EXEC(q);

	lProfiles.reserve(q.size() == -1 ? 8 : q.size());
	while (q.next())
	{
		lProfiles.append(ConfigProfile());
		ConfigProfile & lProfile = lProfiles.back();
		lProfile.mId = q.value(0).toInt();
		lProfile.mService = q.value(1).toString();
		lProfile.mAccount = q.value(2).toString();
		lProfile.mLevel = (Level)q.value(3).toInt();
		lProfile.mLastAccess = q.value(4).toInt();
	}

	return lProfiles;
}

QString ConfigProfile::toString() const
{
	return QString('[') + mService + "] " + mAccount;
}

QString ConfigProfile::service() const
{
	return mService;
}

QDateTime ConfigProfile::lastAccess() const
{
	return QDateTime::fromTime_t(mLastAccess);
}

Profile::Ptr ConfigProfile::load()
{
	throw Exception("Error loading profile", "Unknown service identifier: " + mService);

	// Load succesful, update access time

	mLastAccess = QDateTime::currentDateTimeUtc().toTime_t();

	QSqlQuery q;
	q.prepare("update profile set last_access=? where id=?");
	q.bindValue(0, mLastAccess);
	q.bindValue(1, mId);
	SQL_EXEC(q);
}

void ConfigProfile::setAccount(QString pAccount)
{
	mAccount = pAccount;
}

void ConfigProfile::setService(QString pService)
{
	mService = pService;
}

void ConfigProfile::setLevel(Level pLevel)
{
	mLevel = pLevel;
}

void ConfigProfile::erase() const
{
	QSqlQuery q;
	q.prepare("delete from profile where id=?");
	q.bindValue(0, mId);
	SQL_EXEC(q);
}

void ConfigProfile::save()
{
	QSqlQuery q;
	if (mId == -1)
	{
		q.prepare("insert into profile (service,name,level) values (?,?,?)");
		q.bindValue(0, mService);
		q.bindValue(1, mAccount);
		q.bindValue(2, mLevel);
		SQL_EXEC(q);
		mId = q.lastInsertId().toInt();
	}
	else
	{
		q.prepare("update profile set name=?, level=? where id=?");
		q.bindValue(0, mAccount);
		q.bindValue(1, mLevel);
		q.bindValue(2, mId);
		SQL_EXEC(q);
	}
}
