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
		BROWSER_MALLOC_ERROR,
	} BrowserReturnCode;

	/**
	 * Bidir Communication Channel
	 */
	typedef struct {
		queue *in;
		queue *out;
	} CommChannel;


	typedef struct {

		DBusConnection *conn;
		pthread_t thread;

		CommChannel ch;

		/*
		// Browser --> Caller
		queue *q;

		// Caller --> Browser
		queue *qin;
		*/

	} browserParams;

	// PROTOTYPES
	// ===========
	BrowserReturnCode browser_init(OUT browserParams **bp);


#endif /* BROWSER_H_ */
