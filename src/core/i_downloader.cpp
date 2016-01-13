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

QString IDownloader::escapeFilePath(QString pPath)
{
	for (int i = 0; i < pPath.size(); ++i)
	{
		QChar ch = pPath[i];
		if (ch >= '0' && ch <= '9') continue;
		if (ch >= 'a' && ch <= 'z') continue;
		if (ch >= 'A' && ch <= 'Z') continue;
		pPath[i] = '_';
	}
	return pPath;
}
