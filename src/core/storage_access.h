#ifndef CORE_STORAGE_ACCESS_H_
#define CORE_STORAGE_ACCESS_H_

class QDir;
class QSqlDatabase;

template <typename T> class Initializer;

class StorageAccess
{
public:
	inline StorageAccess() {}
	inline ~StorageAccess() {}

	QDir cache() const;

private:
	friend class Initializer<StorageAccess>;
	static void initialize();
	static void checkDatabaseStructure();
	static void finalize();

	static QDir mStorageCache;
	static QSqlDatabase mStorageDatabase;
};

#endif // CORE_STORAGE_ACCESS_H_
