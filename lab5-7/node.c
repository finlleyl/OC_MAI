#include "common.h"

int main(int argc, char** argv) {
    int id = atoi(argv[1]);
    int parent_id = -1;
    if(argc > 2) parent_id = atoi(argv[2]);
    void* ctx = zmq_ctx_new();
    void* rep = zmq_socket(ctx, ZMQ_REP);
    char endpoint[256];
    sprintf(endpoint, "ipc:///tmp/node_%d.ipc", id);
    zmq_bind(rep, endpoint);
    void* req = NULL;
    if(parent_id != -1) {
        req = zmq_socket(ctx, ZMQ_REQ);
        char p_endpoint[256];
        sprintf(p_endpoint, "ipc:///tmp/node_%d.ipc", parent_id);
        zmq_connect(req, p_endpoint);
    }
    while(1) {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        int rc = zmq_msg_recv(&msg, rep, 0);
        if(rc == -1) { zmq_msg_close(&msg); break; }
        char* data = (char*)zmq_msg_data(&msg);
        int len = zmq_msg_size(&msg);
        char* cmd = strndup(data, len);
        zmq_msg_close(&msg);
        char* token = strtok(cmd, " ");
        if(!token) {
            free(cmd);
            char err[]="Error";
            zmq_send(rep, err, strlen(err),0);
            continue;
        }
        if(strcmp(token,"exec")==0) {
            token = strtok(NULL," ");
            int count = atoi(token);
            int sum=0;
            for(int i=0;i<count;i++) {
                token = strtok(NULL," ");
                if(!token) {sum=0;break;}
                sum+=atoi(token);
            }
            char resp[256];
            sprintf(resp,"%d",sum);
            zmq_send(rep, resp, strlen(resp),0);
        } else if(strcmp(token,"ping")==0) {
            char resp[]="1";
            zmq_send(rep,resp,strlen(resp),0);
        } else {
            char err[]="Error";
            zmq_send(rep, err, strlen(err),0);
        }
        free(cmd);
    }
    zmq_close(rep);
    if(req) zmq_close(req);
    zmq_ctx_term(ctx);
    return 0;
}
