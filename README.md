# axil-nd-race

`nd-race` for [axil-nd](../axil-nd), ported from SIC to libxylem.

Owns the five playable races: their per-attribute bonuses, their fight
weight, and the name↔id association. It co-implements nd-attr's `attr_stat`
chain (base value + race bonus, read through `nd_last()`), implements
`fighter_wt` with the racial weight, and is the first and only consumer of
the engine's `nd_assoc` XY hook.

## Install

```sh
make install
```

Installs:

```
lib/libnd-race.so
include/nd/race.h
```

There is deliberately no `lib/nd-race.so` symlink (see `axil-nd-wts` for
why: `mods.load` names the installed filename, and the OpenBSD packing list
never lists a symlink).

Also packaged for deb, apk, rpm, brew and openbsd from a `v*` tag.

## Build from source

```sh
make
```

Needs [libxylem](https://github.com/tty-pt/libxylem) and the engine's game
API, `<nd/xy.h>`, plus `<nd/attr.h>` for the chain this module co-implements.
Note the `Makefile` carries **no sibling `-I`**: `<nd/attr.h>` resolves from
the **installed** `nd-attr` package (or a checkout at `../axil-nd-attr` only
if the compiler already finds it), so build against an installed engine and
installed `nd-attr`, or add the `-I` yourself for a fully uninstalled tree.
CI names the deps explicitly (`axil-nd,libxylem,nd-attr`).

```sh
git clone https://github.com/tty-pt/nd-race && cd nd-race
make
```

## What it does

* `xy_install()` registers the `race` table (auto-indexed), the `race_id`
  and `race_sid` maps, and the `race_rhd` secondary index, then associates
  names to ids through the engine's `nd_assoc` hook.
* `race_set` / `race_query` / `race_add` assign and look races up;
  `attr_stat` adds the racial bonus on top of the chain value; `fighter_wt`
  answers the racial weight; `on_add` and `on_status` cover creation and
  display.

## Testing

There is no `test.sh` here. Behaviour is asserted by the engine's own suite:

```sh
cd ../axil-nd
make && ./test.sh
```

## Notes from the port

* `SIC_DEF` → `XY_IMPL`, `mod_install` → `xy_install`, `sic_last(&last)` →
  `nd_last(&last)`. `stat` became `attr_stat`: the bare name collides with
  libc's `stat(2)` (pulled in via `<sys/stat.h>`).
* This TU `XY_IMPL`s `attr_stat` from `<nd/attr.h>`, so it defines
  `ATTR_IMPL` before including it.
* `fighter_wt` deliberately returns only the racial weight with no
  `nd_last()` term — an open call, not an omission.
* The link line is libxylem alone. `NEEDED` is `libxylem.so` and `libc.so.6`.

## License

BSD 2-Clause, carried over from `tty-pt/nd-race`. See `LICENSE`.
