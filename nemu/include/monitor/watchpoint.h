#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
        int value;
        char *str;
        char Enb;
        char *type;
	/* TODO: Add more members if necessary */


} WP;

void print_wp();
int check();
WP *new_wp(char*);
void free_wp(int);
#endif
