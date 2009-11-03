/**
 * @file browser_msg.h
 *
 *  Created on: 2009-10-28
 *      Author: jldupont
 */

#ifndef BROWSER_MSG_H_
#define BROWSER_MSG_H_

	#include <iostream>
	#include <sstream>
	#include <string>

	#include "macros.h"
	#include "queue.h"
	#include "browser.h"

	using namespace std;

	/**
	 * Browser Message Class
	 */
	class BMsg {

	public:

		//values are included for easier debugging
		typedef enum {
			//From Browser to Client
			BMSG_INVALID=0,
			BMSG_JSON,
			BMSG_MALLOC_ERROR,
			BMSG_DBUS_CONN_ERROR,
			BMSG_DBUS_ADDFILTER_ERROR,
			BMSG_DBUS_ADDMATCH_ERROR,
			BMSG_DBUS_SERVICE_BROWSER_ERROR,
			BMSG_DBUS_DISCONNECTED,
			BMSG_ALLFORNOW,
			BMSG_CACHEEXHAUSTED,
			BMSG_EXITED,
			BMSG_INIT_ERROR,

			//From Client
			BMSG_RECONNECT,
			BMSG_EXIT,
			BMSG_EXIT_CLEAN,

			//Management
			BMSG_FLUSH_MARKER,

			BMSG_LAST_MESSAGE
		} BMsgType;

		static const char *strTypes[BMSG_LAST_MESSAGE+1];

	public:
		BMsgType type;
		ostringstream *json_string;

	public:
		BMsg() {
			type=BMSG_INVALID;
			json_string=NULL;
		}

		BMsg(BMsgType _type) {
			type=_type;
			json_string=NULL;
		}
		BMsg(BMsgType _type, ostringstream *_resp) {
			type=_type;
			json_string=_resp;
		}
		~BMsg() {
			if (NULL!=json_string)
				delete json_string;
		}
		const char *translateType(void) {
			translateType(type);
		}

		const char *translateType(BMsgType type) {
			if (0>type) return NULL;
			if ((sizeof(strTypes)/sizeof(const char *))<type) return NULL;

			return strTypes[type];
		}

		bool isExit(void) {
			return type==BMSG_EXIT;
		}

		bool isReconnect(void) {
			return type==BMSG_RECONNECT;
		}

		 std::string getJsonString(void) {
			std::stringbuf *sbuf;
			std:string st;
			sbuf=json_string->rdbuf();
			st=sbuf->str();
			return st;
		}

	};



	// Prototypes
	// ^^^^^^^^^^

	void browser_push_simple_msg(queue *q, BMsg::BMsgType type);
	void browser_push_simple_msg(browserParams *bp, BMsg::BMsgType type);
	void browser_push_msg(browserParams *bp, BMsg *msg);
	void browser_push_msg(queue *q, BMsg *msg);


#endif /* BROWSER_MSG_H_ */
