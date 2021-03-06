#ifndef MESSAGES_H
#define MESSAGES_H 1
#include <inttypes.h>
#include "flow.h"

#define MESSAGE_BUFSIZE 8 //make sure the maxsize is correct
#define MESSAGE_MAXSIZE 64

enum messagetype{
        mt_hello,
        mt_addtrigger,
        mt_addtrigger_return,
        mt_deltrigger,
        mt_deltrigger_return,
        mt_triggersatisfaction,
        mt_triggerquery,
        mt_triggerquery_return,
        mt_bye
};

struct messageheader{
        enum messagetype type;
        uint16_t length;
};

struct message_hello{
        uint32_t id;
        uint32_t time;
};

struct message_addtrigger{
        struct flow f;
        struct flow mask;
        uint32_t eventid;
	uint32_t flowgranularity;
	uint16_t timeinterval;
        char buf[MESSAGE_BUFSIZE];
};

struct message_deltrigger{
        struct flow f;
        struct flow mask;
        uint32_t eventid;
};

struct message_deltrigger_return{
        uint32_t time;
        uint16_t eventid;
        bool success;
};

struct message_addtrigger_return{
        uint32_t time;
        uint16_t eventid;
        bool success;
};

struct message_triggersatisfaction{
	struct flow f;
        uint32_t time;
        uint16_t eventid;
        uint16_t code;
        char buf [MESSAGE_BUFSIZE];
};

struct message_triggerquery{
        struct flow f;
        struct flow mask;
        uint16_t eventid;
        uint16_t time;
};

#endif /* messages.h */
