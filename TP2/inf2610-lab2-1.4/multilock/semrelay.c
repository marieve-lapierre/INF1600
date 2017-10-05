/*
 * semrelay.c
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <semaphore.h>

#include "semrelay.h"
#include "statistics.h"
#include "multilock.h"
#include "utils.h"

void *semrelay_main(void * data) {
    unsigned long i, j;
    struct experiment * e = data;
        
    for (i = 0; i < e->outer; i++) {
        // TODO: attendre notre tour
        sem_wait(e->lock + e->rank* sizeof(sem_t));
        for (j = 0; j < e->inner; j++) {
            unsigned long idx = (i * e->inner) + j;
            statistics_add_sample(e->data, (double) idx);
        }
        // TODO: signaler le travailleur suivant
        sem_post(e->lock + e->rank* sizeof(sem_t));
    }
    
    sem_post(e->lock + (e->rank+1) * sizeof(sem_t));
    return NULL;
}

void semrelay_init(struct experiment * e) {
    int i;

    e->data = make_statistics();
    // TODO: allocation d'un tableau de sémaphores sem_t dans e->lock
    e->lock = malloc(e->nr_thread * sizeof(sem_t));
    // TODO: initialisation des sémaphores
   int valeurInitial = 0;
    for (int i = 0; i < e->nr_thread; i++) {
       valeurInitial = 0;
       if(i == 0){
        valeurInitial = 1;
       }       
       sem_init(e->lock+ i* sizeof(sem_t), 0, valeurInitial);
   }
}

void semrelay_destroy(struct experiment * e) {
    int i;

    // copie finale dans e->stats
    statistics_copy(e->stats, e->data);
    free(e->data);

    // TODO: destruction du verrou
    for (int i = 0; i < e->nr_thread; i++)
       sem_destroy(e->lock + i* sizeof(sem_t));
    // TODO: liberation de la memoire du verrou
    free(e->lock);
}

