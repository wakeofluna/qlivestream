#include "config.h"
#include "i_downloader.h"

IDownloader::IDownloader(QObject * parent) : QObject(parent)
{
	mCurrentChunk = -1;
	mChunkSize = -1;
	mChunkProgress = 0;
	mChunkLength = 0;
}

IDownloader::~IDownloader()
{
}
