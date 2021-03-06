/*
 * spinlock.h
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#ifndef SPINLOCK_H_
#define SPINLOCK_H_

#include "multilock.h"

void *spinlock_main(void * data);
void spinlock_init(struct experiment * e);
void spinlock_destroy(struct experiment * e);

#endif /* SPINLOCK_H_ */
