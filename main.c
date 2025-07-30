#include <stdlib.h>

#include <nd/nd.h>
#include <nd/attr.h>

typedef struct {
	char name[32];
	unsigned attr_bonus[ATTR_MAX];
} race_t;

unsigned race_hd, race_id_hd, race_max = 0;

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

	if (type != TYPE_ENTITY)
		return 1;

	race_id = random() % (race_max + 1);
	nd_put(race_id_hd, &ref, &race_id);
	return 0;
}

static inline void
race_add(char *name, unsigned str_b, unsigned con_b,
		unsigned dex_b, unsigned int_b,
		unsigned wiz_b, unsigned cha_b) {
	race_t race = {
		.attr_bonus = {
			str_b, con_b, dex_b,
			int_b, wiz_b, cha_b
		},
	};

	strlcpy(race.name, name, sizeof(race.name));
	race_max = nd_put(race_hd, NULL, &race);
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

void
mod_open(void) {
	nd_len_reg("race", sizeof(race_t));
	race_hd = nd_open("race", "u", "race", ND_AINDEX);
	race_id_hd = nd_open("race_id", "u", "u", 0);
}

void
mod_install(void) {
	mod_open();

	race_add("human", 1, 1 ,1, 1, 1, 1);
	race_add("elf", 0, 0 ,2, 0, 0, 0);
	race_add("dwarf", 0, 2 ,0, 0, 0, 0);
	race_add("halfling", 0, 0 ,2, 0, 0, 0);
	race_add("half-orc", 2, 1 ,0, 0, 0, 0);
}
