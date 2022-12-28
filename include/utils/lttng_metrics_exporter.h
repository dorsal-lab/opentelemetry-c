#ifndef LTTNG_METRICS_EXPORTER_H
#define LTTNG_METRICS_EXPORTER_H

#include <opentelemetry/common/spin_lock_mutex.h>
#include <opentelemetry/sdk/metrics/push_metric_exporter.h>

namespace metrics_sdk = opentelemetry::sdk::metrics;

class LttngMetricsExporter final : public metrics_sdk::PushMetricExporter {
public:
  /**
   * Create an LttngMetricsExporter
   */
  explicit LttngMetricsExporter() noexcept = default;

  /**
   * Create an LttngMetricsExporter
   */
  explicit LttngMetricsExporter(
      metrics_sdk::AggregationTemporality aggregation_temporality) noexcept;

  /**
   * Export
   * @param data metrics data
   */
  opentelemetry::sdk::common::ExportResult
  Export(const metrics_sdk::ResourceMetrics &data) noexcept override;

  /**
   * Get the AggregationTemporality for LttngMetricsExporter exporter
   *
   * @return AggregationTemporality
   */
  metrics_sdk::AggregationTemporality GetAggregationTemporality(
      metrics_sdk::InstrumentType instrument_type) const noexcept override;

  /**
   * Force flush the exporter.
   */
  bool ForceFlush(std::chrono::microseconds timeout =
                      (std::chrono::microseconds::max)()) noexcept override;

  /**
   * Shut down the exporter.
   * @param timeout an optional timeout.
   * @return return the status of this operation
   */
  bool Shutdown(std::chrono::microseconds timeout =
                    (std::chrono::microseconds::max)()) noexcept override;

private:
  metrics_sdk::AggregationTemporality aggregation_temporality_;
  bool is_shutdown_ = false;
  mutable opentelemetry::common::SpinLockMutex lock_;

  bool isShutdown() const noexcept;
};

#endif // !LTTNG_METRICS_EXPORTER_H
