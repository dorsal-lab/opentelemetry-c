#include <opentelemetry_c/opentelemetry_c.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief Function called periodically by the counter to get the difference
 * between the current observation and the last sent
 */
int64_t counter_callback() {
  static int64_t last_n_active_requests = 0;
  int64_t current_n_active_requests = rand() % 100;
  int64_t delta = current_n_active_requests - last_n_active_requests;
  last_n_active_requests = current_n_active_requests;
  printf("n_active_requests=%ld\n", current_n_active_requests);
  printf("delta=%ld\n", delta);
  return delta;
}

int main() {
  printf("Observable Up Down Counter Basic example starts ...!\n");
  srand(0); // NOLINT
  otelc_init_metrics_provider("test_service", "0.0.1", "com.test",
                              "fake-instance-id-123456789", 1000, 500);
  void *counter = otelc_create_int64_observable_up_down_counter(
      "n_active_requests", "Simple counter to keep track of the number of "
                           "active requests in the system");
  void *registration = otelc_int64_observable_up_down_counter_register_callback(
      counter, &counter_callback);
  sleep(60); // Give time to counter to call callback few times
  otelc_int64_observable_up_down_counter_cancel_registration(counter,
                                                             registration);
  otelc_destroy_observable_up_down_counter(counter);
  printf("Observable Up Down Counter Basic example ends ...!\n");
  return 0;
}
