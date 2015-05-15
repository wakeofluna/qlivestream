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
	mLoggedIn = false;
	mId = -1;
	mLevel = STREAMER;
}

Profile::~Profile()
{
}

void Profile::updateToken(QString pToken)
{
	QSqlQuery q;
	q.prepare("update profile set token=? where id=?");
	q.bindValue(0, pToken);
	q.bindValue(1, mId);
	SQL_EXEC(q);
	mToken = pToken;
}
