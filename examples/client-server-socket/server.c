#include "zhelpers.h"

#include <opentelemetry_c/opentelemetry_c.h>
#include <zmq.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  otelc_init_tracer_provider("client-server-socket-example-server", "0.0.1", "",
                             "machine-server-0.0.1");
  void *tracer = otelc_get_tracer();

  void *context = zmq_ctx_new();
  void *responder = zmq_socket(context, ZMQ_REP);
  int rc = zmq_bind(responder, "tcp://*:5555");
  assert(rc == 0);

  int request_nbr;
  for (request_nbr = 0; request_nbr != 5; request_nbr++) {
    char *remote_context = s_recv(responder);
    void *span = otelc_start_span(tracer, "get-hello-response",
                                  OTELC_SPAN_KIND_SERVER, remote_context);

    char *message = s_recv(responder);
    printf("[server] Received context from client: %s\n", remote_context);
    printf("[server] Received from client: %s\n", message);
    free(remote_context);
    free(message);
    sleep(1); //  Do some "work"
    // We decide to not add any context on response.
    // In this example, context is useful only when we start a span
    s_send(responder, "World!");

    otelc_end_span(span);
  }
  zmq_close(responder);
  zmq_ctx_destroy(context);
  otelc_destroy_tracer(tracer);
  return 0;
}
