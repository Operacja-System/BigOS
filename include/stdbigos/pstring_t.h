#ifndef STDBIGOS_PSTRING_T_H
#define STDBIGOS_PSTRING_T_H

/// pstring_t:
/// either (P)ascal-like length based string
/// or (p)edantically checked string
typedef struct {
	size_t len;
	char* data;
} pstring_t;

#endif
