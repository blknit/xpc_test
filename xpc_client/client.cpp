//
//  main.cpp
//  xpc_client
//
//  Created by tongqing.liu on 6/13/20.
//  Copyright © 2020 tongqing.liu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <xpc/xpc.h>

int
main(int argc, char *argv[])
{
    xpc_connection_t conn;

    xpc_object_t msg_with_reply = xpc_dictionary_create(NULL, NULL, 0);
    xpc_dictionary_set_bool(msg_with_reply, "reply", true);
    xpc_dictionary_set_string(msg_with_reply, "msg", "msg_with_reply");

    xpc_object_t msg_without_replay = xpc_dictionary_create(NULL, NULL, 0);
    xpc_dictionary_set_bool(msg_without_replay, "reply", false);
    xpc_dictionary_set_string(msg_without_replay, "msg", "msg_without_reply");

    conn = xpc_connection_create_mach_service("com.test.xpc", NULL, 0);
    if (conn == NULL) {
        perror("xpc_connection_create_mach_service");
        return (1);
    }
    
    xpc_connection_set_event_handler(conn, ^(xpc_object_t obj) {
        printf("Received message in generic event handler: %p\n", obj);
        printf("%s\n", xpc_copy_description(obj));
    });

    xpc_connection_resume(conn);
    xpc_connection_send_message(conn, msg_without_replay);

    xpc_connection_send_message_with_reply(conn, msg_with_reply, NULL, ^(xpc_object_t resp) {
        printf("Received second message: %p\n", resp);
        printf("%s\n", xpc_copy_description(resp));
    });

    xpc_connection_send_message_with_reply(conn, msg_with_reply, NULL, ^(xpc_object_t resp) {
        printf("Received third message: %p\n", resp);
        printf("%s\n", xpc_copy_description(resp));
    });

    dispatch_main();
}
