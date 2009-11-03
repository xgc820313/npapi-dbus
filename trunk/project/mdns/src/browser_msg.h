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
			BMSG_JSON=1,
			BMSG_MALLOC_ERROR=2,
			BMSG_DBUS_CONN_ERROR=3,
			BMSG_DBUS_ADDFILTER_ERROR=4,
			BMSG_DBUS_ADDMATCH_ERROR=5,
			BMSG_DBUS_SERVICE_BROWSER_ERROR=6,
			BMSG_DBUS_DISCONNECTED=7,
			BMSG_ALLFORNOW=8,
			BMSG_CACHEEXHAUSTED=9,
			BMSG_EXITED=10,

			//From Client
			BMSG_RECONNECT=11,
			BMSG_EXIT=12,
			BMSG_EXIT_CLEAN=13,

			//Management
			BMSG_FLUSH_MARKER=14
		} BMsgType;

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
