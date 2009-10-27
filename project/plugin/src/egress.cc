/**
 * @file   egress.cc
 *
 * @date   2009-10-26
 * @author jldupont
 */

#include <pthread.h>
#include "dbus/dbus.h"

void *__egress_thread_function(void *conn);


/**
 * Egress thread initialization
 */
void egress_init(DBusConnection *conn, pthread_t *thread) {

	pthread_create(thread, NULL, &__egress_thread_function, (void *)conn);

}//


void *
__egress_thread_function(void *conn) {

}
