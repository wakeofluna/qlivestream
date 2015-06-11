#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <qcompilerdetection.h>

#define APP_NAME "Qlivestream"
#define APP_NAME_LC "qlivestream"

#ifdef core_EXPORTS
#define COREDLL Q_DECL_EXPORT
#else
#define COREDLL Q_DECL_IMPORT
#endif

// Helper for lambda captures
#if defined(CDT_PARSER) || __cplusplus < 201402L
#define CAPTURE(x) x
#else
#define CAPTURE(x) x = std::move(x)
#endif

#define SQL_PREPARE(q,stmt) do { if (!q.prepare(stmt)) throw q; } while (0)
#define SQL_EXEC(q) do { if (!q.exec()) throw q; } while (0)

#include "core/exception.h"
#include "core/logger.h"

#endif // _CONFIG_H_
