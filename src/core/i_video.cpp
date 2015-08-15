#include "i_video.h"

IVideo::IVideo(IChannel & pChannel, QVariant pId) : QObject(&pChannel), mChannel(pChannel), mId(pId)
{
	mLength = -1;
	mViews = -1;
}

IVideo::~IVideo()
{
}

bool IVideo::operator< (IVideo const & pOther) const
{
	return mRecorded > pOther.mRecorded;
}
