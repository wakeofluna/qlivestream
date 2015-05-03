#ifndef FORMS_MAIN_WINDOW_H_
#define FORMS_MAIN_WINDOW_H_

#include "core/profile.h"
#include <QMainWindow>

namespace Ui
{
	class MainWindow;
}

namespace forms
{

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
	MainWindow(Profile pProfile, QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow * ui;

	Profile mProfile;
};

} // namespace forms

#endif // FORMS_MAIN_WINDOW_H_
