#ifndef FORMS_CHANNEL_OBJECT_WIDGET_H_
#define FORMS_CHANNEL_OBJECT_WIDGET_H_

#include <QWidget>
class QByteArray;
class ChannelObject;

namespace Ui
{
	class ChannelObjectWidget;
}

namespace forms
{

class ChannelObjectWidget : public QWidget
{
Q_OBJECT

public:
	ChannelObjectWidget(ChannelObject * pObject, QWidget * parent);
	~ChannelObjectWidget();

	inline ChannelObject * object() const { return mChannel; }

	void setLogo(QByteArray const & pData);

public slots:
	void updateFromObject();

signals:
	void clicked(ChannelObject * pCategory = nullptr);

private slots:
	void on_btnChannel_clicked();

private:
	Ui::ChannelObjectWidget * ui;
	ChannelObject * mChannel;
};

} // namespace forms

#endif // FORMS_CHANNEL_OBJECT_WIDGET_H_
