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
		BROWSER_MALLOC_ERROR,

		BROWSER_DBUS_CONN_ERROR,
		BROWSER_DBUS_FILTER_ERROR,
		BROWSER_DBUS_MATCH_ERROR,
		BROWSER_DBUS_SERVICE_ERROR,

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

		CommChannel *cc;

	} browserParams;

	// PROTOTYPES
	// ===========
	BrowserReturnCode browser_init(CommChannel *cc);




#endif /* BROWSER_H_ */
