/* race.h — nd-race's cross-module API: the five playable races, their
 * per-attribute bonuses, and their fight weight.
 *
 * Include this from a module TU that wants to look a race up or assign one,
 * and NOT from nd-race's own src/libnd-race.c: an XY_IMPL and an XY_DECL of
 * the same name in one TU collide, which is the direct replacement for the
 * old `SIC_DECL` + `SIC_DEF` pairing in a single file.
 *
 * Usage:
 *
 *     #include <ttypt/xy-mod.h>     // must come first: injects the xy context
 *     #include <nd/xy.h>            // engine service hooks (nd_get, ...)
 *     #include <nd/race.h>          // this file
 *
 * The consumer does not need to load nd-race itself -- the engine loads every
 * module in mods.load into one region and XY dispatches by name -- but the
 * engine's mods.load must list race, or these forward to a provider that is
 * not there.
 *
 * NOTE: this is a MODULE-OWNED header, not an engine one. The old location was
 * `include/uapi/race.h`; the old `~/nd/module.mk` installed it as
 * `$(PREFIX)/include/nd/race.h`, so `nd/` is this header's home and it is
 * installed here with `FOLDER := nd`.
 *
 * The old header included `<nd/type.h>`, a file that no longer exists. Its only
 * live content for this module was SIC_DECL/SIC_DEF/SIC_CALL (all now XY_*);
 * the types a consumer needs come from `<nd/xy.h>`. Nothing here needs to
 * include it.
 */

#ifndef ND_RACE_H
#define ND_RACE_H

#include <ttypt/xy.h>

/* API */
XY_DECL(int, race_set, unsigned, skid, unsigned, race_id);
XY_DECL(unsigned, race_query, char *, name);

XY_DECL(unsigned, race_add, char *, name, unsigned, str_b,
		unsigned, con_b, unsigned, dex_b,
		unsigned, int_b, unsigned, wiz_b,
		unsigned, cha_b, unsigned, wt);

#endif /* !ND_RACE_H */