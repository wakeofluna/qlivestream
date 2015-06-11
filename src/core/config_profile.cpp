#include "config.h"
#include "config_profile.h"
#include <QDateTime>
#include <QSqlQuery>
#include <QVariant>
#include <QVector>

ConfigProfile::ConfigProfile()
{
	mId = -1;
	mLevel = Profile::ANONYMOUS;
	mLastAccess = 0;
}

ConfigProfile::~ConfigProfile()
{

}

ConfigProfile::List ConfigProfile::listProfiles()
{
	List lProfiles;

	QSqlQuery q;
	SQL_PREPARE(q, "select id,service,name,level,token,last_access from profile order by name");
	SQL_EXEC(q);

	lProfiles.reserve(q.size() == -1 ? 8 : q.size());
	while (q.next())
	{
		lProfiles.append(ConfigProfile());
		ConfigProfile & lProfile = lProfiles.back();
		lProfile.mId = q.value(0).toInt();
		lProfile.mService = q.value(1).toString();
		lProfile.mAccount = q.value(2).toString();
		lProfile.mLevel = q.value(3).toInt();
		lProfile.mToken = q.value(4).toString();
		lProfile.mLastAccess = q.value(5).toInt();
	}

	return lProfiles;
}

QString ConfigProfile::toString() const
{
	return QString("[%1] %2").arg(mService).arg(mAccount);
}

QString ConfigProfile::service() const
{
	return mService;
}

QDateTime ConfigProfile::lastAccess() const
{
	return QDateTime::fromTime_t(mLastAccess);
}

Profile::UPtr ConfigProfile::load() const
{
	Profile::UPtr lProfile = createProfile(mService);
	if (!lProfile)
		throw Exception("Error loading profile", QString("Unknown service identifier: %1").arg(mService));

	lProfile->mId = mId;
	lProfile->mAccount = mAccount;
	lProfile->mToken = mToken;
	lProfile->mLevel = (Profile::Level)mLevel;

	// Load succesful, update access time
	int lLastAccess = QDateTime::currentDateTimeUtc().toTime_t();

	QSqlQuery q;
	SQL_PREPARE(q, "update profile set last_access=? where id=?");
	q.bindValue(0, lLastAccess);
	q.bindValue(1, mId);
	SQL_EXEC(q);

	return lProfile;
}

void ConfigProfile::setAccount(QString pAccount)
{
	mAccount = pAccount;
}

void ConfigProfile::setService(QString pService)
{
	mService = pService;
}

void ConfigProfile::setLevel(int pLevel)
{
	mLevel = pLevel;
}

void ConfigProfile::erase() const
{
	QSqlQuery q;
	SQL_PREPARE(q, "delete from profile where id=?");
	q.bindValue(0, mId);
	SQL_EXEC(q);
}

void ConfigProfile::save()
{
	QSqlQuery q;
	if (mId == -1)
	{
		SQL_PREPARE(q, "insert into profile (service,name,level) values (?,?,?)");
		q.bindValue(0, mService);
		q.bindValue(1, mAccount);
		q.bindValue(2, mLevel);
		SQL_EXEC(q);
		mId = q.lastInsertId().toInt();
	}
	else
	{
		SQL_PREPARE(q, "update profile set name=?, level=? where id=?");
		q.bindValue(0, mAccount);
		q.bindValue(1, mLevel);
		q.bindValue(2, mId);
		SQL_EXEC(q);
	}
}
