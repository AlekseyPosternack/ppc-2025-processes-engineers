#pragma once

#include <cstdint>
#include <vector>

#include "posternak_a_radix_merge_sort/common/include/common.hpp"
#include "task/include/task.hpp"

namespace posternak_a_radix_merge_sort {

class PosternakARadixMergeSortMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit PosternakARadixMergeSortMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  std::vector<uint32_t> RadixSortLocal(std::vector<int> &data);
  std::vector<int> ConvertToSigned(const std::vector<uint32_t> &unsigned_data);
  void ComputeCountsAndDisplacements(int total_n, int world_size, std::vector<int> &counts, std::vector<int> &displs);
  void MergeChunks(std::vector<int> &global_result, const std::vector<std::vector<int>> &chunks);
};

}  // namespace posternak_a_radix_merge_sort
