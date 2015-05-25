#ifndef FORMS_MAIN_WINDOW_H_
#define FORMS_MAIN_WINDOW_H_

#include "core/profile.h"
#include "core/storage_access.h"

#include <QMainWindow>
class CategoryObject;
class ChannelObject;

namespace Ui
{
	class MainWindow;
}

namespace forms
{

class MainWindowCategories;
class MainWindowFollowing;

class MainWindow : public QMainWindow, public StorageAccess
{
Q_OBJECT

public:
	MainWindow(Profile::UPtr && pProfile, QWidget *parent = 0);
	~MainWindow();

public slots:
	void openCategoryTab(CategoryObject * pCategory);
	void openChannelTab(ChannelObject * pChannel);

protected:
	void closeEvent(QCloseEvent * event) override;

private slots:
	void on_mnuFileLogout_triggered();
	void on_mnuFileExit_triggered();
	void on_mnuViewChannel_triggered();
	void on_mnuViewFollowing_triggered();
	void on_mnuViewCategories_triggered();
	void on_mnuHelpDebugNetwork_triggered();
	void on_mnuHelpAbout_triggered();
	void on_mnuHelpAboutQt_triggered();

	void on_tabWidget_tabCloseRequested(int pIndex);

private:
	Ui::MainWindow * ui;

	Profile::UPtr mProfile;
	forms::MainWindowCategories * mCategories;
	forms::MainWindowFollowing * mFollowing;
};

} // namespace forms

#endif // FORMS_MAIN_WINDOW_H_
