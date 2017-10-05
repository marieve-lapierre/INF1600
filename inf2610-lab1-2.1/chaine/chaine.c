/*
 * chaine.c
 *
 *  Created on: 2013-08-15
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include "whoami.h"
#include <pth.h>

/* Astuce:
 * Conversion de string à nombre: atoi()
 * Conversion de nombre à string: asprintf()
 */

void *baz(void* arg) {
	increment_rank();
	whoami("baz");
    return NULL;
}

void *foo(void* arg) {
	increment_rank();
	whoami("foo");    
    return NULL;
}

void *bar(void* arg) {
	increment_rank();
	whoami("bar");
    return NULL;
}


int main(int argc, char **argv) {
    pth_init();
    pth_t tbaz, tfoo, tbar;
	int n = 2;
	if (argc == 2) {
	    n = atoi(argv[1]);
	}

	// ajoute le répertoire courant dans $PATH
	add_pwd_to_path(argv[0]);

	increment_rank();
	whoami("chaine");
	// Exécution de n cycles foo bar baz
	int i;
        
	for (i = 0; i < n; i++) {
        
        tbaz = pth_spawn(PTH_ATTR_DEFAULT, baz, NULL);
        pth_join(tbaz, NULL);
        tfoo = pth_spawn(PTH_ATTR_DEFAULT, foo, NULL);
        pth_join(tfoo, NULL);
        tbar = pth_spawn(PTH_ATTR_DEFAULT, bar, NULL);
        pth_join(tbar, NULL);
	}
	return 0;
}


