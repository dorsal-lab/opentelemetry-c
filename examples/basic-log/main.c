#include <opentelemetry_c/opentelemetry_c.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief Generate some logs
 */
int main() {
  printf("Basic Log example starts ...!\n");

  srand(0); // NOLINT

  otelc_init_logger_provider("test_service", "0.0.1", "com.test",
                             "fake-instance-id-123456789");
  void *logger = otelc_get_logger();

  for (int i = 0; i < 3; i++) {
    otelc_log(logger, OTEL_C_LOG_SEVERITY_KINFO, "Hello");
    // Sleep between 0 and 1 second
    usleep(1000000.0F / RAND_MAX * rand()); // NOLINT
  }

  otelc_destroy_logger(logger);
  printf("Basic Log example ends ...!\n");
  return 0;
}
