#include "posternak_a_radix_merge_sort/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <vector>
#include <cstdint> 

#include "posternak_a_radix_merge_sort/common/include/common.hpp"

namespace posternak_a_radix_merge_sort {

PosternakARadixMergeSortSEQ::PosternakARadixMergeSortSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool PosternakARadixMergeSortSEQ::ValidationImpl() {
  return !GetInput().empty();
}

bool PosternakARadixMergeSortSEQ::PreProcessingImpl() {
  return true;
}

bool PosternakARadixMergeSortSEQ::RunImpl() {
  const std::vector<int>& input = GetInput();
  const int n = static_cast<int>(input.size());

  std::vector<uint32_t> unsigned_data(n);
  for (int i = 0; i < n; i++) {
    unsigned_data[i] = static_cast<uint32_t>(input[i]) ^ 0x80000000u;
  }

  std::vector<uint32_t> buffer(n);

  constexpr int NUM_BYTES = 4;
  constexpr uint32_t BYTE_MASK = 0xFF;

  for (int byte_index = 0; byte_index < NUM_BYTES; byte_index++) {
    std::vector<int> count(256, 0);

    for (uint32_t value : unsigned_data) {
      uint8_t current_byte = (value >> (byte_index * 8)) & BYTE_MASK;
      count[current_byte]++;
    }

    for (int i = 1; i < 256; i++) {
      count[i] += count[i - 1];
    }

    for (int i = static_cast<int>(n) - 1; i >= 0; i--) {
      uint8_t current_byte = (unsigned_data[i] >> (byte_index * 8)) & BYTE_MASK;
      buffer[--count[current_byte]] = unsigned_data[i];
    }

    unsigned_data.swap(buffer);
  }

  std::vector<int> sorted_output(n);
  for (int i = 0; i < n; i++) {
    sorted_output[i] = static_cast<uint32_t>(unsigned_data[i]) ^ 0x80000000u;;
  }

  GetOutput() = std::move(sorted_output);
  return true;
}

bool PosternakARadixMergeSortSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace posternak_a_radix_merge_sort
