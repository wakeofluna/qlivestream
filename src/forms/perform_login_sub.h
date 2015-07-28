#ifndef FORMS_PERFORM_LOGIN_SUB_H_
#define FORMS_PERFORM_LOGIN_SUB_H_

#include <QWidget>

class IProfile;
class QAbstractButton;

namespace Ui
{
	class PerformLoginAcquire;
	class PerformLoginError;
	class PerformLoginInvalid;
}

namespace forms
{

class SubPanelAcquire : public QWidget
{
Q_OBJECT

public:
	SubPanelAcquire(IProfile & pProfile, QWidget * parent);
	~SubPanelAcquire();

private slots:
	void on_btnAcquire_clicked();
	void on_txtToken_textChanged(QString const & pString);
	void on_btnToken_clicked();

signals:
	void onSetToken(QString pToken);

private:
	IProfile & mProfile;
	Ui::PerformLoginAcquire * ui;
};


class SubPanelError : public QWidget
{
Q_OBJECT

public:
	SubPanelError(QString pError, QWidget * parent);
	~SubPanelError();

private slots:
	void on_btnError_rejected();

signals:
	void cancel();

private:
	Ui::PerformLoginError * ui;
};


class SubPanelInvalid : public QWidget
{
Q_OBJECT

public:
	SubPanelInvalid(IProfile & pProfile, QWidget * parent);
	~SubPanelInvalid();

private slots:
	void on_btnInvalid_accepted();
	void on_btnInvalid_rejected();

signals:
	void cancel();
	void retry();

private:
	IProfile & mProfile;
	Ui::PerformLoginInvalid * ui;
};


} // namespace forms

#endif // FORMS_PERFORM_LOGIN_SUB_H_
