// Note : Please keep this import the first to avoid running into conflict
// between Abseil library and OpenTelemetry C++ absl::variant implementation
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter.h>

#include "utils/lttng_metrics_exporter.h"
#include "utils/lttng_opentelemetry_exporter_tracepoints.h"

#include <opentelemetry/exporters/otlp/otlp_metric_utils.h>
#include <opentelemetry/sdk_config.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h>

#include <iostream>

namespace metrics_sdk = opentelemetry::sdk::metrics;

LttngMetricsExporter::LttngMetricsExporter()
    : LttngMetricsExporter(
          opentelemetry::exporter::otlp::OtlpGrpcMetricExporterOptions()) {}

LttngMetricsExporter::LttngMetricsExporter(
    const opentelemetry::exporter::otlp::OtlpGrpcMetricExporterOptions &options)
    : options_(options), aggregation_temporality_selector_{
                             opentelemetry::exporter::otlp::OtlpMetricUtils::
                                 ChooseTemporalitySelector(
                                     options_.aggregation_temporality)} {}

opentelemetry::sdk::common::ExportResult LttngMetricsExporter::Export(
    const metrics_sdk::ResourceMetrics &data) noexcept {
  if (isShutdown()) {
    OTEL_INTERNAL_LOG_ERROR("[OTLP METRICS gRPC] Exporting "
                            << data.scope_metric_data_.size()
                            << " metric(s) failed, exporter is shutdown")
    return opentelemetry::sdk::common::ExportResult::kFailure;
  }
  if (data.scope_metric_data_.empty()) {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }
  opentelemetry::proto::collector::metrics::v1::ExportMetricsServiceRequest
      request;
  opentelemetry::exporter::otlp::OtlpMetricUtils::PopulateRequest(data,
                                                                  &request);
  for (int i = 0; i < request.resource_metrics_size(); i++) {
    const auto &resource_metrics = request.resource_metrics(i);
    auto resource_metrics_str = resource_metrics.SerializeAsString();
    lttng_ust_tracepoint(opentelemetry, resource_metrics,
                         (uint8_t *)resource_metrics_str.c_str(),
                         resource_metrics_str.size());
#ifndef NDEBUG
    std::cout << "Debug resource metrics :" << std::endl
              << resource_metrics.DebugString() << std::endl;
#endif // !NDEBUG
  }

  return opentelemetry::sdk::common::ExportResult::kSuccess;
}

metrics_sdk::AggregationTemporality
LttngMetricsExporter::GetAggregationTemporality(
    metrics_sdk::InstrumentType instrument_type) const noexcept {
  return aggregation_temporality_selector_(instrument_type);
}

bool LttngMetricsExporter::ForceFlush(
    std::chrono::microseconds /*timeout =
        (std::chrono::microseconds::max)()*/) noexcept {
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return true;
}

bool LttngMetricsExporter::Shutdown(
    std::chrono::microseconds /*timeout =
        (std::chrono::microseconds::max)()*/) noexcept {
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_shutdown_ = true;
  return true;
}

bool LttngMetricsExporter::isShutdown() const noexcept {
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return is_shutdown_;
}
