#ifndef FORMS_CHANNEL_INFO_H_
#define FORMS_CHANNEL_INFO_H_

#include <QWidget>
#include <QString>
class ChannelObject;

namespace Ui
{
	class ChannelInfo;
}

namespace forms
{

class ChannelInfo : public QWidget
{
Q_OBJECT

public:
	explicit ChannelInfo(ChannelObject & pChannel, QWidget * parent);
	~ChannelInfo();

protected slots:
	void updateFromChannel();

private:
	Ui::ChannelInfo * ui;
	ChannelObject & mChannel;
};

} // namespace forms

#endif // FORMS_CHANNEL_INFO_H_
