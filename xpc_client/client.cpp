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

int main(int argc, char *argv[])
{
    // setup connection
    xpc_connection_t conn;

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
    
    // case 1 send without reply
    xpc_object_t msg_without_replay = xpc_dictionary_create(NULL, NULL, 0);
    xpc_dictionary_set_bool(msg_without_replay, "reply", false);
    xpc_dictionary_set_string(msg_without_replay, "msg", "msg_without_reply");
    
    xpc_connection_send_message(conn, msg_without_replay);
    
    xpc_release(msg_without_replay);

    // case 2 send with reply sync
    xpc_object_t msg_with_reply1 = xpc_dictionary_create(NULL, NULL, 0);
    xpc_dictionary_set_bool(msg_with_reply1, "reply", true);
    xpc_dictionary_set_string(msg_with_reply1, "msg", "msg_with_reply");
    
    xpc_object_t reply = xpc_connection_send_message_with_reply_sync(conn, msg_with_reply1);
    printf("Received second message: %p\n", reply);
    printf("%s\n", xpc_copy_description(reply));
    
    xpc_release(msg_with_reply1);

    // case 3 send with reply async with timeout in 1 sec
    xpc_object_t msg_with_reply2 = xpc_dictionary_create(NULL, NULL, 0);
    xpc_dictionary_set_bool(msg_with_reply2, "reply", true);
    xpc_dictionary_set_string(msg_with_reply2, "msg", "msg_with_reply");
    
    dispatch_group_t group = dispatch_group_create();
    dispatch_group_enter(group);
    xpc_connection_send_message_with_reply(conn, msg_with_reply2, NULL, ^(xpc_object_t resp) {
        printf("Received third message: %p\n", resp);
        printf("%s\n", xpc_copy_description(resp));
        dispatch_group_leave(group);
    });
    
    dispatch_time_t timeout = dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC);
    dispatch_group_wait(group, timeout);
    
    xpc_release(msg_with_reply2);
    printf("end\n");

    // clean up
    xpc_release(conn);
    
    // loop for ever
    // dispatch_main();
}
