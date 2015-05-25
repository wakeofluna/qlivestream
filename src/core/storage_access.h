#ifndef CORE_STORAGE_ACCESS_H_
#define CORE_STORAGE_ACCESS_H_

#include <functional>
class QByteArray;
class QDir;
class QString;
class QUrl;

template <typename T> class Initializer;

class StorageAccess
{
public:
	typedef std::function<void (QByteArray const&)> CacheHitCallback;
	typedef std::function<void (CacheHitCallback &&)> CacheMissCallback;

public:
	inline StorageAccess() {}
	inline ~StorageAccess() {}

protected:
	void accessCache(QString const & pCacheString, CacheMissCallback && pMiss, CacheHitCallback && pHit);

private:
	friend class Initializer<StorageAccess>;
	static void initialize();
	static void finalize();
	static void checkDatabaseStructure();

	bool fromCache(QString const & pCacheString, QByteArray & pData) const;
	void insertInCache(QString const & pCacheString, QByteArray const & pData);
};

#endif // CORE_STORAGE_ACCESS_H_
