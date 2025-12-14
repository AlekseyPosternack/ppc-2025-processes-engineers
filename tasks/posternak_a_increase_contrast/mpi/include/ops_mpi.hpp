#pragma once

#include "posternak_a_increase_contrast/common/include/common.hpp"
#include "task/include/task.hpp"

namespace posternak_a_increase_contrast {

class PosternakAIncreaseContrastMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit PosternakAIncreaseContrastMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace posternak_a_increase_contrast
