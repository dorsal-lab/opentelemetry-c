#include <opentelemetry_c/opentelemetry_c.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  printf("Up Down Counter Basic example starts ...!\n");
  srand(0); // NOLINT
  init_metrics_provider("test_service", "0.0.1", "com.test",
                        "fake-instance-id-123456789", 1000, 500);
  void *counter = create_int64_up_down_counter(
      "n_active_requests", "Simple counter to keep track of the number of "
                           "active requests in the system");
  int64_t n_active_requests = 0;
  int64_up_down_counter_add(counter, 0);
  for (int i = 0; i < 60; i++) {
    // Randomly generate a number for active requests
    int64_t n_active_requests_new = rand() % 100; // NOLINT
    int64_t delta = n_active_requests_new - n_active_requests;
    printf("n_active_requests=%ld\n", n_active_requests_new);
    printf("delta=%ld\n", delta);
    int64_up_down_counter_add(counter, delta);
    n_active_requests = n_active_requests_new;
    // The work : Sleep between 0 and 1 second
    usleep(1000000.0F / RAND_MAX * rand()); // NOLINT
  }
  sleep(2); // Give time to process the last metric
  destroy_up_down_counter(counter);
  printf("Up Down Counter Basic example ends ...!\n");
  return 0;
}
