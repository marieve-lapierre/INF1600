/*
 * pagedump.c
 *
 *  Created on: 2013-10-23
 *      Author: francis
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

/*
 * Savegarder la page contenant ptr dans le fichier fname
 */
void save_page(char *fname, void *ptr) {
    (void) fname;
    (void) ptr;
    /*
     * TODO:
     * 1 - calculer l'adresse du début de la page
     * 2 - ouvrir le fichier de destination
     * 3 - écrire la page dans le fichier
     * 4 - fermer le fichier
     */
    FILE* new_file;
    int page_size = getpagesize()- 1;
    int invert_page_size = ~page_size;
    long start_adr_l = (long) ptr & invert_page_size;
    ptr = (void *) start_adr_l;
    
    char *mode_open = "w+";
    if ((new_file = fopen(fname, mode_open)) != NULL) {
            fwrite(ptr, sizeof(char), page_size/sizeof(char), new_file);
            fclose(new_file);
    } else {
        perror("A problem occur when attempting to open the file.");   
        fclose(new_file);    
    }

    return;
}

int main(int argc, char **argv) {
    /*
     * L'utilisation de volatile empêche le compilateur
     * d'optimiser (i.e. supprimer) les variables.
     *
     * (void) var; évite l'avertissement concernant une variable non-utilisée
     *
     */

    /*
     * Variables sur la pile (stack)
     */
    volatile int cafe1 = 0xCAFE1111; (void) cafe1;
    volatile int cafe2 = 0xCAFE2222; (void) cafe2;
    volatile int cafe3[2] = { 0xCAFE3333, 0xCAFE4444 }; (void) cafe3;

    /*
     * Variables sur le monceau (heap)
     */
    volatile int *beef1 = malloc(sizeof(int)); (void) beef1;
    volatile int *beef2 = malloc(sizeof(int)); (void) beef2;
    volatile int *beef3 = malloc(2 * sizeof(int)); (void) beef3;
    *beef1 = 0xBEEF1111;
    *beef2 = 0xBEEF2222;
    beef3[0] = 0xBEEF3333;
    beef3[1] = 0xBEEF4444;

    save_page("beef.page", (void *)beef1);
    save_page("cafe.page", (void *)&cafe1);
    return 0;
}
