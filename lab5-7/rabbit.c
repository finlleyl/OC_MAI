#include "rabbit.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
static amqp_connection_state_t c;
int rabbit_connect(const char*host,int port) {
    c=amqp_new_connection();
    amqp_socket_t*s=amqp_tcp_socket_new(c);
    if(amqp_socket_open(s,host,port))return -1;
    if(amqp_login(c,"/",0,131072,0,AMQP_SASL_METHOD_PLAIN,"guest","guest").reply_type!=AMQP_RESPONSE_NORMAL)return -1;
    amqp_channel_open(c,1);
    if(amqp_get_rpc_reply(c).reply_type!=AMQP_RESPONSE_NORMAL)return -1;
    return 0;
}
int rabbit_create_queue(const char* q) {
    amqp_queue_declare_ok_t*r=amqp_queue_declare(c,1,amqp_cstring_bytes(q),0,0,0,0,amqp_empty_table);
    if(!r)return -1;
    return 0;
}
int rabbit_send(const char*q,const char*msg,size_t len) {
    amqp_basic_properties_t p;
    memset(&p,0,sizeof(p));
    p._flags=AMQP_BASIC_CONTENT_TYPE_FLAG|AMQP_BASIC_DELIVERY_MODE_FLAG;
    p.content_type=amqp_cstring_bytes("text/plain");
    p.delivery_mode=2;
    if(amqp_basic_publish(c,1,amqp_cstring_bytes(""),amqp_cstring_bytes(q),0,0,&p,amqp_cstring_bytes(msg)))return -1;
    return 0;
}
int rabbit_recv(const char*q,char*buf,size_t sz) {
    amqp_basic_consume(c,1,amqp_cstring_bytes(q),amqp_empty_bytes,0,1,0,amqp_empty_table);
    amqp_rpc_reply_t res=amqp_get_rpc_reply(c);
    if(res.reply_type!=AMQP_RESPONSE_NORMAL)return -1;
    for(;;) {
        amqp_envelope_t e;
        amqp_maybe_release_buffers(c);
        amqp_rpc_reply_t r=amqp_consume_message(c,&e,NULL,0);
        if(r.reply_type==AMQP_RESPONSE_NORMAL) {
            if(e.message.body.len<sz) {
                memcpy(buf,e.message.body.bytes,e.message.body.len);
                buf[e.message.body.len]='\0';
                amqp_destroy_envelope(&e);
                return 0;
            }
            amqp_destroy_envelope(&e);
            return -1;
        } else {
            return -1;
        }
    }
    return -1;
}
