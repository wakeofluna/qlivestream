#include "config.h"
#include "channel_info.h"
#include "ui_channel_info.h"
#include "core/channel_object.h"

namespace forms
{

ChannelInfo::ChannelInfo(ChannelObject & pChannel, QWidget * parent) : QWidget(parent), mChannel(pChannel)
{
	ui = new Ui::ChannelInfo();
	ui->setupUi(this);

	setWindowTitle(pChannel.name());
	ui->txtChannel->setText(pChannel.name());

	updateFromChannel();
}

ChannelInfo::~ChannelInfo()
{
	delete ui;
}

void ChannelInfo::updateFromChannel()
{
	ui->txtChannel->setText(mChannel.displayName());
	ui->txtTitle->setText(mChannel.status());
	ui->lblNumFollowers->setText(QString::number(mChannel.numFollowers()));
	ui->lblNumViews->setText(QString::number(mChannel.numViews()));
}

} // namespace forms
