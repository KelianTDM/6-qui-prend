#ifndef MESSAGE_H
#define MESSAGE_H

typedef enum{
    MSG_INT = 1,
    MSG_CARDS = 2,
    MSG_LINES =3,
    MSG_WAIT =4,
    MSG_SCORE = 5,
    MSG_END = 6,
    MSG_MATE = 7
} MsgType;

typedef struct{
    MsgType type;
    int size;
} MsgHeader;

#endif