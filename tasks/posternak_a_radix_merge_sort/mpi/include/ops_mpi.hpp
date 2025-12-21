#pragma once

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

  void Merge(std::vector<int> &array, int left, int mid, int right, int radix);
  void MergeSort(std::vector<int> &array, int left, int right, int radix);
};

}  // namespace posternak_a_radix_merge_sort
