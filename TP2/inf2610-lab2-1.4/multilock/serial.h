/*
 * serial.h
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#ifndef SERIAL_H_
#define SERIAL_H_

void * serial_main(void * data);
void serial_init(struct experiment * e);
void serial_destroy(struct experiment * e);

#endif /* SERIAL_H_ */
