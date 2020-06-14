//
//  main.cpp
//  xpc_test
//
//  Created by tongqing.liu on 6/13/20.
//  Copyright © 2020 tongqing.liu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <dispatch/dispatch.h>
#include <xpc/xpc.h>

void reply_msg_to_client(xpc_connection_t peer, xpc_object_t msg)
{
    xpc_object_t resp = xpc_dictionary_create_reply(msg);
    const char * name = xpc_dictionary_get_string(msg, "msg");
    xpc_dictionary_set_string(resp, "msg", name);
    xpc_connection_send_message(peer, resp);
}

void send_msg_to_client(xpc_connection_t peer, xpc_object_t msg)
{
    xpc_object_t resp = xpc_dictionary_create(NULL, NULL, 0);
    const char * name = xpc_dictionary_get_string(msg, "msg");
    xpc_dictionary_set_string(resp, "msg", name);
    xpc_connection_send_message(peer, resp);
}

int main(int argc, char *argv[])
{
    xpc_connection_t conn;
    conn = xpc_connection_create_mach_service("com.test.xpc", NULL,
        XPC_CONNECTION_MACH_SERVICE_LISTENER);

    xpc_connection_set_event_handler(conn, ^(xpc_object_t peer) {
        printf("New connection, peer=%p\n", peer);
        xpc_connection_set_event_handler((xpc_connection_t)peer, ^(xpc_object_t event) {
            if (event == XPC_ERROR_CONNECTION_INVALID) {
                printf("Connection closed by remote end\n");
                return;
            }

            if (xpc_get_type(event) != XPC_TYPE_DICTIONARY) {
                printf("Received something else than a dictionary!\n");
                return;
            }

            printf("Message received: %p\n", event);
            printf("%s\n", xpc_copy_description(event));

            bool reply = xpc_dictionary_get_bool(event, "reply");
            send_msg_to_client((xpc_connection_t)peer, event);
            
            if(reply)
                reply_msg_to_client((xpc_connection_t)peer, event);
        });

        xpc_connection_resume((xpc_connection_t)peer);
    });

    xpc_connection_resume(conn);
    dispatch_main();
}
