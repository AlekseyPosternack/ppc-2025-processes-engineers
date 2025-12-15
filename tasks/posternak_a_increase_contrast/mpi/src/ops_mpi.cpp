#include "posternak_a_increase_contrast/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <vector>

#include "posternak_a_increase_contrast/common/include/common.hpp"

namespace posternak_a_increase_contrast {

PosternakAIncreaseContrastMPI::PosternakAIncreaseContrastMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput().resize(in.size());
}

bool PosternakAIncreaseContrastMPI::ValidationImpl() {
  return !GetInput().empty();
}

bool PosternakAIncreaseContrastMPI::PreProcessingImpl() {
  return true;
}

bool PosternakAIncreaseContrastMPI::RunImpl() {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int data_len = 0;
  if (rank == 0) {
    data_len = static_cast<int>(GetInput().size());
  }
  MPI_Bcast(&data_len, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (data_len == 0) {
    GetOutput().clear();
    return true;
  }

  int local_size = data_len / size;
  int remainder = data_len % size;
  int my_size = local_size + (rank < remainder ? 1 : 0);

  std::vector<int> counts(size), step(size);
  int start = 0;
  for (int i = 0; i < size; ++i) {
    counts[i] = local_size + (i < remainder ? 1 : 0);
    step[i] = start;
    start += counts[i];
  }

  std::vector<unsigned char> proc_part(my_size);
  if (rank == 0) {
    MPI_Scatterv(GetInput().data(), counts.data(), step.data(), MPI_UNSIGNED_CHAR, proc_part.data(), my_size,
                 MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
  } else {
    MPI_Scatterv(nullptr, nullptr, nullptr, MPI_UNSIGNED_CHAR, proc_part.data(), my_size, MPI_UNSIGNED_CHAR, 0,
                 MPI_COMM_WORLD);
  }

  unsigned char local_min = 255, local_max = 0;
  for (unsigned char pixel : proc_part) {
    local_min = std::min(local_min, pixel);
    local_max = std::max(local_max, pixel);
  }

  unsigned char data_min, data_max;
  MPI_Allreduce(&local_min, &data_min, 1, MPI_UNSIGNED_CHAR, MPI_MIN, MPI_COMM_WORLD);
  MPI_Allreduce(&local_max, &data_max, 1, MPI_UNSIGNED_CHAR, MPI_MAX, MPI_COMM_WORLD);

  std::vector<unsigned char> local_output(my_size);
  if (data_min == data_max) {
    std::fill(local_output.begin(), local_output.end(), 128);
  } else {
    const double scale = 255.0 / (data_max - data_min);
    for (int i = 0; i < my_size; ++i) {
      int new_pixel = static_cast<int>((proc_part[i] - data_min) * scale + 0.5);
      if (new_pixel < 0.0) {
        new_pixel = 0.0;
      } else if (new_pixel > 255.0) {
        new_pixel = 255.0;
      }
      local_output[i] = static_cast<unsigned char>(new_pixel);
    }
  }

  // рассылка результата всем процессам
  // занимает время, но без него падает func тесты из-за их архитектуры
  GetOutput().resize(data_len);
  MPI_Allgatherv(local_output.data(), my_size, MPI_UNSIGNED_CHAR, GetOutput().data(), counts.data(), step.data(),
                 MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);

  return true;
}

bool PosternakAIncreaseContrastMPI::PostProcessingImpl() {
  return true;
}

}  // namespace posternak_a_increase_contrast
