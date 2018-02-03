#ifndef FORMS_MAIN_ABOUT_H_
#define FORMS_MAIN_ABOUT_H_

#include <QWidget>

namespace Ui
{
	class MainAbout;
}

namespace forms
{

class MainAbout : public QWidget
{
Q_OBJECT

public:
	explicit MainAbout(QWidget *parent = nullptr);
	~MainAbout();

private:
	Ui::MainAbout * ui;
};

} // namespace forms

#endif // FORMS_MAIN_ABOUT_H_
