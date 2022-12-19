#include "opentelemetry_c.h"

#include "utils/lttng_span_exporter.h"
#include "utils/map.h"
#include "utils/socket_carrier.h"

#include <opentelemetry/context/context.h>
#include <opentelemetry/context/propagation/global_propagator.h>
#include <opentelemetry/context/propagation/text_map_propagator.h>
#include <opentelemetry/context/runtime_context.h>
#include <opentelemetry/nostd/shared_ptr.h>
#include <opentelemetry/sdk/resource/semantic_conventions.h>
#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/sdk/trace/simple_processor_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include <opentelemetry/trace/context.h>
#include <opentelemetry/trace/propagation/http_trace_context.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/trace/scope.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/span_metadata.h>
#include <opentelemetry/trace/tracer.h>

#include <cstddef>
#include <memory>
#include <string>

namespace context = opentelemetry::context;
namespace trace = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace nostd = opentelemetry::nostd;
namespace resource = opentelemetry::sdk::resource;

struct SpanAndContext {
  nostd::shared_ptr<trace::Span> span;
  nostd::shared_ptr<trace::Scope> scope;
  nostd::shared_ptr<context::Context> context;
};

void init_tracing(const char *service_name, const char *service_version,
                  const char *service_namespace,
                  const char *service_instance_id) {
  resource::ResourceAttributes attributes = {
      {resource::SemanticConventions::kServiceName, std::string(service_name)},
      {resource::SemanticConventions::kServiceVersion,
       std::string(service_version)},
      {resource::SemanticConventions::kServiceNamespace,
       std::string(service_namespace)},
      {resource::SemanticConventions::kServiceInstanceId,
       std::string(service_instance_id)},
  };
  auto resource = resource::Resource::Create(attributes);
  auto exporter =
      std::unique_ptr<trace_sdk::SpanExporter>(new LttngSpanExporter);
  auto processor =
      trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));

  std::shared_ptr<trace::TracerProvider> provider =
      trace_sdk::TracerProviderFactory::Create(std::move(processor), resource);
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
  // set global propagator
  context::propagation::GlobalTextMapPropagator::SetGlobalPropagator(
      nostd::shared_ptr<context::propagation::TextMapPropagator>(
          new trace::propagation::HttpTraceContext()));
}

void *get_tracer() {
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
  return new nostd::shared_ptr<trace::Tracer>(provider->GetTracer(
      std::string("opentelemetry-c"), std::string(OPENTELEMETRY_VERSION)));
}

void destroy_tracer(void *tracer) {
  delete static_cast<nostd::shared_ptr<trace::Tracer> *>(tracer);
}

void *start_span(void *tracer, const char *span_name, span_kind_t span_kind,
                 const char *remote_context) {
  // Span options
  trace::StartSpanOptions options;

  trace::SpanKind otel_span_kind;
  switch (span_kind) {
  case SPAN_KIND_INTERNAL:
    otel_span_kind = trace::SpanKind::kInternal;
    break;
  case SPAN_KIND_SERVER:
    otel_span_kind = trace::SpanKind::kServer;
    break;
  case SPAN_KIND_CLIENT:
    otel_span_kind = trace::SpanKind::kClient;
    break;
  case SPAN_KIND_PRODUCER:
    otel_span_kind = trace::SpanKind::kProducer;
    break;
  case SPAN_KIND_CONSUMER:
    otel_span_kind = trace::SpanKind::kConsumer;
    break;
  default:
    otel_span_kind = trace::SpanKind::kInternal;
  }
  options.kind = otel_span_kind;

  auto ctx = context::RuntimeContext::GetCurrent();
  // Context extraction
  if (remote_context != NULL && strcmp(remote_context, "") != 0) { // NOLINT
    auto prop =
        context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    const SocketTextMapCarrier carrier((std::string(remote_context)));
    auto carrier_ctx = prop->Extract(carrier, ctx);
    options.parent = trace::GetSpan(carrier_ctx)->GetContext();
  }

  // Create the span
  auto *tracer_p = static_cast<nostd::shared_ptr<trace::Tracer> *>(tracer);
  auto span = (*tracer_p)->StartSpan(span_name, options);
  auto scope =
      std::make_shared<trace::Scope>((*tracer_p)->WithActiveSpan(span));
  auto new_ctx =
      std::make_shared<context::Context>(context::RuntimeContext::GetCurrent());
  return new SpanAndContext{span, scope, new_ctx};
}

char *extract_context_from_current_span(void *span) {
  auto *span_and_context = static_cast<SpanAndContext *>(span);
  auto prop =
      context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
  SocketTextMapCarrier carrier;
  prop->Inject(carrier, *(span_and_context->context));
  std::string ctx_s = carrier.Serialize();
  const auto size = ctx_s.size();
  char *buffer = new char[size + 1]; // NOLINT
  memcpy(buffer, ctx_s.c_str(), size + 1);
  return buffer;
}

void set_span_status(void *span, span_status_code_t code,
                     const char *description) {
  auto *span_and_context = static_cast<SpanAndContext *>(span);
  trace::StatusCode otel_code;
  switch (code) {
  case SPAN_STATUS_CODE_UNSET:
    otel_code = trace::StatusCode::kUnset;
    break;
  case SPAN_STATUS_CODE_OK:
    otel_code = trace::StatusCode::kOk;
    break;
  case SPAN_STATUS_CODE_ERROR:
    otel_code = trace::StatusCode::kError;
    break;
  default:
    otel_code = trace::StatusCode::kUnset;
  }
  span_and_context->span->SetStatus(otel_code, std::string(description));
}

void end_span(void *span) {
  auto *span_and_context = static_cast<SpanAndContext *>(span);
  (*span_and_context).span->End();
  delete span_and_context;
}
