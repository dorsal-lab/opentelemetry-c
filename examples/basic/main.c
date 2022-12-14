#include "opentelemetry-c.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
 * @brief Generate some spans
 *
 * The visualisation should look like this :
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

	char service_name [] = "test_service";
	char service_version [] = "test_service";
	char service_namespace [] = "test_service";
	char service_instance_id [] = "test_service";

	init_tracing(service_name, service_version, service_namespace, service_instance_id);
	void *tracer = get_tracer();

	span_kind_t kind = SPAN_KIND_INTERNAL;
	void *outer_span = start_span(tracer, "test-operation", kind, "");
	for (int i = 0; i < 3; i++) {
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
