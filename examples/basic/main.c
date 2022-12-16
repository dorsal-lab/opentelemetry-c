#include "opentelemetry_c.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
 * @brief Generate some spans
 *
 * The visualization should look like this :
 *
 * ––|–––––––|–––––––|–––––––|–––––––|–––––––|–––––––|–––––––|–––––––|–––––––|–>
 * time
 *
 *   [test-operation·······················································]
 *    [test-operation-loop] [test-operation-loop] ... [test-operation-loop]
 *
 */
int main() {
  printf("Basic example starts ...!\n");

  srand(0); // NOLINT

  char service_name[] = "test_service";
  char service_version[] = "0.0.1";
  char service_namespace[] = "com.test";
  char service_instance_id[] = "fake-instance-id-123456789";

  init_tracing(service_name, service_version, service_namespace,
               service_instance_id);
  void *tracer = get_tracer();

  span_kind_t kind = SPAN_KIND_INTERNAL;
  void *outer_span = start_span(tracer, "test-operation", kind, "");
  for (int i = 0; i < 3; i++) {
    // When creating nested span, there is no need to set the context
    void *span = start_span(tracer, "test-operation-loop", kind, "");
    // Sleep between 0 and 1 second
    usleep(1000000.0F / RAND_MAX * rand()); // NOLINT
    set_span_status(span, SPAN_STATUS_CODE_OK, "");
    end_span(span);
  }
  end_span(outer_span);

  printf("Basic example ends ...!\n");
  return 0;
}
