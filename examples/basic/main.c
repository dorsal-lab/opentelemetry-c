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
 * ––|–––––––|–––––––|–––––––|–––––––|–––––––|–––––––|–––––––|–> time
 *
 *   [test-operation-group··································]
 *    [test-operation] [test-operation] ... [test-operation]
 */
int main() {
	printf("Basic example starts ...!\n");

	srand(0); // NOLINT

	init_tracing();
	void *tracer = get_tracer("test-service", "0.0.1");

	void *outer_span = start_span(tracer, "test-operation-group", NULL);
	for (int i = 0; i < 10; i++) {
		void *span = start_span(tracer, "test-operation", NULL);
		// Sleep between 0 and 1 second
		usleep(1000000.0F / RAND_MAX * rand()); // NOLINT
		end_span(span);
	}
	end_span(outer_span);

	printf("Basic example ends ...!\n");
	return 0;
}
