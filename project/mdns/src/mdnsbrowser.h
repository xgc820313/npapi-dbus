/**
 * @file mdnsbrowser.h
 *
 *  Created on: 2009-11-02
 *      Author: jldupont
 */

#ifndef MDNSBROWSER_H_
#define MDNSBROWSER_H_

	#include "browser.h"
	#include "browser_msg.h"


	class MDNSBrowser {

		CommChannel *cc;
		int initState;

	public:
		MDNSBrowser();
		~MDNSBrowser();

	public:
		/**
		 * @return TRUE  SUCCESS
		 * @return FALSE FAILURE
		 */
		bool init(void);

		/**
		 * Required to be used in place of 'delete'
		 */
		void release(void);

		/**
		 * Pushes a Message to the worker thread
		 */
		void pushMsg(BMsg::BMsgType type);

		void pushMsg(BMsg *msg);

		/**
		 * Retrieves a Message from the worker thread
		 *
		 * @return NULL if no message
		 */
		BMsg *popMsg(void);

	};



#endif /* MDNSBROWSER_H_ */
