#include "posternak_a_increase_contrast/seq/include/ops_seq.hpp"

#include <cstddef>
#include <string>
#include <utility>

#include "posternak_a_increase_contrast/common/include/common.hpp"

namespace posternak_a_increase_contrast {

PosternakAIncreaseContrastSEQ::PosternakAIncreaseContrastSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool PosternakAIncreaseContrastSEQ::ValidationImpl() {
  std::pair<std::string, std::string> &lines = GetInput();
  std::string s1 = lines.first;
  std::string s2 = lines.second;
  return !s1.empty() && !s2.empty();
}

bool PosternakAIncreaseContrastSEQ::PreProcessingImpl() {
  return true;
}
bool PosternakAIncreaseContrastSEQ::RunImpl() {
  std::pair<std::string, std::string> &lines = GetInput();
  std::string s1 = lines.first;
  std::string s2 = lines.second;

  int diff_count = 0;
  size_t min = 0;
  size_t max = 0;
  size_t s1_len = s1.length();
  size_t s2_len = s2.length();
  if (s1_len >= s2_len) {
    min = s2_len;
    max = s1_len;
  } else {
    min = s1_len;
    max = s2_len;
  }
  for (size_t i = 0; i < min; i++) {
    if (s1[i] != s2[i]) {
      diff_count++;
    }
  }

  diff_count += static_cast<int>(max - min);
  GetOutput() = diff_count;
  return true;
}

bool PosternakAIncreaseContrastSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace posternak_a_increase_contrast
