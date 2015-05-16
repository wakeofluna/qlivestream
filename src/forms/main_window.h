#ifndef FORMS_MAIN_WINDOW_H_
#define FORMS_MAIN_WINDOW_H_

#include "core/profile.h"
#include "core/storage_access.h"
#include <QMainWindow>

namespace Ui
{
	class MainWindow;
}

namespace forms
{

class MainWindow : public QMainWindow, public StorageAccess
{
Q_OBJECT

public:
	MainWindow(Profile::UPtr && pProfile, QWidget *parent = 0);
	~MainWindow();

public slots:
	void refreshFollowing();
	void rollupFollowing();
	void refreshGames();
	void rollupGames();

signals:
	void updatedFollowing();
	void updatedGames();

protected:
	void closeEvent(QCloseEvent * event) override;

private slots:
	void on_mnuFileLogout_triggered();
	void on_mnuFileExit_triggered();
	void on_mnuHelpDebugNetwork_triggered();
	void on_mnuHelpAbout_triggered();
	void on_mnuHelpAboutQt_triggered();

	void on_btnFollowingRefresh_clicked();
	void on_btnGamesRefresh_clicked();

private:
	Ui::MainWindow * ui;

	Profile::UPtr mProfile;
};

} // namespace forms

#endif // FORMS_MAIN_WINDOW_H_
