#include "opentelemetry_c.h"
#ifndef LTTNG_EXPORTER_ENABLED
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h>
#endif // LTTNG_EXPORTER_ENABLED

#ifdef LTTNG_EXPORTER_ENABLED
#include "utils/lttng_metrics_exporter.h"
#include "utils/lttng_spans_exporter.h"
#endif // LTTNG_EXPORTER_ENABLED
#include "utils/socket_carrier.h"

#include <opentelemetry/common/attribute_value.h>
#include <opentelemetry/context/context.h>
#include <opentelemetry/context/propagation/global_propagator.h>
#include <opentelemetry/context/propagation/text_map_propagator.h>
#include <opentelemetry/context/runtime_context.h>
#ifndef LTTNG_EXPORTER_ENABLED
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter.h>
#endif // LTTNG_EXPORTER_ENABLED
#include <opentelemetry/metrics/async_instruments.h>
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/nostd/shared_ptr.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h>
#include <opentelemetry/sdk/metrics/meter.h>
#include <opentelemetry/sdk/metrics/meter_provider.h>
#include <opentelemetry/sdk/metrics/metric_reader.h>
#include <opentelemetry/sdk/metrics/push_metric_exporter.h>
#include <opentelemetry/sdk/resource/semantic_conventions.h>
#include <opentelemetry/sdk/trace/simple_processor_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include <opentelemetry/trace/context.h>
#include <opentelemetry/trace/propagation/http_trace_context.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/trace/scope.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/span_metadata.h>
#include <opentelemetry/trace/tracer.h>

#include <cstddef>
#include <map>
#include <memory>
#include <string>

namespace context = opentelemetry::context;
namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace metrics_api = opentelemetry::metrics;
namespace nostd = opentelemetry::nostd;
namespace resource = opentelemetry::sdk::resource;
namespace trace = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;

using AttrMap = std::map<std::string, opentelemetry::common::AttributeValue>;

struct SpanAndContext {
  nostd::shared_ptr<trace::Span> span;
  nostd::shared_ptr<trace::Scope> scope;
  nostd::shared_ptr<context::Context> context;
};

void init_tracer_provider(const char *service_name, const char *service_version,
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
#ifdef LTTNG_EXPORTER_ENABLED
  auto exporter =
      std::unique_ptr<trace_sdk::SpanExporter>(new LttngSpanExporter);
#else
  auto exporter = std::unique_ptr<trace_sdk::SpanExporter>(
      new opentelemetry::exporter::otlp::OtlpGrpcExporter);
#endif // LTTNG_EXPORTER_ENABLED
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

void *create_attr_map() { return new AttrMap; }

void set_bool_attr(void *attr_map, const char *key, int boolean_value) {
  (*static_cast<AttrMap *>(attr_map))[key] = !!boolean_value;
}

void set_int32_t_attr(void *attr_map, const char *key, int32_t value) {
  (*static_cast<AttrMap *>(attr_map))[key] = value;
}

void set_int64_t_attr(void *attr_map, const char *key, int64_t value) {
  (*static_cast<AttrMap *>(attr_map))[key] = value;
}

void set_uint64_t_attr(void *attr_map, const char *key, uint64_t value) {
  (*static_cast<AttrMap *>(attr_map))[key] = value;
}

void set_double_attr(void *attr_map, const char *key, double value) {
  (*static_cast<AttrMap *>(attr_map))[key] = value;
}

void set_str_attr(void *attr_map, const char *key, const char *value) {
  (*static_cast<AttrMap *>(attr_map))[key] = value;
}

void destroy_attr_map(void *attr_map) {
  delete static_cast<AttrMap *>(attr_map);
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

void set_span_attrs(void *span, void *attr_map) {
  auto *span_and_context = static_cast<SpanAndContext *>(span);
  auto *attr_map_p = static_cast<AttrMap *>(attr_map);
  for (auto const &map_entry : *attr_map_p) {
    span_and_context->span->SetAttribute(map_entry.first, map_entry.second);
  }
}

void add_span_event(void *span, const char *event_name, void *attr_map) {
  auto *span_and_context = static_cast<SpanAndContext *>(span);
  auto *attr_map_p = static_cast<AttrMap *>(attr_map);
  span_and_context->span->AddEvent(event_name, *attr_map_p);
}

void end_span(void *span) {
  auto *span_and_context = static_cast<SpanAndContext *>(span);
  (*span_and_context).span->End();
  delete span_and_context;
}

void init_metrics_provider(const char *service_name,
                           const char *service_version,
                           const char *service_namespace,
                           const char *service_instance_id,
                           int64_t export_interval_millis,
                           int64_t export_timeout_millis) {
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
#ifdef LTTNG_EXPORTER_ENABLED
  std::unique_ptr<metrics_sdk::PushMetricExporter> exporter{
      new LttngMetricsExporter};
#else
  std::unique_ptr<metrics_sdk::PushMetricExporter> exporter{
      new opentelemetry::exporter::otlp::OtlpGrpcMetricExporter};
#endif // LTTNG_EXPORTER_ENABLED
  auto provider = std::shared_ptr<metrics_api::MeterProvider>(
      new metrics_sdk::MeterProvider(
          std::make_unique<metrics_sdk::ViewRegistry>(), resource));
  metrics_sdk::PeriodicExportingMetricReaderOptions options;
  options.export_interval_millis =
      std::chrono::milliseconds(export_interval_millis);
  options.export_timeout_millis =
      std::chrono::milliseconds(export_timeout_millis);
  std::unique_ptr<metrics_sdk::MetricReader> reader{
      new metrics_sdk::PeriodicExportingMetricReader(std::move(exporter),
                                                     options)};
  auto provider_p =
      std::static_pointer_cast<metrics_sdk::MeterProvider>(provider);
  provider_p->AddMetricReader(std::move(reader));
  metrics_api::Provider::SetMeterProvider(provider);
}

void *create_int64_up_down_counter(const char *name, const char *description) {
  auto provider = metrics_api::Provider::GetMeterProvider();
  auto p = dynamic_cast<metrics_sdk::MeterProvider *>(provider.get());
  // up down counter view
  std::string counter_name = std::string(name) + "_up_down_counter";
  std::unique_ptr<metrics_sdk::InstrumentSelector> instrument_selector{
      new metrics_sdk::InstrumentSelector(
          metrics_sdk::InstrumentType::kUpDownCounter, counter_name)};
  std::unique_ptr<metrics_sdk::MeterSelector> meter_selector{
      new metrics_sdk::MeterSelector(name, "1.2.0",
                                     "https://opentelemetry.io/schemas/1.2.0")};
  std::unique_ptr<metrics_sdk::View> sum_view{new metrics_sdk::View{
      name, description, metrics_sdk::AggregationType::kSum}};
  p->AddView(std::move(instrument_selector), std::move(meter_selector),
             std::move(sum_view));
  // up down counter
  auto meter = p->GetMeter(name, "1.2.0");
  return new nostd::unique_ptr<metrics_api::UpDownCounter<int64_t>>(
      meter->CreateInt64UpDownCounter(counter_name, description));
}

void int64_up_down_counter_add(void *counter, int64_t value) {
  static_cast<nostd::unique_ptr<metrics_api::UpDownCounter<int64_t>> *>(counter)
      ->get()
      ->Add(value, context::RuntimeContext::GetCurrent());
}

void destroy_up_down_counter(void *counter) {
  delete static_cast<nostd::unique_ptr<metrics_api::UpDownCounter<int64_t>> *>(
      counter);
}

void *create_int64_observable_up_down_counter(const char *name,
                                              const char *description) {
  auto provider = metrics_api::Provider::GetMeterProvider();
  auto p = dynamic_cast<metrics_sdk::MeterProvider *>(provider.get());
  // up down counter view
  std::string observable_counter_name =
      std::string(name) + "_observable_up_down_counter";
  std::unique_ptr<metrics_sdk::InstrumentSelector> instrument_selector{
      new metrics_sdk::InstrumentSelector(
          metrics_sdk::InstrumentType::kObservableUpDownCounter,
          observable_counter_name)};
  std::unique_ptr<metrics_sdk::MeterSelector> meter_selector{
      new metrics_sdk::MeterSelector(name, "1.2.0",
                                     "https://opentelemetry.io/schemas/1.2.0")};
  std::unique_ptr<metrics_sdk::View> sum_view{new metrics_sdk::View{
      name, description, metrics_sdk::AggregationType::kSum}};
  p->AddView(std::move(instrument_selector), std::move(meter_selector),
             std::move(sum_view));
  // up down counter
  auto meter = p->GetMeter(name, "1.2.0");
  return new nostd::shared_ptr<metrics_api::ObservableInstrument>(
      meter->CreateInt64ObservableUpDownCounter(observable_counter_name,
                                                description));
}

template <typename T>
static void counter_observable_fetcher(
    opentelemetry::metrics::ObserverResult observer_result, void *callback) {
  if (nostd::holds_alternative<
          nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<T>>>(
          observer_result)) {
    auto callback_f = (T(*)())callback;
    T value = (*callback_f)();
    nostd::get<nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<T>>>(
        observer_result)
        ->Observe(value);
  }
}

void int64_observable_up_down_counter_register_callback(void *counter,
                                                        int64_t (*callback)()) {
  static_cast<nostd::shared_ptr<metrics_api::ObservableInstrument> *>(counter)
      ->get()
      ->AddCallback(counter_observable_fetcher<int64_t>, (void *)callback);
}

void destroy_observable_up_down_counter(void *counter) {
  delete static_cast<nostd::shared_ptr<metrics_api::ObservableInstrument> *>(
      counter);
}
