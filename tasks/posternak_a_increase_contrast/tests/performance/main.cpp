#include <gtest/gtest.h>

#include <string>
#include <utility>

#include "posternak_a_increase_contrast/common/include/common.hpp"
#include "posternak_a_increase_contrast/mpi/include/ops_mpi.hpp"
#include "posternak_a_increase_contrast/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace posternak_a_increase_contrast {

class PosternakAIncreaseContrastPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 10000000;
  std::string first_string_;
  std::string second_string_;
  InType input_data_;
  OutType expected_output_{};

  void SetUp() override {
    for (int i = 0; i < kCount_; i++) {
      first_string_ += "string";
      second_string_ += "strong";
    }
    input_data_ = std::make_pair(first_string_, second_string_);
    expected_output_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return expected_output_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(PosternakAIncreaseContrastPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, PosternakAIncreaseContrastMPI, PosternakAIncreaseContrastSEQ>(
        PPC_SETTINGS_posternak_a_increase_contrast);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = PosternakAIncreaseContrastPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, PosternakAIncreaseContrastPerfTests, kGtestValues, kPerfTestName);

}  // namespace posternak_a_increase_contrast
