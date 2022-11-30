#ifndef OPENTELEMETRY_C_H
#define OPENTELEMETRY_C_H

#ifdef __cplusplus
extern "C" {
#endif

void init_tracing();
void *get_tracer(char *service_name, char *service_version);
void destroy_tracer(void *tracer);

void *start_span(void *tracer, char *span_name, char *remote_context);
char *extract_context_from_current_span();
// TODO(augustinsangam): Add support for span->SetStatus
void end_span(void *span);

#ifdef __cplusplus
}
#endif

#endif // !OPENTELEMETRY_C_H
