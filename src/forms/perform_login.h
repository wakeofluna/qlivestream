#ifndef FORMS_PERFORM_LOGIN_H_
#define FORMS_PERFORM_LOGIN_H_

#include "core/network_access.h"
#include "replies/token_check.h"
#include <QDialog>

class Profile;

namespace Ui
{
	class PerformLogin;
}

namespace forms
{

class PerformLogin : public QDialog, public NetworkAccess
{
Q_OBJECT

public:
	explicit PerformLogin(Profile & pProfile, QWidget * parent = 0);
	~PerformLogin();

protected:
	void showEvent(QShowEvent * event);

private:
	void proceed();
	void restart();
	void runStep();

private:
	Ui::PerformLogin * ui;

	Profile & mProfile;
	int mStep;
	QWidget * mSubPanel;
	TokenCheck::Ptr mTokenCheck;
};

} // namespace forms

#endif // FORMS_PERFORM_LOGIN_H_
