#ifndef RACE_H
#define RACE_H

#include <nd/type.h>

/* API */
SIC_DECL(int, race_set, unsigned, skid, unsigned, race_id);
SIC_DECL(unsigned, race_query, char *, name);

SIC_DECL(unsigned, race_add, char *, name, unsigned, str_b,
		unsigned, con_b, unsigned, dex_b,
		unsigned, int_b, unsigned, wiz_b,
		unsigned, cha_b, unsigned, wt);

#endif
