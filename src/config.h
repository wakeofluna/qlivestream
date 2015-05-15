#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "core/exception.h"

#define APP_NAME "Qlivestream"

// Helper for lambda captures
#if defined(CDT_PARSER) || __cplusplus < 201402L
#define CAPTURE(x) x
#else
#define CAPTURE(x) x = std::move(x)
#endif

#define SQL_EXEC(q) do { if (!q.exec()) throw q; } while (0)

#endif // _CONFIG_H_
