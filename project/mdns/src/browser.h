/*
 * browser.h
 *
 *  Created on: 2009-10-28
 *      Author: jldupont
 */

#ifndef BROWSER_H_
#define BROWSER_H_

    #include <pthread.h>

    #include "macros.h"
    #include "dbus/dbus.h"
    #include "queue.h"


	typedef enum {
		BROWSER_OK = 0,
		BROWSER_INVALID_CODE,
		BROWSER_DBUS_CONN_ERROR,
	} BrowserReturnCode;

	typedef struct {

		DBusConnection *conn;
		pthread_t thread;
		queue *q;

	} browserParams;

	// PROTOTYPES
	// ===========
	BrowserReturnCode browser_init(OUT browserParams **bp);


#endif /* BROWSER_H_ */
