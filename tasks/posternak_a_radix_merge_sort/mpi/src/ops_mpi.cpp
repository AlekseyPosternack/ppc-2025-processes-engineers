#include "posternak_a_radix_merge_sort/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "posternak_a_radix_merge_sort/common/include/common.hpp"

namespace posternak_a_radix_merge_sort {

PosternakARadixMergeSortMPI::PosternakARadixMergeSortMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool PosternakARadixMergeSortMPI::ValidationImpl() {
  const std::vector<int> &input = GetInput();
  return !input.empty();
}

bool PosternakARadixMergeSortMPI::PreProcessingImpl() {
  return true;
}

bool PosternakARadixMergeSortMPI::RunImpl() {
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  std::vector<int> input_local;
  int total_n = 0;
  const std::vector<int> *root_input_ptr = nullptr;

  if (world_rank == 0) {
    input_local = GetInput();
    total_n = input_local.size();
    root_input_ptr = &GetInput();
  }

  MPI_Bcast(&total_n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (total_n == 0) {
    GetOutput() = std::vector<int>();
    return true;
  }

  int base = total_n / world_size;
  int extra = total_n % world_size;
  int my_size = base + (world_rank < (int)extra ? 1 : 0);

  std::vector<int> counts(world_size);
  std::vector<int> displs(world_size, 0);
  for (int i = 0; i < world_size; i++) {
    counts[i] = base + (i < (int)extra ? 1 : 0);
    if (i > 0) {
      displs[i] = displs[i - 1] + counts[i - 1];
    }
  }

  input_local.resize(my_size);

  // Fix: Separate the send buffer logic to avoid ternary operator with nullptr
  const int *sendbuf = nullptr;
  if (world_rank == 0) {
    sendbuf = root_input_ptr->data();
  }

  MPI_Scatterv(sendbuf, counts.data(), displs.data(), MPI_INT, input_local.data(), my_size, MPI_INT, 0, MPI_COMM_WORLD);

  std::vector<uint32_t> unsigned_data(my_size);
  for (int i = 0; i < my_size; i++) {
    unsigned_data[i] = static_cast<uint32_t>(input_local[i]) ^ 0x80000000u;
  }

  std::vector<uint32_t> buffer(my_size);
  constexpr int NUM_BYTES = 4;
  constexpr uint32_t BYTE_MASK = 0xFF;

  for (int byte_index = 0; byte_index < NUM_BYTES; ++byte_index) {
    std::vector<int> count(256, 0);
    for (uint32_t val : unsigned_data) {
      uint8_t b = (val >> (byte_index * 8)) & BYTE_MASK;
      count[b]++;
    }
    for (int i = 1; i < 256; i++) {
      count[i] += count[i - 1];
    }
    for (int i = my_size - 1; i >= 0; i--) {
      uint8_t b = (unsigned_data[i] >> (byte_index * 8)) & BYTE_MASK;
      buffer[--count[b]] = unsigned_data[i];
    }
    unsigned_data.swap(buffer);
  }

  std::vector<int> local_sorted(my_size);
  for (int i = 0; i < my_size; i++) {
    local_sorted[i] = static_cast<int>(unsigned_data[i] ^ 0x80000000u);
  }

  std::vector<int> global_result;
  if (world_rank == 0) {
    std::vector<std::vector<int>> chunks(world_size);
    chunks[0] = local_sorted;

    for (int src = 1; src < world_size; src++) {
      chunks[src].resize(counts[src]);
      MPI_Recv(chunks[src].data(), counts[src], MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    global_result = chunks[0];
    std::vector<int> tmp;
    for (int i = 1; i < world_size; i++) {
      tmp.clear();
      tmp.resize(global_result.size() + chunks[i].size());
      std::merge(global_result.begin(), global_result.end(), chunks[i].begin(), chunks[i].end(), tmp.begin());
      global_result.swap(tmp);
    }
  } else {
    MPI_Send(local_sorted.data(), my_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  std::vector<int> final_output(total_n);
  if (world_rank == 0) {
    final_output = std::move(global_result);
  }
  MPI_Bcast(final_output.data(), total_n, MPI_INT, 0, MPI_COMM_WORLD);

  GetOutput() = std::move(final_output);
  return true;
}

bool PosternakARadixMergeSortMPI::PostProcessingImpl() {
  return true;
}

}  // namespace posternak_a_radix_merge_sort
