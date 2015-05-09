#ifndef FORMS_PERFORM_LOGIN_SUB_H_
#define FORMS_PERFORM_LOGIN_SUB_H_

#include <QWidget>

class Profile;

namespace Ui
{
	class PerformLoginAcquire;
}

namespace forms
{

class SubPanelAcquire : public QWidget
{
Q_OBJECT

public:
	SubPanelAcquire(Profile & pProfile, QWidget * parent);
	~SubPanelAcquire();

private slots:
	void on_btnAcquire_clicked();
	void on_txtToken_textChanged(QString const & pString);
	void on_btnToken_clicked();

signals:
	void onSetToken(QString pToken);

private:
	Profile & mProfile;
	Ui::PerformLoginAcquire * ui;
};

} // namespace forms

#endif // FORMS_PERFORM_LOGIN_SUB_H_
