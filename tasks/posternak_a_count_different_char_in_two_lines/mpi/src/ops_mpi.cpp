#include "posternak_a_count_different_char_in_two_lines/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cmath>
#include <cstddef>
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

  std::string s1 = "";
  std::string s2 = "";

  int count = 0;

  if (rank == 0) {
    std::pair<std::string, std::string> &lines = GetInput();
    s1 = lines.first;
    s2 = lines.second;
  }

  int string_lens[2] = {0, 0};
  if (rank == 0) {
    string_lens[0] = s1.length();
    string_lens[1] = s2.length();
  }

  MPI_Bcast(string_lens, 2, MPI_INT, 0, MPI_COMM_WORLD);

  int s1_len = string_lens[0];
  int s2_len = string_lens[1];

  int min_len = 0;
  if (s1_len >= s2_len) {
    min_len = s2_len;
  } else {
    min_len = s1_len;
  }

  if (rank != 0) {
    s1.resize(s1_len);
    s2.resize(s2_len);
  }

  MPI_Bcast(&s1[0], s1_len, MPI_CHAR, 0, MPI_COMM_WORLD);
  MPI_Bcast(&s2[0], s2_len, MPI_CHAR, 0, MPI_COMM_WORLD);

  int process_workplace = min_len / size;
  int start_place = rank * process_workplace;
  int end_place = start_place + process_workplace;

  if (rank == size - 1) {
    end_place = min_len;
  }

  int process_count = 0;
  for (int i = start_place; i < end_place; i++) {
    if (s1[i] != s2[i]) {
      process_count++;
    }
  }

  MPI_Allreduce(&process_count, &count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  count += std::abs(s1_len - s2_len);
  GetOutput() = count;

  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool PosternakACountDifferentCharInTwoLinesMPI::PostProcessingImpl() {
  return true;
}

}  // namespace posternak_a_count_different_char_in_two_lines
