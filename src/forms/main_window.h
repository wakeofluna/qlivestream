#ifndef FORMS_MAIN_WINDOW_H_
#define FORMS_MAIN_WINDOW_H_

#include "core/profile.h"
#include "core/storage_access.h"

#include <QMainWindow>
class QScrollArea;

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
	void on_mnuViewChannel_triggered();
	void on_mnuViewFollowing_triggered();
	void on_mnuViewGames_triggered();
	void on_mnuHelpDebugNetwork_triggered();
	void on_mnuHelpAbout_triggered();
	void on_mnuHelpAboutQt_triggered();

	void on_tabWidget_tabCloseRequested(int pIndex);
	void on_btnFollowingRefresh_clicked();
	void on_btnGamesRefresh_clicked();

	void checkRollupFollowing(int pSliderValue);
	void checkRollupGames(int pSliderValue);

private:
	bool toggleTabVisible(QWidget * pWidget);
	bool checkRollupFor(QScrollArea * pArea, int pSliderValue);
	void appendCategoryObjects(QVector<CategoryObject*> const& pList, bool pClear = false);

	Ui::MainWindow * ui;

	Profile::UPtr mProfile;
	bool mCanRollupFollowing;
	bool mCanRollupGames;
	int mRollupFollowingOffset;
	int mRollupGamesOffset;
};

} // namespace forms

#endif // FORMS_MAIN_WINDOW_H_
