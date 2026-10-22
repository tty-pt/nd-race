/* race-types.h — nd-race's cross-module types and constants */

#ifndef ND_RACE_TYPES_H
#define ND_RACE_TYPES_H

#include <nd/attr-types.h>

typedef struct {
	char name[32];
	unsigned attr_bonus[ATTR_MAX];
	unsigned wt;
} race_t;

#endif /* !ND_RACE_TYPES_H */
