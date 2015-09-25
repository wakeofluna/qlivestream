#ifndef FORMS_MAIN_WINDOW_FOLLOWING_H_
#define FORMS_MAIN_WINDOW_FOLLOWING_H_

#include "core/storage_access.h"
#include <QWidget>

class IChannel;
class IProfile;
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
	explicit MainWindowFollowing(IProfile & pProfile, QWidget * parent);
	~MainWindowFollowing();

public slots:
	void refresh();
	void rollup();

signals:
	void selected(IChannel * pChannel);

private slots:
	void checkRollup(int pSliderValue);
	void onChannelsUpdated();
	void onChannelsOnlineChanged();

private:
	Ui::MainWindowFollowing * ui;

	IProfile & mProfile;
	QList<ChannelObjectWidget*> mOnlineWidgets;
	QList<ChannelObjectWidget*> mOfflineWidgets;
};

} // namespace forms

#endif // FORMS_MAIN_WINDOW_FOLLOWING_H_
