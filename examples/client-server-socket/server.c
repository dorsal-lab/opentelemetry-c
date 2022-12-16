#include "context.h"
#include "zhelpers.h"

#include <opentelemetry_c.h>
#include <zmq.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    init_tracing("client-server-socket-example-server", "0.0.1", "",
                 "machine-server-0.0.1");
    void *tracer = get_tracer();

    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    int rc = zmq_bind(responder, "tcp://*:5555");
    assert(rc == 0);

    int request_nbr;
    for (request_nbr = 0; request_nbr != 5; request_nbr++) {
        char *client_message_and_context = s_recv(responder);
        char *message, *context;
        split_message_and_context(client_message_and_context, &message,
                                  &context);
        void *span =
            start_span(tracer, "get-hello-response", SPAN_KIND_SERVER, context);

        printf("Received from client: %s\n", message);
        free(client_message_and_context);
        sleep(1); //  Do some "work"
        // We decide to not add any context on response.
        // In this example, context is useful only when we start a span
        s_send(responder, "World!");

        end_span(span);
    }
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}
