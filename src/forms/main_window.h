#ifndef FORMS_MAIN_WINDOW_H_
#define FORMS_MAIN_WINDOW_H_

#include <memory>
#include <QMainWindow>
#include <QMap>
#include <QString>
#include "channel_info.h"

#include "../core/storage_access.h"

class ICategory;
class IChannel;
class IProfile;

namespace Ui
{
	class MainWindow;
}

namespace forms
{

class ChannelInfo;
class MainWindowCategories;
class MainWindowFollowing;

class MainWindow : public QMainWindow, public StorageAccess
{
Q_OBJECT

public:
	MainWindow(std::unique_ptr<IProfile> && pProfile, QWidget *parent = 0);
	~MainWindow();

public slots:
	void openCategoryTab(ICategory * pCategory);
	void openChannelTab(IChannel * pChannel);

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

	std::unique_ptr<IProfile> mProfile;
	forms::MainWindowCategories * mCategories;
	forms::MainWindowFollowing * mFollowing;
	forms::ChannelInfo * mYourChannel;
};

} // namespace forms

#endif // FORMS_MAIN_WINDOW_H_
