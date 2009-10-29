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

int
__browser_setup_dbus_conn(CommChannel *cc, DBusConnection **conn) {

	DBusError error;

	dbus_error_init(&error);

	*conn = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
	dbus_error_free (&error);
	if (NULL==*conn)
		goto fail;

	//DBGMSG("> Configuring filter function\n");
	// Configure the filter function
	if (!dbus_connection_add_filter (*conn, __browser_filter_func, vbp, NULL)) {
		browser_push_simple_msg( bp, BMsg::BMSG_DBUS_ADDFILTER_ERROR );
	}



	return TRUE;

fail:
	dbus_error_free (&error);

	return FALSE;
}//






BrowserReturnCode browser_init(OUT browserParams **bp) {

	//if this fails, we aint' going far anyway...
	*bp = (browserParams *) malloc(sizeof(browserParams));
	if (NULL==*bp) {
		return BROWSER_MALLOC_ERROR;
	}

	DBusError error;

	dbus_error_init (&error);

	//DBGMSG("> Attempting DBus connection\n");
	(*bp)->conn = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
	if (NULL==(*bp)->conn) {
		DBGMSG("Error: DBus connection\n");
	  dbus_error_free (&error);
	  free(bp);
	  return BROWSER_DBUS_CONN_ERROR;
	}

	//if this fails, we aint' going far anyway...
	(*bp)->q=queue_create();
	(*bp)->qin=queue_create();
	if ((NULL==(*bp)->q) || (NULL==(*bp)->qin)){
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

	//DBGMSG("> Configuring filter function\n");
	// Configure the filter function
	if (!dbus_connection_add_filter (conn, __browser_filter_func, vbp, NULL)) {
		browser_push_simple_msg( bp, BMsg::BMSG_DBUS_ADDFILTER_ERROR );
	}

	dbus_error_init (&error);

	//DBGMSG("> Configuring inteface match rule\n");
	dbus_bus_add_match(conn, "interface=org.freedesktop.Avahi.ServiceBrowser", &error);
	if (dbus_error_is_set(&error)) {
		browser_push_simple_msg( bp, BMsg::BMSG_DBUS_ADDMATCH_ERROR );
	}

	dbus_error_free(&error);

	//DBGMSG("> Registering Client to Avahi ServiceBrowser\n");
	if (!__browser_send_request_service_browser_new(conn)) {
		browser_push_simple_msg( bp, BMsg::BMSG_DBUS_SERVICE_BROWSER_ERROR );
	}

	DBGMSG("** Browser thread entering read-write loop\n");
	while (dbus_connection_read_write_dispatch(conn, 100));
	DBGMSG("!! Browser thread exiting\n");

	free(vbp);

	return NULL;
}//

int
__browser_send_request_service_browser_new(IN DBusConnection *conn) {

	DBusMessage *msg, *reply;
	char *path;

	//DBGMSG(" > Preparing DBus method call\n");
	msg=dbus_message_new_method_call("org.freedesktop.Avahi",
									 "/",
									 "org.freedesktop.Avahi.Server",
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
	 *
	 *  <arg name="path"      type="o"  direction="out"/>
	 * </method>
	 */
	int32_t interface = -1; //unspecified
	int32_t protocol  = -1; //unspecified
	uint32_t zero = 0;
	const char *sname  = "_http._tcp";
	const char *domain = "";

	// We must ask Avahi to communicate its results on the DBus
	// There must be at least 1 browser client on the DBus or
	// else Avahi won't generate signals on DBus
	int result = dbus_message_append_args(
			msg,
			DBUS_TYPE_INT32,   &interface,
			DBUS_TYPE_INT32,   &protocol,
			DBUS_TYPE_STRING,  &sname,
			DBUS_TYPE_STRING,  &domain,
			DBUS_TYPE_UINT32,  &zero,
			DBUS_TYPE_INVALID);

	if (!result) {
		DBGLOG(LOG_ERR, "browser: error building message");
		return FALSE;
	}

	dbus_message_set_destination(msg, "org.freedesktop.Avahi");

	//DBGMSG(" > Sending on DBus\n");
	result=dbus_connection_send(conn, msg, NULL);
	if (!result) {
		DBGLOG(LOG_ERR, "browser: error sending message");
	}

	//DBGMSG(" > Releasing message\n");
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

	int mtype = dbus_message_get_type(msg);
	DBGBEGIN
		if (DBUS_MESSAGE_TYPE_ERROR==mtype) {
			const char *ename = dbus_message_get_error_name(msg);
			printf("--> Error: %s\n", ename);
			return;
		}
	DBGEND;

	// we only care about signals
	if (DBUS_MESSAGE_TYPE_SIGNAL != mtype) {
		return;
	}

	// signal name e.g. ItemNew , ItemRemove
	const char *signalName = dbus_message_get_member(msg);
	if (NULL==signalName) {
		// paranoia... shouldn't happen
		return;
	}

	if (0==strcmp(signalName, "CacheExhausted")) {
		browser_push_simple_msg(bp, BMsg::BMSG_CACHEEXHAUSTED);
	}

	if (0==strcmp(signalName, "AllForNow")) {
		browser_push_simple_msg(bp, BMsg::BMSG_ALLFORNOW);
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
	dbus_int32_t interface, protocol;
	dbus_uint32_t flags;
	char *name=NULL, *type=NULL, *domain=NULL;

	DBusError error;
	dbus_error_init (&error);

    int result=dbus_message_get_args(
            msg, &error,
            DBUS_TYPE_INT32, &interface,
            DBUS_TYPE_INT32, &protocol,
            DBUS_TYPE_STRING, &name,
            DBUS_TYPE_STRING, &type,
            DBUS_TYPE_STRING, &domain,
            DBUS_TYPE_UINT32, &flags,
            DBUS_TYPE_INVALID);
    if (dbus_error_is_set(&error)) {
    	DBGLOG(LOG_ERR, "browser: error whilst decoding event: (%s)", error.message);
    	return;
    }

	// =============================================

	std::ostringstream *rjson;

	rjson = new std::ostringstream();

	*rjson << "{ 'signal':'" << signalName << "', 'interface':'" << interface << "',";
	*rjson << " 'protocol':'" << protocol << "', 'name':'" << name << "','type':'" << type << "',";
	*rjson << " 'domain':'" << domain << "', 'flags':'" << flags << "' }";

	BMsg *bmsg = new BMsg(BMsg::BMSG_JSON, rjson);

	browser_push_msg(bp, bmsg);

}//
