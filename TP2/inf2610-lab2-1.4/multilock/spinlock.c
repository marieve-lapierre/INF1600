/*
 * spinlock.c
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include "minispinlock.h"
#include "statistics.h"
#include "multilock.h"

/* Spinlock: verrou actif
 *
 * Voir l'implémentation du verrou dans le fichier minispinlock.asm
 * et l'interface dans minispinlock.h
 */
void *spinlock_main(void * data) {
    unsigned long i, j;
    struct experiment * e = data;
    for (i = 0; i < e->outer; i++) {
        // TODO: verrouiller
        lock_mini_spin(&e->lock);
        for (j = 0; j < e->inner; j++) {
            unsigned long idx = (i * e->inner) + j;
            statistics_add_sample(e->data, (double) idx);
        }
        // TODO: deverrouiller
        unlock_mini_spin(&e->lock);
    }
    return NULL;
}

void spinlock_init(struct experiment * e) {
    e->data = make_statistics();
    // TODO: allocation d'un long dans e->lock
    e->lock = malloc(sizeof(long));
    // TODO: initialisation à zéro
    e->lock = 0;    
}

void spinlock_destroy(struct experiment * e) {
    statistics_copy(e->stats, e->data);
    free(e->data);
    // TODO: liberation de la memoire du verrou
    free(e->lock);
}

