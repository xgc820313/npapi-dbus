/**
 * @file browser.cc
 *
 * @date 2009-10-28
 * @author jldupont
 */
#include <stdio.h>
#include <string.h>

#include "browser.h"
#include "browser_msg.h"

#include<iostream>
#include <sstream>

using namespace std;


//{{ PROTOTYPES

	void *            __browser_thread_function(void *params);
	DBusHandlerResult __browser_filter_func (DBusConnection *connection, DBusMessage *message, void *user_data);
	void __browser_handle_message(DBusMessage *msg, browserParams *bp);
	int __browser_send_request_service_browser_new(DBusConnection *conn);
	void __browser__process_signal(DBusMessage *msg, browserParams *bp, const char *signalName);
//}}



BrowserReturnCode browser_init(OUT browserParams **bp) {

	//if this fails, we aint' going far anyway...
	*bp = (browserParams *) malloc(sizeof(browserParams));
	if (NULL==*bp) {
		return BROWSER_MALLOC_ERROR;
	}

	DBusError error;
	DBusBusType type = DBUS_BUS_SYSTEM;

	dbus_error_init (&error);

	DBGMSG("> Attempting DBus connection\n");
	(*bp)->conn = dbus_bus_get (type, &error);
	if (NULL==(*bp)->conn) {
		DBGMSG("Error: DBus connection\n");
	  dbus_error_free (&error);
	  free(bp);
	  return BROWSER_DBUS_CONN_ERROR;
	}

	DBGMSG("> Creating communication queue\n");
	//if this fails, we aint' going far anyway...
	(*bp)->q=queue_create();
	if (NULL==*bp) {
		free(bp);
		return BROWSER_MALLOC_ERROR;
	}

	dbus_error_free (&error);

	pthread_create(&((*bp)->thread), NULL, &__browser_thread_function, (void *) *bp);

	return BROWSER_OK;
}//

void *
__browser_thread_function(IN void *vbp) {

	DBusError error;

	//make shortcuts
	browserParams *bp=(browserParams *) vbp;
	DBusConnection *conn=bp->conn;

	// Configure the filter function
	if (!dbus_connection_add_filter (conn, __browser_filter_func, vbp, NULL)) {
		//@TODO better way to handle this...
		browser_push_simple_msg( bp, BMsg::BMSG_DBUS_ADDFILTER_ERROR );
	}

	dbus_error_init (&error);

	dbus_bus_add_match(conn, "interface=org.freedesktop.Avahi.ServiceBrowser", &error);
	if (dbus_error_is_set(&error)) {
		//@TODO better way to handle this...
		browser_push_simple_msg( bp, BMsg::BMSG_DBUS_ADDMATCH_ERROR );
	}

	dbus_error_free(&error);

	if (!__browser_send_request_service_browser_new(conn)) {
		//@TODO better way to handle this...
		browser_push_simple_msg( bp, BMsg::BMSG_DBUS_SERVICE_BROWSER_ERROR );
	}

	while (dbus_connection_read_write_dispatch(conn, 100));

	free(vbp);

	return NULL;
}//

int
__browser_send_request_service_browser_new(IN DBusConnection *conn) {

	DBusMessage *msg;

	msg=dbus_message_new_method_call("org.freedesktop.Avahi",
									"/",
									"org.freedesktop.Avahi.ServiceBrowser",
									"ServiceBrowserNew");
	if (NULL==msg)
		return FALSE;

	/*
	 * <method name="ServiceBrowserNew">
	 * 	<arg name="interface" type="i"  direction="in"/>
	 *  <arg name="protocol"  type="i"  direction="in"/>
	 *  <arg name="type"      type="s"  direction="in"/>
	 *  <arg name="domain"    type="s"  direction="in"/>
	 *  <arg name="flags"     type="u"  direction="in"/>
	 *  <arg name="path"      type="o"  direction="out"/>
	 * </method>
	 */
	dbus_int32_t minusOne = -1;
	dbus_int32_t zero = 0;

	// We must ask Avahi to communicate its results on the DBus
	// There must be at least 1 browser client on the DBus
	int result = dbus_message_append_args(
			msg,
			DBUS_TYPE_INT32,   &minusOne,
			DBUS_TYPE_INT32,   &minusOne,
			DBUS_TYPE_STRING,  "_http._tcp",
			DBUS_TYPE_STRING,  "local",
			DBUS_TYPE_UINT32,  &zero,
			DBUS_TYPE_STRING,  "org.freedesktop.Avahi.ServiceBrowser",
			DBUS_TYPE_INVALID);

	if (!result) {
		DBGLOG(LOG_ERR, "browser: error building message");
		return FALSE;
	}

	result=dbus_connection_send(conn, msg, NULL);

	if (!result) {
		DBGLOG(LOG_ERR, "browser: error sending message");
	}

	dbus_message_unref(msg);

	return result;
}//



// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------




DBusHandlerResult
__browser_filter_func (IN DBusConnection *connection,
						IN DBusMessage   *message,
						IN void          *vbp)
{
	//DBGLOG(LOG_INFO, "ingress_filter_func, conn: %i  message: %i", connection, message);

	if (dbus_message_is_signal (message, DBUS_INTERFACE_LOCAL, "Disconnected")) {

		browser_push_simple_msg( (browserParams *) vbp, BMsg::BMSG_DBUS_DISCONNECTED );
	} else {

		// main message handling starts here
		__browser_handle_message(message, (browserParams *)vbp);
	}

	return DBUS_HANDLER_RESULT_HANDLED;
}//

void
__browser_handle_message(IN DBusMessage *msg, IN browserParams *bp) {

	// we only care about signals
	if (DBUS_MESSAGE_TYPE_SIGNAL != dbus_message_get_type(msg)) {
		return;
	}

	// signal name e.g. ItemNew , ItemRemove
	const char *signalName = dbus_message_get_member(msg);
	if (NULL==signalName) {
		// paranoia... shouldn't happen
		return;
	}

	if ((0==strcmp(signalName, "ItemNew")) || (0==strcmp(signalName, "ItemRemove"))) {
		__browser__process_signal(msg, bp, signalName);
	}
}//

/*
    <signal name="ItemNew">
      <arg name="interface" type="i"/>
      <arg name="protocol"  type="i"/>
      <arg name="name"      type="s"/>
      <arg name="type"      type="s"/>
      <arg name="domain"    type="s"/>
      <arg name="flags"     type="u"/>
    </signal>

    <signal name="ItemRemove">
      <arg name="interface" type="i"/>
      <arg name="protocol"  type="i"/>
      <arg name="name"      type="s"/>
      <arg name="type"      type="s"/>
      <arg name="domain"    type="s"/>
      <arg name="flags"     type="u"/>
    </signal>
 */
void
__browser__process_signal(DBusMessage *msg, browserParams *bp, const char *signalName) {

	int t;
	DBusMessageIter *iter=(DBusMessageIter *)malloc(sizeof(DBusMessageIter));
	dbus_int32_t interface, protocol;
	dbus_uint32_t flags;
	char *name, *type, *domain;


	// Iterate over the DBus message
	dbus_message_iter_init (msg, iter);

	//interface
	t = dbus_message_iter_get_arg_type (iter);
	if (DBUS_TYPE_INT32!=t) {
		DBGLOG(LOG_ERR, "browser: expecting (INT32) for 'interface' parameter");
		free(iter);
		return;
	}
	dbus_message_iter_get_basic (iter, &interface);

	//protocol
	t = dbus_message_iter_get_arg_type (iter);
	if (DBUS_TYPE_INT32!=t) {
		DBGLOG(LOG_ERR, "browser: expecting (INT32) for 'protocol' parameter");
		free(iter);
		return;
	}
	dbus_message_iter_get_basic (iter, &protocol);

	//name
	t = dbus_message_iter_get_arg_type (iter);
	if (DBUS_TYPE_STRING!=t) {
		DBGLOG(LOG_ERR, "browser: expecting (STRING) for 'name' parameter");
		free(iter);
		return;
	}
	dbus_message_iter_get_basic (iter, &name);

	//type
	t = dbus_message_iter_get_arg_type (iter);
	if (DBUS_TYPE_STRING!=t) {
		DBGLOG(LOG_ERR, "browser: expecting (STRING) for 'type' parameter");
		free(iter);
		return;
	}
	dbus_message_iter_get_basic (iter, &type);

	//domain
	t = dbus_message_iter_get_arg_type (iter);
	if (DBUS_TYPE_STRING!=t) {
		DBGLOG(LOG_ERR, "browser: expecting (STRING) for 'domain' parameter");
		free(iter);
		return;
	}
	dbus_message_iter_get_basic (iter, &domain);

	//flags
	t = dbus_message_iter_get_arg_type (iter);
	if (DBUS_TYPE_UINT32!=t) {
		DBGLOG(LOG_ERR, "browser: expecting (UINT32) for 'flags' parameter");
		free(iter);
		return;
	}
	dbus_message_iter_get_basic (iter, &flags);
	free(iter);

	// =============================================

	std::ostringstream *rjson;

	rjson = new std::ostringstream();

	*rjson << "{ 'signal':'" << signalName << "', 'interface':'" << interface << "',";
	*rjson << " 'protocol':'" << protocol << "', 'name':'" << name << "','type':'" << type << "',";
	*rjson << " 'domain':'" << domain << "', 'flags':'" << flags << "' }";

	BMsg *bmsg = new BMsg(BMsg::BMSG_JSON, rjson);

	browser_push_msg(bp, bmsg);

}//
