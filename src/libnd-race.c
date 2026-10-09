/* src/libnd-race.c — nd-race, ported to libxylem.
 *
 * Owns the five playable races (human, elf, dwarf, halfling, half-orc): their
 * per-attribute bonuses, their fight weight, and the name -> id index. A new
 * entity is assigned a random race on on_add, unless its skeleton maps to a
 * fixed one via race_set().
 *
 * Original: tty-pt/nd-race @ 134 lines main.c, from the nd-basics
 * superproject.
 *
 * This TU XY_IMPLs race_set, race_query, race_add, attr_stat, on_add,
 * on_status and fighter_wt, and so must NOT include include/nd/race.h or
 * nd/hooks.h -- an XY_IMPL and an XY_DECL of the same name in one TU is the XY
 * equivalent of the old SIC_DEF + SIC_DECL collision.
 *
 * Two names changed shape in the port:
 *
 *   - `stat` -> `attr_stat`. `<ttypt/xy.h>` pulls in `<sys/stat.h>`
 *     transitively, so a module-visible function named `stat` collides with
 *     libc's stat(2) -- the same rename nd-attr made. attr_stat is also the
 *     hook nd-attr now XY_IMPLs, so this is the co-implementor that adds the
 *     race's bonus on top of the base value.
 *
 *   - `sic_last(&last)` -> `nd_last(&last)`. The old call read the previous
 *     implementor's return so this module could ADD to it; nd_last() reads the
 *     in-flight predecessor, which only works because libxylem's dispatch now
 *     publishes xy_last_ran per module. nd-attr (loaded first) is the base of
 *     the chain; this is its co-implementor.
 */

#include <ttypt/xy-mod.h>

#include <nd/xy.h>

#include <stdlib.h>
#include <string.h>

/* nd-attr's XY_DECLs are switched off for this TU: nd-race CO-IMPLEMENTS
 * attr_stat (it adds the race bonus to attr's base value), so it XY_IMPLs a
 * name attr.h also XY_DECLs. ATTR_IMPL is the guard nd-attr's own provider TU
 * uses, reused here for the same reason -- an XY_IMPL and an XY_DECL of one
 * name in a single TU collide. */
#include <nd/attr-types.h>

typedef struct {
	char name[32];
	unsigned attr_bonus[ATTR_MAX];
	unsigned wt;
} race_t;

static unsigned race_hd, race_rhd, race_id_hd, race_sid_hd;
static unsigned race_max = 0;

/* API. XY_IMPL both defines the function and emits the dispatch adapter, so
 * each name gets exactly one, with its body -- no forward declarations.
 *
 * Order matters below: XY_IMPL emits a definition, so a caller has to come
 * after its callee. */

XY_IMPL(int, race_set, unsigned, skid, unsigned, race_id)
{
	return (int)nd_put(race_sid_hd, &skid, &race_id);
}

XY_IMPL(unsigned, race_query, char *, name)
{
	unsigned ret = NOTHING;

	nd_get(race_rhd, &ret, name);
	return ret;
}

/* Co-implementor of nd-attr's attr_stat chain: base value + race bonus.
 * nd_last() gives us whatever ran before us in this dispatch. */
XY_IMPL(unsigned, attr_stat, unsigned, ref, enum attribute, at)
{
	race_t race;
	unsigned race_id, last;

	nd_get(race_id_hd, &race_id, &ref);
	nd_get(race_hd, &race, &race_id);

	nd_last(&last);
	return last + race.attr_bonus[at];
}

XY_IMPL(int, on_add, unsigned, ref, unsigned, type, uint64_t, v)
{
	unsigned race_id;
	OBJ obj;

	if (type != TYPE_ENTITY)
		return 1;

	(void) v;
	nd_get(HD_OBJ, &obj, &ref);
	if (nd_get(race_sid_hd, &race_id, &obj.skid))
		race_id = random() % 5;

	nd_put(race_id_hd, &ref, &race_id);
	return 0;
}

XY_IMPL(unsigned, race_add, char *, name, unsigned, str_b,
		unsigned, con_b, unsigned, dex_b,
		unsigned, int_b, unsigned, wiz_b,
		unsigned, cha_b, unsigned, wt)
{
	race_t race = {
		.attr_bonus = {
			str_b, con_b, dex_b,
			int_b, wiz_b, cha_b
		},
		.wt = wt,
	};

	strlcpy(race.name, name, sizeof(race.name));
	return race_max = (unsigned)nd_put(race_hd, NULL, &race);
}

XY_IMPL(int, on_status, unsigned, ref)
{
	race_t race;
	unsigned race_id;

	nd_get(race_id_hd, &race_id, &ref);
	nd_get(race_hd, &race, &race_id);

	nd_printf(ref, "Race\t%8s, bstr %3u, bcon %3u, bdex %3u, bint %3u, bwis %3u, bcha %3u\n",
			race.name,
			race.attr_bonus[ATTR_STR],
			race.attr_bonus[ATTR_CON],
			race.attr_bonus[ATTR_DEX],
			race.attr_bonus[ATTR_INT],
			race.attr_bonus[ATTR_WIZ],
			race.attr_bonus[ATTR_CHA]);
	return 0;
}

/* the nd_assoc secondary-index callback: index each race row by its name */
static int race_assoc(const void ** const skey,
		const void * const key,
		const void * const data)
{
	(void) key;
	*skey = ((const race_t *) data)->name;
	return 0;
}

/* Co-implementor of nd-fight's fighter_wt chain: base weight + race weight. */
XY_IMPL(unsigned, fighter_wt, unsigned, ref)
{
	race_t race;
	unsigned race_id;

	nd_get(race_id_hd, &race_id, &ref);
	nd_get(race_hd, &race, &race_id);

	return race.wt;
}

XY_MODULE_API void
xy_install(void)
{
	nd_len_reg("race", sizeof(race_t));
	race_hd = (unsigned)nd_open("race", "u", "race", ND_AINDEX);
	race_rhd = (unsigned)nd_open("race_rhd", "s", "u", ND_SEC | ND_PGET);
	nd_assoc((int)race_rhd, race_hd, race_assoc);
	race_id_hd = (unsigned)nd_open("race_id", "u", "u", 0);
	race_sid_hd = (unsigned)nd_open("race_sid", "u", "u", 0);

	{
		unsigned wt_punch = nd_put(HD_WTS, NULL, "punch");

		race_add("human", 1, 1, 1, 1, 1, 1, wt_punch);
		race_add("elf", 0, 0, 2, 0, 0, 0, wt_punch);
		race_add("dwarf", 0, 2, 0, 0, 0, 0, wt_punch);
		race_add("halfling", 0, 0, 0, 0, 0, 0, wt_punch);
		race_add("half-orc", 2, 1, 0, 0, 0, 0, wt_punch);
	}
}