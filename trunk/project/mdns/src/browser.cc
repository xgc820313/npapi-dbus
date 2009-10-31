/**
 * @file browser.cc
 *
 * @date 2009-10-28
 * @author jldupont
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

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
	int __browser_setup_dbus_conn(browserParams **bp);
//}}

	// FSM related
	// ***********

typedef enum {
		E_NULL=0,
		E_ANY=1,
		E_DBUS_DISCONNECT,
		E_CONNECT_OK,
		E_CONNECT_ERROR,
		E_CMD_RECONNECT,
		E_CMD_EXIT
	} FsmEvent;

typedef enum {
		ST_START=0, //default is also the entry
		ST_CONNECT,
		ST_SERVE,
		ST_WAIT,
		ST_EXIT,
		//======
		ST_TRAP,
	} FsmState;

typedef struct {
		FsmState state;
		FsmEvent event;
		browserParams *bp;
	} FsmContext;


typedef FsmEvent StateFn(FsmContext *c);


//prototypes
StateFn ActionConnect, ActionServe, ActionWait, ActionExit;


typedef struct {
	FsmState current_state;
	FsmEvent event;
	FsmState next_state;
	FsmEvent  (*action)(FsmContext *);
} TransitionElement;


TransitionElement TransitionTable[] = {

		{ST_START,   E_ANY,             ST_CONNECT, &ActionConnect},

		{ST_CONNECT, E_NULL,            ST_CONNECT, &ActionConnect},
		{ST_CONNECT, E_CONNECT_OK,      ST_SERVE,   &ActionServe},
		{ST_CONNECT, E_CONNECT_ERROR,   ST_WAIT,    &ActionWait},
		{ST_CONNECT, E_ANY,             ST_WAIT,    &ActionWait},

		{ST_SERVE,   E_DBUS_DISCONNECT, ST_WAIT,    &ActionWait},
		{ST_SERVE,   E_CMD_EXIT,        ST_EXIT,    &ActionExit},
		{ST_SERVE,   E_ANY,             ST_SERVE,   &ActionServe},

		{ST_WAIT,    E_CMD_RECONNECT,   ST_CONNECT, &ActionConnect},
		{ST_WAIT,    E_CMD_EXIT,        ST_EXIT,    &ActionExit},
		{ST_WAIT,    E_ANY,             ST_WAIT,    &ActionWait},

		{ST_TRAP,    E_ANY,             ST_CONNECT, &ActionConnect}
};



// *************************************************************************
// *************************************************************************




/**
 * Initialization of a Browser thread
 *
 * It is the responsability of the Client user of this
 * function to clean the CommChannel.
 *
 * @param cc: [IN/OUT] pointer to receive the configured fields CommChannel
 */
BrowserReturnCode
browser_init(CommChannel *cc) {

	assert(cc);

	browserParams *bp = (browserParams *) malloc(sizeof(browserParams));
	if (NULL==bp)
		goto exit;

	//if this fails, we aint' going far anyway...
	cc->in  = queue_create();
	cc->out = queue_create();

	if ((NULL==cc->in) || (NULL==cc->out))
		goto clean;

	bp->cc = cc;

	pthread_create(&(bp->thread), NULL, &__browser_thread_function, (void *) bp);

ok:
	return BROWSER_OK;
//  ==================

clean:

	if (cc->in)
		free(cc->in);
	if (cc->out)
		free(cc->out);

exit:

	cc->in =NULL;
	cc->out=NULL;

	return BROWSER_MALLOC_ERROR;
}//




//  PRIVATE FROM HEREON <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// ====================





FsmEvent
event_pump(browserParams *bp) {

	assert(bp);

	DBusConnection *conn = bp->conn;

	// connected on DBus yet?
	if (NULL!=conn) {
		if (dbus_connection_read_write_dispatch(conn, 100 /*timeout*/))
			return E_DBUS_DISCONNECT;
	}

	int mtype=BMsg::BMSG_INVALID;
	BMsg *msg= (BMsg *) queue_get_nb(bp->cc->in);
	if (NULL!=msg) {
		mtype=msg->type;
	}

	FsmEvent ret = E_NULL;
	switch(mtype) {
	case BMsg::BMSG_RECONNECT:
		ret=E_CMD_RECONNECT;
		break;
	case BMsg::BMSG_EXIT:
		ret=E_CMD_EXIT;
		break;
	default:
		ret=E_NULL;
		break;
	}

	if (NULL!=msg)
		delete msg;

	return ret;
}//


void
run_fsm(browserParams *bp) {

	FsmState currentState = ST_START, nextState;
	FsmEvent currentEvent = E_NULL, re=E_NULL;
	FsmContext fc;

	fc.bp = bp;

	int tcount=sizeof(TransitionTable)/sizeof(TransitionElement);

	DBGMSG("=> entering FSM loop\n");
	do {
		// we've got an event from the last
		// "action": process it before we take on
		// some other events
		if (E_NULL!=re)
			currentEvent = re;

		if (E_NULL==re)
			currentEvent = event_pump(bp);

		for (int i=0; i<tcount;i++) {
			FsmState ts = TransitionTable[i].current_state;
			FsmState ns = TransitionTable[i].next_state;
			FsmEvent te = TransitionTable[i].event;
			FsmEvent (*action)(FsmContext *) = TransitionTable[i].action;

			// state matches?
			if ((currentState == ts) || (ST_TRAP == ts)) {
				// event matches?
				if ((currentEvent==te) || (E_ANY==te)) {

					// ** PATTERN MATCH **
					fc.state = currentState;
					fc.event = currentEvent;

					DBGMSG("=> dispatching to action, state=%i, event=%i\n", ts, te);
					re = (*action)(&fc);
					currentState = ns; //new "current" state
					break;
				}
			}// ===============================================

		}//for

	} while(currentState != ST_EXIT);
	DBGMSG("=> exiting FSM loop\n");

}//


FsmEvent
ActionConnect(FsmContext *c) {

	FsmEvent event=E_NULL;

	int ret=__browser_setup_dbus_conn( &(c->bp) );

	switch(ret) {
	case BROWSER_DBUS_CONN_ERROR:
		DBGLOG(LOG_ERR, "browser: dbus connection error\n");
		event=E_CONNECT_ERROR;
		break;

	case BROWSER_DBUS_FILTER_ERROR:
		DBGLOG(LOG_ERR, "browser: dbus connection error: can't install filter function\n");
		event=E_CONNECT_ERROR;
		break;

	case BROWSER_DBUS_MATCH_ERROR:
		DBGLOG(LOG_ERR, "browser: dbus connection error: can't install match filter function\n");
		event=E_CONNECT_ERROR;
		break;

	case BROWSER_DBUS_SERVICE_ERROR:
		DBGLOG(LOG_ERR, "browser: dbus connection error: can't connect to Avahi\n");
		event=E_CONNECT_ERROR;
		break;

	case BROWSER_OK:
		DBGMSG("Browser: connect ok!\n");
		event=E_CONNECT_OK;
		break;

	default:
		event=E_NULL;
		break;
	}

	return event;
}//


FsmEvent
ActionServe(FsmContext *c) {

	DBGMSG("--- ActionServe\n");

	// not much to do... the event_pump
	// takes care of dispatching work to be done.

	return E_NULL;
}//


FsmEvent
ActionWait(FsmContext *c) {

	DBGMSG("--- ActionWait\n");

	// not much to do... the event_pump
	// takes care of the waiting ;-)

	return E_NULL;
}//


FsmEvent
ActionExit(FsmContext *c) {

	DBGLOG(LOG_INFO, "browser: exiting");

	//push exit message back to the Client
	browser_push_simple_msg(c->bp, BMsg::BMSG_EXITED);

	return E_CMD_EXIT;
}//





// ============================================================================================
// ============================================================================================
// ============================================================================================






/**
 * Sets up access to DBus
 *
 * Returns the DBusConnection in *bp
 */
int
__browser_setup_dbus_conn(browserParams **bp) {

	int ret=BROWSER_OK;
	DBusConnection *conn;
	DBusError error;

	dbus_error_init(&error);

	conn = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
	if (NULL==conn) {
		ret=BROWSER_DBUS_CONN_ERROR;
		goto fail;
	}

	//DBGMSG("> Configuring filter function\n");
	// Configure the filter function
	if (!dbus_connection_add_filter (conn, __browser_filter_func, bp, NULL)) {
		browser_push_simple_msg( *bp, BMsg::BMSG_DBUS_ADDFILTER_ERROR );
		ret=BROWSER_DBUS_FILTER_ERROR;
		goto fail;
	}

	dbus_error_init (&error);

	//DBGMSG("> Configuring inteface match rule\n");
	dbus_bus_add_match(conn, "interface=org.freedesktop.Avahi.ServiceBrowser", &error);
	if (dbus_error_is_set(&error)) {
		browser_push_simple_msg( *bp, BMsg::BMSG_DBUS_ADDMATCH_ERROR );
		ret=BROWSER_DBUS_MATCH_ERROR;
		goto fail;
	}

	//DBGMSG("> Registering Client to Avahi ServiceBrowser\n");
	if (!__browser_send_request_service_browser_new(conn)) {
		browser_push_simple_msg( *bp, BMsg::BMSG_DBUS_SERVICE_BROWSER_ERROR );
		ret = BROWSER_DBUS_SERVICE_ERROR;
		goto fail;
	}

	(*bp)->conn = conn;
	return BROWSER_OK;

fail:

	if (conn) {
		dbus_connection_close(conn);
	}
	(*bp)->conn=NULL;

	dbus_error_free (&error);
	return ret;
}//


/**
 * Thread function
 *
 * @param _bp: browserParams
 */
void *
__browser_thread_function(void *bp) {

	DBGMSG("** Browser thread entering read-write loop\n");
		run_fsm( (browserParams *) bp);
	DBGMSG("!! Browser thread exiting\n");

	//the field 'cc' of the browserParams struct
	//needs to be taken care of by the original caller.
	free((browserParams *)bp);

	return NULL;
}//

int
__browser_send_request_service_browser_new(DBusConnection *conn) {

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
						IN void          *_bp)
{
	//DBGLOG(LOG_INFO, "ingress_filter_func, conn: %i  message: %i", connection, message);

	if (dbus_message_is_signal (message, DBUS_INTERFACE_LOCAL, "Disconnected")) {

		browser_push_simple_msg( (browserParams *) _bp, BMsg::BMSG_DBUS_DISCONNECTED );
	} else {

		// main message handling starts here
		__browser_handle_message(message, (browserParams *)_bp);
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
