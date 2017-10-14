/*
 * bar.c
 *
 *  Created on: 2013-08-15
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "whoami.h"

int main(int argc, char **argv) {
    char compteur = *argv[1];
	increment_rank();
	whoami("bar");
    
    int n = compteur;
    n = n - 1;
    
    if(n != '0'){
        char i = n;
        printf(" %c ", i);
        char *args[] = {"baz", &i, NULL};
        execv("baz", args);
    }
	return 0;
}
