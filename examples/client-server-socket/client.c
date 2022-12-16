#include <opentelemetry_c.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

#include "context.h"
#include "zhelpers.h"

int main(void) {
    init_tracing("client-server-socket-example-client", "0.0.1", "",
                 "machine-client-0.0.1");
    void *tracer = get_tracer();

    printf("Connecting to hello world server...\n");
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 5; request_nbr++) {
        void *span = start_span(tracer, "get-hello", SPAN_KIND_CLIENT, "");

        char *context = extract_context_from_current_span();
        char *message_and_context = add_context_to_message("Hello", context);
        s_send(requester, message_and_context);
        free(context);
        free(message_and_context);
        // We decide to not add any context on response.
        // In this example, context is useful only when we start a span
        char *server_response = s_recv(requester);
        printf("Received from the server: %s\n", server_response);
        free(server_response);

        end_span(span);
    }
    zmq_close(requester);
    zmq_ctx_destroy(context);
    return 0;
}
