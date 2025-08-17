#include "./include/uapi/race.h"
#include <stdlib.h>

#include <nd/nd.h>
#include <nd/attr.h>

typedef struct {
	char name[32];
	unsigned attr_bonus[ATTR_MAX];
	unsigned wt;
} race_t;

unsigned race_hd, race_rhd, race_id_hd, race_sid_hd, race_max = 0;
unsigned wt_punch;

SIC_DEF(int, race_set, unsigned, skid, unsigned, race_id);
SIC_DEF(unsigned, race_query, char *, name);
SIC_DEF(unsigned, race_add, char *, name, unsigned, str_b,
		unsigned, con_b, unsigned, dex_b,
		unsigned, int_b, unsigned, wiz_b,
		unsigned, cha_b, unsigned, wt);

int race_set(unsigned skid, unsigned race_id) {
	return nd_put(race_sid_hd, &skid, &race_id);
}

unsigned race_query(char *name) {
	unsigned ret = NOTHING;
	nd_get(race_rhd, &ret, name);
	return ret;
}

unsigned stat(unsigned ref, enum attribute at) {
	race_t race;
	unsigned race_id, last;

	nd_get(race_id_hd, &race_id, &ref);
	nd_get(race_hd, &race, &race_id);

	sic_last(&last);
	return last + race.attr_bonus[at];
}

int on_add(unsigned ref, unsigned type, uint64_t v __attribute__((unused))) {
	unsigned race_id;
	OBJ obj;

	if (type != TYPE_ENTITY)
		return 1;

	nd_get(HD_OBJ, &obj, &ref);
	if (nd_get(race_sid_hd, &race_id, &obj.skid))
		race_id = random() % 5;

	nd_put(race_id_hd, &ref, &race_id);
	return 0;
}

unsigned race_add(char *name, unsigned str_b,
		unsigned con_b, unsigned dex_b,
		unsigned int_b, unsigned wiz_b,
		unsigned cha_b, unsigned wt)
{
	race_t race = {
		.attr_bonus = {
			str_b, con_b, dex_b,
			int_b, wiz_b, cha_b
		},
		.wt = wt,
	};

	strlcpy(race.name, name, sizeof(race.name));
	return race_max = nd_put(race_hd, NULL, &race);
}

int
on_status(unsigned ref)
{
	race_t race;
	unsigned race_id;

	nd_get(race_id_hd, &race_id, &ref);
	nd_get(race_hd, &race, &race_id);

	nd_writef(ref, "Race\t%8s, bstr %3u, bcon %3u, bdex %3u, bint %3u, bwis %3u, bcha %3u\n",
			race.name,
			race.attr_bonus[ATTR_STR],
			race.attr_bonus[ATTR_CON],
			race.attr_bonus[ATTR_DEX],
			race.attr_bonus[ATTR_INT],
			race.attr_bonus[ATTR_WIZ],
			race.attr_bonus[ATTR_CHA]);
	return 0;
}

int race_assoc(const void ** const skey,
		const void * const key __attribute__((unused)),
		const void * const data) {
	*skey = ((race_t *) data)->name;
	return 0;
}

unsigned fighter_wt(unsigned ref) {
	race_t race;
	unsigned race_id;

	nd_get(race_id_hd, &race_id, &ref);
	nd_get(race_hd, &race, &race_id);

	return race.wt;
}

void
mod_open(void) {
	nd_len_reg("race", sizeof(race_t));
	race_hd = nd_open("race", "u", "race", ND_AINDEX);
	race_rhd = nd_open("race_rhd", "s", "u", ND_SEC | ND_PGET);
	nd_assoc(race_rhd, race_hd, &race_assoc);
	race_id_hd = nd_open("race_id", "u", "u", 0);
	race_sid_hd = nd_open("race_sid", "u", "u", 0);
}

void
mod_install(void) {
	mod_open();

	wt_punch = nd_put(HD_WTS, NULL, "punch");

	race_add("human", 1, 1, 1, 1, 1, 1, wt_punch);
	race_add("elf", 0, 0, 2, 0, 0, 0, wt_punch);
	race_add("dwarf", 0, 2, 0, 0, 0, 0, wt_punch);
	race_add("halfling", 0, 0, 2, 0, 0, 0, wt_punch);
	race_add("half-orc", 2, 1, 0, 0, 0, 0, wt_punch);
}
