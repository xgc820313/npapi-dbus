/**
 * @file   ingress.cc
 *
 * @date   2009-10-26
 * @author jldupont
 */

#include <pthread.h>
#include "ingress.h"
#include "macros.h"

void *__ingress_thread_function(void *conn);
DBusHandlerResult ingress_filter_func (DBusConnection *connection, DBusMessage *message, void *user_data);


/**
 * Egress thread initialization
 */
void ingress_init(DBusConnection *conn, pthread_t *thread, queue **q) {

	//if this fails, we aint' going far anyway...
	*q=queue_create();

	threadParams *params=(threadParams *) malloc(sizeof(threadParams));
	params->conn=conn;
	params->q=*q;

	pthread_create(thread, NULL, &__ingress_thread_function, (void *) params);

}//


void *
__ingress_thread_function(void *params) {

	//make shortcuts
	threadParams *p=(threadParams *)params;
	DBusConnection *conn=p->conn;
	queue *q=p->q;

	// Configure the filter function
	if (!dbus_connection_add_filter (conn, ingress_filter_func, NULL, NULL)) {
		//@TODO push disconnected message
	}

	//@TODO clean-up params!
}//


DBusHandlerResult
ingress_filter_func (DBusConnection *connection,
					DBusMessage     *message,
					void            *user_data)
{
	//DBGLOG(LOG_INFO, "ingress_filter_func, conn: %i  message: %i", connection, message);


	// we need to inform the Erlang Client of our 'unique-name'
	// associated with the connection.
	if (Iuniq_name_sent == FALSE) {
	  const char *uniq_name = dbus_bus_get_unique_name(connection);
	  DBGLOG(LOG_INFO, "unique-name: %s", uniq_name);

	  //paranoia
	  if (NULL==uniq_name) {
			exit(EDBUS_INVALID_UNIQUE_NAME);
	  }
	  if (ingress_send_unique_name(uniq_name)) {
		  exit(EDBUS_ERROR_SENDING_UNIQ);
	  }
	  Iuniq_name_sent=TRUE;
	}

	// main message handling starts here
	ingress_handle_message(message, user_data);

	if (dbus_message_is_signal (message,
							  DBUS_INTERFACE_LOCAL,
							  "Disconnected"))
	exit (EDBUS_DISCONNECTED);

	// We are passing the message along to the Erlang Client...
	return DBUS_HANDLER_RESULT_HANDLED;
}
