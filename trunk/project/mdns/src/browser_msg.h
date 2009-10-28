/**
 * @file browser_msg.h
 *
 *  Created on: 2009-10-28
 *      Author: jldupont
 */

#ifndef BROWSER_MSG_H_
#define BROWSER_MSG_H_

	#include "macros.h"
	#include "queue.h"

	/**
	 * Browser Message Class
	 */
	class BMsg {

		typedef enum {
		 BMSG_INVALID=0,

		} BMsgType;

	public:
		BMsgType type;

	public:
		BMsg() {
			type=BMSG_INVALID;
		}

	};

#endif /* BROWSER_MSG_H_ */
