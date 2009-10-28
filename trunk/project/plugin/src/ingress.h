/**
 * @file ingress.h
 *
 *  Created on: 2009-10-27
 *      Author: jldupont
 */

#ifndef INGRESS_H_
#define INGRESS_H_

    #include "dbus/dbus.h"
    #include "queue.h"

	typedef struct {

		DBusConnection *conn;
		pthread_t *thread;

	} threadParams;

	// PROTOTYPES
	// ===========
	void ingress_init(DBusConnection *conn, pthread_t *thread, queue **q);



#endif /* INGRESS_H_ */
