#ifndef LTTNG_LOGS_EXPORTER_H
#define LTTNG_LOGS_EXPORTER_H

#include <opentelemetry/common/spin_lock_mutex.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter.h>

namespace logs_sdk = opentelemetry::sdk::logs;

/**
 * The OTLP exporter exports log data in OpenTelemetry Protocol (OTLP) format in
 * gRPC.
 */
class LttngLogsExporter : public logs_sdk::LogRecordExporter {
public:
  /**
   * Create an OtlpGrpcLogRecordExporter with default exporter options.
   */
  LttngLogsExporter();

  /**
   * Create an OtlpGrpcLogRecordExporter with user specified options.
   * @param options An object containing the user's configuration options.
   */
  LttngLogsExporter(
      const opentelemetry::exporter::otlp::OtlpGrpcExporterOptions &options);

  /**
   * Creates a recordable that stores the data in protobuf.
   * @return a newly initialized Recordable object.
   */
  std::unique_ptr<logs_sdk::Recordable> MakeRecordable() noexcept override;

  /**
   * Exports a vector of log records to the configured gRPC endpoint. Guaranteed
   * to return after a timeout specified from the options passed to the
   * constructor.
   * @param records A list of log records.
   */
  opentelemetry::sdk::common::ExportResult
  Export(const opentelemetry::nostd::span<std::unique_ptr<logs_sdk::Recordable>>
             &records) noexcept override;

  /**
   * Shutdown this exporter.
   * @param timeout The maximum time to wait for the shutdown method to return.
   */
  bool Shutdown(std::chrono::microseconds timeout =
                    std::chrono::microseconds::max()) noexcept override;

private:
  // Configuration options for the exporter
  const opentelemetry::exporter::otlp::OtlpGrpcExporterOptions options_;

  bool is_shutdown_ = false;
  mutable opentelemetry::common::SpinLockMutex lock_;
  bool isShutdown() const noexcept;
};

#endif // !LTTNG_LOGS_EXPORTER_H
