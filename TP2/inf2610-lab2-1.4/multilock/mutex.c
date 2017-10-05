/*
 * mutex.c
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "statistics.h"
#include "multilock.h"

#include "utils.h"

void * mutex_main(void * data) {
    unsigned long i, j, inner;
    struct experiment * e = data;
    

    for (i = 0; i < e->outer; i++) {
        // TODO: verrouiller
        pthread_mutex_lock(e->lock);
        for (j = 0; j < e->inner; j++) {
            unsigned long idx = (i * e->inner) + j;
            statistics_add_sample(e->data, (double) idx);
        }
        // TODO deverrouiller
        pthread_mutex_unlock(e->lock);
    }

    
    return NULL;
}

void mutex_init(struct experiment * e) {
    e->data = make_statistics();
    // TODO: allocation d'un pthread_mutex_t dans e->lock
    e->lock = malloc(sizeof(pthread_mutex_t));
    // TODO: initialisation du mutex
    pthread_mutex_init(e->lock, NULL);
    
}

void mutex_destroy(struct experiment * e) {
    statistics_copy(e->stats, e->data);
    free(e->data);
    // TODO: destruction du verrou
    pthread_mutex_destroy(e->lock);
    // TODO: liberation de la memoire du verrou
    free(e->lock);
}


