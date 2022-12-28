// Note : Please keep this import the first to avoid running into conflict
// between Abseil library and OpenTelemetry C++ absl::variant implementation
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter.h>

#include "utils/lttng_opentelemetry_exporter_tracepoints.h"
#include "utils/lttng_spans_exporter.h"

#include <opentelemetry/exporters/otlp/otlp_recordable.h>
#include <opentelemetry/exporters/otlp/otlp_recordable_utils.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <string>

namespace otlp_exporter = opentelemetry::exporter::otlp;
namespace sdk_trace = opentelemetry::sdk::trace;

std::unique_ptr<sdk_trace::Recordable>
LttngSpanExporter::MakeRecordable() noexcept {
  return std::unique_ptr<sdk_trace::Recordable>(
      new otlp_exporter::OtlpRecordable);
}

opentelemetry::sdk::common::ExportResult LttngSpanExporter::Export(
    const opentelemetry::nostd::span<std::unique_ptr<sdk_trace::Recordable>>
        &spans) noexcept {
  if (spans.empty()) {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest request;
  otlp_exporter::OtlpRecordableUtils::PopulateRequest(spans, &request);

  for (int i = 0; i < request.resource_spans_size(); i++) {
    auto resource_spans = request.resource_spans(i);
    std::string resource_spans_str = resource_spans.SerializeAsString();
    lttng_ust_tracepoint(opentelemetry, resource_spans,
                         (uint8_t *)resource_spans_str.c_str(),
                         resource_spans_str.length());
#ifndef NDEBUG
    std::cout << "Debug resource spans :" << std::endl
              << resource_spans.DebugString() << std::endl;
#endif // !NDEBUG
  }
  return opentelemetry::sdk::common::ExportResult::kSuccess;
}

bool LttngSpanExporter::Shutdown(
    std::chrono::microseconds /*timeout*/) noexcept {
  return true;
}
