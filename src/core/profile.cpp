#include "config.h"
#include "profile.h"

#include <QApplication>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

Profile::Profile()
{
	mId = -1;
}

Profile::~Profile()
{
}

bool Profile::isValid() const
{
	return mId >= 0;
}

QStringList Profile::listProfiles()
{
	QStringList lProfiles;

	QSqlQuery q;
	q.prepare("select id,service,name from profile order by name");
	SQL_EXEC(q);

	while (q.next())
		lProfiles.append(q.value(2).toString());

	return lProfiles;
}

Profile Profile::load(QString pName)
{
	Profile lProfile;
	lProfile.mAccount = pName;

	QSqlQuery q;
	q.prepare("select id,token from profile where name=?");
	q.bindValue(0, pName);
	SQL_EXEC(q);

	if (!q.next())
		return lProfile;

	lProfile.mId = q.value(0).toInt();
	lProfile.mToken = q.value(1).toString();

	q.prepare("select scope from profile_scope where profile_id=?");
	q.bindValue(0, lProfile.mId);
	SQL_EXEC(q);

	while (q.next())
	{
		AuthScope lScope = AuthScope::fromString(q.value(0).toString());
		if (lScope.isValid())
			lProfile.mRequested.set(lScope);
	}

	return lProfile;
}

void Profile::erase() const
{
	QSqlQuery q;
	q.prepare("delete from profile where id=?");
	q.bindValue(0, mId);
	SQL_EXEC(q);
}

void Profile::save() const
{
	QSqlQuery q;
	if (mId == -1)
	{
		q.prepare("insert into profile (service,name,token) values (?,?,?)");
		q.bindValue(0, "twitch");
		q.bindValue(1, mAccount);
		q.bindValue(2, mToken);
		SQL_EXEC(q);
		const_cast<Profile*>(this)->mId = q.lastInsertId().toInt();
	}
	else
	{
		q.prepare("update profile set name=?, token=? where id=?");
		q.bindValue(0, mAccount);
		q.bindValue(1, mToken);
		q.bindValue(2, mId);
		SQL_EXEC(q);
	}

	q.prepare("delete from profile_scope where profile_id=?");
	q.bindValue(0, mId);
	SQL_EXEC(q);

	q.prepare("insert into profile_scope (profile_id,scope) values (?,?)");
	q.bindValue(0, mId);
	for (int i = 0; i < AuthScope::max; ++i)
	{
		AuthScope lScope((AuthScope::Scope)i);
		if (mRequested.test(lScope))
		{
			q.bindValue(1, lScope.toString());
			SQL_EXEC(q);
		}
	}
}
