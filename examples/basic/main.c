#include <opentelemetry_c/opentelemetry_c.h>

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

  init_tracer_provider("test_service", "0.0.1", "com.test",
                       "fake-instance-id-123456789");
  void *tracer = get_tracer();

  void *outer_span =
      start_span(tracer, "test-operation", SPAN_KIND_INTERNAL, "");
  for (int i = 0; i < 3; i++) {
    // Create a nested span
    // When creating nested span in the same thread, there is no need to set the
    // context
    void *span =
        start_span(tracer, "test-operation-loop", SPAN_KIND_INTERNAL, "");
    // Set an attribute for the loop iteration number
    void *map = create_attr_map();
    set_int32_t_attr(map, "loop-no", i);
    set_span_attrs(span, map);
    destroy_attr_map(map);
    // We log an event that will mark the beginning of the work
    map = create_attr_map();
    set_str_attr(map, "message", "Work started");
    add_span_event(span, "work_start", map);
    destroy_attr_map(map);
    // The work : Sleep between 0 and 1 second
    usleep(1000000.0F / RAND_MAX * rand()); // NOLINT
    // We log an event that will mark the end of the work
    map = create_attr_map();
    set_str_attr(map, "message", "Work ended");
    add_span_event(span, "work_end", map);
    destroy_attr_map(map);
    // Set the span status (optional)
    // Start the span
    set_span_status(span, SPAN_STATUS_CODE_OK, "");
    end_span(span);
  }
  end_span(outer_span);

  destroy_tracer(tracer);
  printf("Basic example ends ...!\n");
  return 0;
}
