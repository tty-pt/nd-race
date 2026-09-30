## 1.0.0

- **nd-race is now an installable library rather than a build artifact of
  the engine.** It builds and installs exactly two files,
  `lib/libnd-race.so` and `include/nd/race.h`, following the same layout as
  `axil-tty` and `axil-auth`, and the same layout `nd-core` was converted to
  first. Previously `make` produced a `race.so` named by the engine's
  `mods.load` and installed nothing. There is no `lib/nd-race.so` symlink:
  `mods.load` names this module `libnd-race`, the installed filename, and
  `module_load_path()` only appends `.so`.

- **The link line is libxylem alone.** `LDLIBS := -lxylem`; the engine is not
  linked. `NEEDED` is `libxylem.so` and `libc.so.6`.

- **The race API is declared in `<nd/race.h>`.** The five races, their
  per-attribute bonuses and their fight weight live there; this TU defines
  `ATTR_IMPL` because it co-implements `attr_stat` (base value + race bonus
  via `nd_last()`) from `<nd/attr.h>`.

- **`stat` became `attr_stat`.** The bare name collides with libc's `stat(2)`
  once `<sys/stat.h>` is in the TU.

- **First consumer of the engine's `nd_assoc` XY hook.** The name→race_id
  association goes through `nd_assoc` (secondary `race_rhd` linked to primary
  `race_hd`), resolving both handles via `hd_resolve()`.

- **`fighter_wt` deliberately returns only the racial weight**, with no
  `nd_last()` term — an open call, not an omission.

- **Dropped the `nd-mod.mk` dependency.** `nd-mod.mk` has now been deleted.
