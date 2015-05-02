#ifndef FORMS_EDIT_PROFILE_H_
#define FORMS_EDIT_PROFILE_H_

#include <QDialog>
#include <QVector>

class Profile;
class QAbstractButton;
class QCheckBox;
class QSettings;

namespace Ui
{
	class EditProfile;
}

namespace forms
{

class EditProfile : public QDialog
{
Q_OBJECT

public:
	explicit EditProfile(Profile & pProfile, QWidget * parent = 0);
	~EditProfile();

private slots:
	void on_txtName_textChanged(QString const & pText);
	void on_txtAccount_textChanged(QString const & pText);
	void on_btnBox_clicked(QAbstractButton * pButton);

private:
	void enableSave() const;

	Ui::EditProfile * ui;
	QVector<QCheckBox*> mCheckboxes;

	Profile & mProfile;
	QString mOriginalName;
};

} // namespace forms

#endif // FORMS_EDIT_PROFILE_H_
