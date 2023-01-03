#include "zhelpers.h"

#include <opentelemetry_c.h>
#include <zmq.h>

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  init_tracer_provider("client-server-socket-example-client", "0.0.1", "",
               "machine-client-0.0.1");
  void *tracer = get_tracer();

  printf("Connecting to hello world server...\n");
  void *context = zmq_ctx_new();
  void *requester = zmq_socket(context, ZMQ_REQ);
  zmq_connect(requester, "tcp://localhost:5555");

  int request_nbr;
  for (request_nbr = 0; request_nbr != 5; request_nbr++) {
    void *span = start_span(tracer, "get-hello", SPAN_KIND_CLIENT, "");

    char *remote_context = extract_context_from_current_span(span);
    s_sendmore(requester, remote_context);
    s_send(requester, "Hello");
    free(remote_context);
    // We decide to not add any context on response.
    // In this example, context is useful only when we start a span
    char *server_response = s_recv(requester);
    printf("[client] Received from the server: %s\n", server_response);
    free(server_response);

    end_span(span);
  }

  zmq_close(requester);
  zmq_ctx_destroy(context);
  destroy_tracer(tracer);
  return 0;
}
