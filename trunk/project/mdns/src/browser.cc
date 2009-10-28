/**
 * @file browser.cc
 *
 * @date 2009-10-28
 * @author jldupont
 */
#include "browser.h"


void *            __browser_thread_function(void *params);
DBusHandlerResult __browser_filter_func (DBusConnection *connection, DBusMessage *message, void *user_data);
void __browser_handle_message(DBusMessage *msg, browserParams *bp);

BrowserReturnCode browser_init(OUT browserParams **bp) {

	//if this fails, we aint' going far anyway...
	*bp = (browserParams *) malloc(sizeof(browserParams));

	DBusError error;
	DBusBusType type = DBUS_BUS_SYSTEM;

	dbus_error_init (&error);

	(*bp)->conn = dbus_bus_get (type, &error);
	if (NULL==(*bp)->conn) {
	  dbus_error_free (&error);
	  return BROWSER_DBUS_CONN_ERROR;
	}

	//if this fails, we aint' going far anyway...
	(*bp)->q=queue_create();

	pthread_create(&((*bp)->thread), NULL, &__browser_thread_function, (void *) *bp);

	return BROWSER_OK;
}//

void *
__browser_thread_function(void *vbp) {

	//make shortcuts
	browserParams *bp=(browserParams *) vbp;
	DBusConnection *conn=bp->conn;

	// Configure the filter function
	if (!dbus_connection_add_filter (conn, __browser_filter_func, vbp, NULL)) {
		//@TODO push disconnected message
	}

	while (dbus_connection_read_write_dispatch(conn, 100));

	return NULL;
}//


DBusHandlerResult
__browser_filter_func (DBusConnection *connection,
					DBusMessage     *message,
					void            *vbp)
{
	//DBGLOG(LOG_INFO, "ingress_filter_func, conn: %i  message: %i", connection, message);

	//make shortcuts
	browserParams *bp=(browserParams *) vbp;

	// main message handling starts here
	__browser_handle_message(message, (browserParams *)vbp);

	if (dbus_message_is_signal (message,
							  DBUS_INTERFACE_LOCAL,
							  "Disconnected")) {
		//@TODO
	}


	// We are passing the message along to the Erlang Client...
	return DBUS_HANDLER_RESULT_HANDLED;
}//

void
__browser_handle_message(DBusMessage *msg, browserParams *bp) {

}//

