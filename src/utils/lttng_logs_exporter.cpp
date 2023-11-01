// Note : Please keep this import the first to avoid running into conflict
// between Abseil library and OpenTelemetry C++ absl::variant implementation
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter.h>

#include "opentelemetry_c/utils/lttng_logs_exporter.h"
#include "opentelemetry_c/utils/lttng_opentelemetry_exporter_tracepoints.h"

#include <opentelemetry/exporters/otlp/otlp_log_recordable.h>
#include <opentelemetry/exporters/otlp/otlp_recordable_utils.h>
#include <opentelemetry/sdk_config.h>

#include <iostream>

namespace logs_sdk = opentelemetry::sdk::logs;

LttngLogsExporter::LttngLogsExporter()
    : LttngLogsExporter(
          opentelemetry::exporter::otlp::OtlpGrpcExporterOptions()) {}

LttngLogsExporter::LttngLogsExporter(
    const opentelemetry::exporter::otlp::OtlpGrpcExporterOptions &options)
    : options_(options) {}

// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<logs_sdk::Recordable>
LttngLogsExporter::MakeRecordable() noexcept {
  return std::unique_ptr<logs_sdk::Recordable>(
      new opentelemetry::exporter::otlp::OtlpLogRecordable());
}

opentelemetry::sdk::common::ExportResult LttngLogsExporter::Export(
    const opentelemetry::nostd::span<std::unique_ptr<logs_sdk::Recordable>>
        &logs) noexcept {
  if (isShutdown()) {
    OTEL_INTERNAL_LOG_ERROR("[OTLP gRPC log] Exporting "
                            << logs.size()
                            << " log(s) failed, exporter is shutdown");
    return opentelemetry::sdk::common::ExportResult::kFailure;
  }
  if (logs.empty()) {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  opentelemetry::proto::collector::logs::v1::ExportLogsServiceRequest request;
  opentelemetry::exporter::otlp::OtlpRecordableUtils::PopulateRequest(logs,
                                                                      &request);

  for (int i = 0; i < request.resource_logs_size(); i++) {
    const auto &resource_logs = request.resource_logs(i);
    auto resource_logs_str = resource_logs.SerializeAsString();
    lttng_ust_tracepoint(opentelemetry, resource_logs,
                         (uint8_t *)resource_logs_str.c_str(),
                         resource_logs_str.size());
#ifndef NDEBUG
    std::cout << "Debug resource logs :" << std::endl
              << resource_logs.DebugString() << std::endl;
#endif // !NDEBUG
  }
  return opentelemetry::sdk::common::ExportResult::kSuccess;
}

bool LttngLogsExporter::Shutdown(
    std::chrono::microseconds /* timeout */) noexcept {
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_shutdown_ = true;
  return true;
}

bool LttngLogsExporter::isShutdown() const noexcept {
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return is_shutdown_;
}
