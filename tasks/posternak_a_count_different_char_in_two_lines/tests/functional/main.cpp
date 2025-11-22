#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <string>
#include <utility>

#include "posternak_a_count_different_char_in_two_lines/common/include/common.hpp"
#include "posternak_a_count_different_char_in_two_lines/mpi/include/ops_mpi.hpp"
#include "posternak_a_count_different_char_in_two_lines/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace posternak_a_count_different_char_in_two_lines {

class PosternakACountDifferentCharInTwoLinesFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {

 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int test = std::get<0>(params);

    switch (test) {
      case 1:
        input_data_ = std::make_pair("string", "strong");
        expected_output_ = 1;
        /* code */
        break;
      case 2:
        input_data_ = std::make_pair("stringstring", "string");
        expected_output_ = 6;
        /* code */
        break;
      case 3:
        input_data_ = std::make_pair("string", "stringstring");
        expected_output_ = 6;
        /* code */
        break;
      case 4:
        input_data_ = std::make_pair("string", "string");
        expected_output_ = 0;
        /* code */
        break;
      case 5:
        input_data_ = std::make_pair("string", "STRING");
        expected_output_ = 6;
        /* code */
        break;
      default:
        break;
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return (expected_output_ == output_data);
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  OutType expected_output_;
};

namespace {

TEST_P(PosternakACountDifferentCharInTwoLinesFuncTests, CountDifferentCharInTwoLines) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 5> kTestParam = {std::make_tuple(1, "test1"), std::make_tuple(2, "test2"),
                                            std::make_tuple(3, "test3"), std::make_tuple(4, "test4"),
                                            std::make_tuple(5, "test5")};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<PosternakACountDifferentCharInTwoLinesMPI, InType>(
                                               kTestParam, PPC_SETTINGS_posternak_a_count_different_char_in_two_lines),
                                           ppc::util::AddFuncTask<PosternakACountDifferentCharInTwoLinesSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_posternak_a_count_different_char_in_two_lines));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    PosternakACountDifferentCharInTwoLinesFuncTests::PrintFuncTestName<PosternakACountDifferentCharInTwoLinesFuncTests>;

INSTANTIATE_TEST_SUITE_P(StringTests, PosternakACountDifferentCharInTwoLinesFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace posternak_a_count_different_char_in_two_lines
