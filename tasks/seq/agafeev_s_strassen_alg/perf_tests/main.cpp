#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <memory>
#include <random>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "core/task/include/task.hpp"
#include "seq/agafeev_s_strassen_alg/include/strassen_seq.hpp"

namespace {
std::vector<double> MatrixMultiply(const std::vector<double>& a, const std::vector<double>& b, int row_col_size) {
  std::vector<double> c(row_col_size * row_col_size, 0);

  for (int i = 0; i < row_col_size; ++i) {
    for (int j = 0; j < row_col_size; ++j) {
      for (int k = 0; k < row_col_size; ++k) {
        c[(i * row_col_size) + j] += a[(i * row_col_size) + k] * b[(k * row_col_size) + j];
      }
    }
  }

  return c;
}

std::vector<double> CreateRandomMatrix(int row_size, int column_size) {
  auto rand_gen = std::mt19937(time(nullptr));
  std::uniform_real_distribution<double> dist(-100.0, 100.0);
  std::vector<double> matrix(row_size * column_size);
  for (unsigned int i = 0; i < matrix.size(); i++) {
    matrix[i] = dist(rand_gen);
  }

  return matrix;
}
}  // namespace

TEST(agafeev_s_strassen_alg_seq, test_pipeline_run) {
  const int n = 128;
  const int m = 128;

  // Credate Data
  std::vector<double> in_matrix1 = CreateRandomMatrix(n, m);
  std::vector<double> in_matrix2 = CreateRandomMatrix(n, m);
  std::vector<double> out(n * m, 0.0);

  // Create task_data
  std::shared_ptr<ppc::core::TaskData> task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in_matrix1.data()));
  task_data->inputs_count.emplace_back(n);
  task_data->inputs_count.emplace_back(m);
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in_matrix2.data()));
  task_data->inputs_count.emplace_back(n);
  task_data->inputs_count.emplace_back(m);
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  auto test_task_sequental = std::make_shared<agafeev_s_strassen_alg_seq::MultiplMatrixSequental>(task_data);

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perf_analyzer = std::make_shared<ppc::core::Perf>(test_task_sequental);
  perf_analyzer->PipelineRun(perf_attr, perf_results);
  ppc::core::Perf::PrintPerfStatistic(perf_results);
  auto temp = MatrixMultiply(in_matrix1, in_matrix2, n);

  for (size_t i = 0; i < out.size(); i++) {
    ASSERT_FLOAT_EQ(temp[i], out[i]);
  }
}

TEST(agafeev_s_strassen_alg_seq, test_task_run) {
  const int n = 128;
  const int m = 128;

  // Credate Data
  std::vector<double> in_matrix1 = CreateRandomMatrix(n, m);
  std::vector<double> in_matrix2 = CreateRandomMatrix(n, m);
  std::vector<double> out(n * m, 0.0);

  // Create task_data
  std::shared_ptr<ppc::core::TaskData> task_data = std::make_shared<ppc::core::TaskData>();
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in_matrix1.data()));
  task_data->inputs_count.emplace_back(n);
  task_data->inputs_count.emplace_back(m);
  task_data->inputs.emplace_back(reinterpret_cast<uint8_t*>(in_matrix2.data()));
  task_data->inputs_count.emplace_back(n);
  task_data->inputs_count.emplace_back(m);
  task_data->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
  task_data->outputs_count.emplace_back(out.size());

  auto test_task_sequental = std::make_shared<agafeev_s_strassen_alg_seq::MultiplMatrixSequental>(task_data);

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perf_analyzer = std::make_shared<ppc::core::Perf>(test_task_sequental);
  perf_analyzer->TaskRun(perf_attr, perf_results);
  ppc::core::Perf::PrintPerfStatistic(perf_results);
  auto temp = MatrixMultiply(in_matrix1, in_matrix2, n);

  for (size_t i = 0; i < out.size(); i++) {
    ASSERT_FLOAT_EQ(temp[i], out[i]);
  }
}