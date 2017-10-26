/*
 * baz.c
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
//printf("argc: %d, argv: %c", argc, *argv[1]);
	increment_rank();
	whoami("baz");
    char *args[] = {"foo", argv[1], NULL};
    execv("foo", args);

	return 0;
}
