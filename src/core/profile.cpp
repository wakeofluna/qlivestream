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
	mLevel = STREAMER;
}

Profile::~Profile()
{
}

void Profile::updateToken(QString pToken) const
{
	QSqlQuery q;
	q.prepare("update profile set token=? where id=?");
	q.bindValue(0, mToken);
	q.bindValue(1, mId);
	SQL_EXEC(q);
}
