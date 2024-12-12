#ifndef RABBIT_H
#define RABBIT_H
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <amqp_framing.h>
int rabbit_connect(const char*,int);
int rabbit_create_queue(const char*);
int rabbit_send(const char*,const char*,size_t);
int rabbit_recv(const char*,char*,size_t);
#endif
