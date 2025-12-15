#include "posternak_a_increase_contrast/seq/include/ops_seq.hpp"

#include <cstddef>
#include <string>
#include <utility>

#include "posternak_a_increase_contrast/common/include/common.hpp"

namespace posternak_a_increase_contrast {

PosternakAIncreaseContrastSEQ::PosternakAIncreaseContrastSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput().resize(in.size());
}

bool PosternakAIncreaseContrastSEQ::ValidationImpl() {
  return !GetInput().empty();
}

bool PosternakAIncreaseContrastSEQ::PreProcessingImpl() {
  return true;
}

bool PosternakAIncreaseContrastSEQ::RunImpl() {
  const std::vector<unsigned char> &input = GetInput();
  std::vector<unsigned char> &output = GetOutput();

  unsigned char min_val = *std::min_element(input.begin(), input.end());
  unsigned char max_val = *std::max_element(input.begin(), input.end());

  if (min_val == max_val) {
    std::fill(output.begin(), output.end(), 128);
    return true;
  }

  double scale = 255.0 / (max_val - min_val);

  for (size_t i = 0; i < input.size(); ++i) {
    double new_pixel = static_cast<unsigned char>((input[i] - min_val) * scale + 0.5);

    if (new_pixel < 0.0) {
      new_pixel = 0.0;
    }
    if (new_pixel > 255.0) {
      new_pixel = 255.0;
    }

    output[i] = static_cast<unsigned char>(new_pixel);
  }

  return true;
}

bool PosternakAIncreaseContrastSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace posternak_a_increase_contrast
