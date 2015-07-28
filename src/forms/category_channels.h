#ifndef FORMS_CATEGORY_CHANNELS_H_
#define FORMS_CATEGORY_CHANNELS_H_

#include <QWidget>
#include <QList>
#include <QString>
#include "core/storage_access.h"
class ICategory;
class IChannel;


namespace Ui
{
	class CategoryChannels;
}

namespace forms
{

class ChannelObjectWidget;
class CategoryChannels : public QWidget, public StorageAccess
{
Q_OBJECT

public:
	explicit CategoryChannels(ICategory & pCategory, QWidget * parent);
	~CategoryChannels();

	inline ICategory & category() const { return mCategory; }

public slots:
	void refresh();
	void rollup();

signals:
	void selected(IChannel * pChannel);

private slots:
	void checkRollup(int pSliderValue);
	void onChannelsUpdated();

private:
	Ui::CategoryChannels * ui;
	ICategory & mCategory;

	QList<ChannelObjectWidget*> mWidgets;
};

}; // namespace forms

#endif // FORMS_CATEGORY_CHANNELS_H_
