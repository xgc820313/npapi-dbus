/**
 * @file main.cc
 *
 *  Created on: 2009-10-28
 *      Author: jldupont
 */
#include "browser.h"
#include "browser_msg.h"


int main(int argc, char *argv[]) {

	CommChannel *cc=(CommChannel *) malloc(sizeof(CommChannel));
	queue *q;
	BMsg *msg;

	printf("> starting\n");

	int result=browser_init( cc );
	if (BROWSER_OK != result) {
		printf("Error: result code: %i\n", result);
		exit(1);
	}

	//shortcut
	q=cc->out;

	printf("> entering loop, q=%i\n", q);
	do {
		result=queue_wait_timer(q, 1000*100);
		if (result)
			continue;

		msg = (BMsg*) queue_get_nb(q);
		if (NULL==msg)
			continue;

		printf("Msg: type: %i\n", msg->type);
		if (msg->type==BMsg::BMSG_JSON) {
			std::stringbuf *sbuf;
			std:string st;
			sbuf=msg->json_string->rdbuf();
			st=sbuf->str();
			//cout << st;
			printf("json string: %s\n", st.data());
		}

		delete msg;

	} while(1);

	return 0;
}//
