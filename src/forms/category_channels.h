#ifndef FORMS_CATEGORY_CHANNELS_H_
#define FORMS_CATEGORY_CHANNELS_H_

#include <QWidget>
#include <QList>
#include <QString>
#include "core/storage_access.h"
class CategoryObject;
class ChannelObject;


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
	explicit CategoryChannels(CategoryObject & pCategory, QWidget * parent);
	~CategoryChannels();

	inline CategoryObject & category() const { return mCategory; }

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
	Ui::CategoryChannels * ui;
	CategoryObject & mCategory;

	QList<ChannelObject*> mChannels;
	QList<ChannelObjectWidget*> mWidgets;
	bool mCanRollup;
};

}; // namespace forms

#endif // FORMS_CATEGORY_CHANNELS_H_
