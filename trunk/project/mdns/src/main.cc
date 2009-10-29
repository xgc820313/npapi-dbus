/**
 * @file main.cc
 *
 *  Created on: 2009-10-28
 *      Author: jldupont
 */
#include "browser.h"
#include "browser_msg.h"


int main(int argc, char *argv[]) {

	browserParams *bp;
	queue *q;
	BMsg *msg;

	int result;

	printf("> starting\n");

	result=browser_init( &bp );
	if (BROWSER_OK != result) {
		printf("Error: result code: %i\n", result);
		exit(1);
	}

	//shortcut
	q=bp->q;

	printf("> entering loop\n");
	do {
		result=queue_wait(q);
		if (result) {
			printf("Error: waiting on queue\n");
			break;
		}
		msg = (BMsg*) queue_get_nb(q);
		if (NULL==msg)
			continue;

		printf("Msg: type: %i\n", msg->type);

		delete msg;

	} while(1);

	return 0;
}//
