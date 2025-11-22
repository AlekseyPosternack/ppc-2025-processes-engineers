#include "posternak_a_count_different_char_in_two_lines/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cmath>
#include <string>
#include <utility>

#include "posternak_a_count_different_char_in_two_lines/common/include/common.hpp"

namespace posternak_a_count_different_char_in_two_lines {

PosternakACountDifferentCharInTwoLinesMPI::PosternakACountDifferentCharInTwoLinesMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool PosternakACountDifferentCharInTwoLinesMPI::ValidationImpl() {
  std::pair<std::string, std::string> &lines = GetInput();
  std::string s1 = lines.first;
  std::string s2 = lines.second;
  return !s1.empty() && !s2.empty();
}

bool PosternakACountDifferentCharInTwoLinesMPI::PreProcessingImpl() {
  return true;
}

bool PosternakACountDifferentCharInTwoLinesMPI::RunImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int count = 0;

  std::pair<std::string, std::string> &lines = GetInput();

  std::string &s1 = lines.first;
  std::string &s2 = lines.second;

  size_t s1_len = lines.first.length();
  size_t s2_len = lines.second.length();

  int min_len = 0;
  if (s1_len >= s2_len) {
    min_len = s2_len;
  } else {
    min_len = s1_len;
  }

  int process_work_size = min_len / size;
  int start = rank * process_work_size;
  int end = start + process_work_size;

  if (rank == size - 1) {
    end = min_len;
  }

  int process_count = 0;
  for (int i = start; i < end; i++) {
    if (s1[i] != s2[i]) {
      process_count++;
    }
  }

  MPI_Allreduce(&process_count, &count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  count += std::abs(static_cast<int>(s1_len) - static_cast<int>(s2_len));
  GetOutput() = count;

  return true;
}

bool PosternakACountDifferentCharInTwoLinesMPI::PostProcessingImpl() {
  return true;
}

}  // namespace posternak_a_count_different_char_in_two_lines
