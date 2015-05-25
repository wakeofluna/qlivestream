#ifndef FORMS_MAIN_WINDOW_FOLLOWING_H_
#define FORMS_MAIN_WINDOW_FOLLOWING_H_

#include "core/storage_access.h"
#include <QWidget>

class ChannelObject;
class Profile;
template <typename T> class QList;

namespace Ui
{
	class MainWindowFollowing;
}

namespace forms
{

class ChannelObjectWidget;

class MainWindowFollowing : public QWidget, public StorageAccess
{
Q_OBJECT

public:
	explicit MainWindowFollowing(Profile & pProfile, QWidget * parent);
	~MainWindowFollowing();

public slots:
	void refresh();
	void rollup();

signals:
	void selected(ChannelObject * pChannel);

private slots:
	void checkRollup(int pSliderValue);

private:
	void clear();
	void addData(QList<ChannelObject*> && pChannels);

private:
	Ui::MainWindowFollowing * ui;

	Profile & mProfile;
	QList<ChannelObject*> mChannels;
	QList<ChannelObjectWidget*> mWidgets;
	bool mCanRollup;
};

} // namespace forms

#endif // FORMS_MAIN_WINDOW_FOLLOWING_H_
