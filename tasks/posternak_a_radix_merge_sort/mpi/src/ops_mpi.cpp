#include "posternak_a_radix_merge_sort/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
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

std::vector<uint32_t> PosternakARadixMergeSortMPI::RadixSortLocal(std::vector<int> &data) {
  constexpr int kNumBytes = 4;
  constexpr uint32_t kByteMask = 0xFFU;

  std::vector<uint32_t> unsigned_data(data.size());
  for (size_t i = 0; i < data.size(); i++) {
    unsigned_data[i] = static_cast<uint32_t>(data[i]) ^ 0x80000000U;
  }

  std::vector<uint32_t> buffer(data.size());

  for (int byte_index = 0; byte_index < kNumBytes; byte_index++) {
    std::vector<int> count(256, 0);
    for (uint32_t val : unsigned_data) {
      const auto b = static_cast<uint8_t>((val >> (byte_index * 8)) & kByteMask);
      ++count[b];
    }

    for (int i = 1; i < 256; ++i) {
      count[i] += count[i - 1];
    }

    for (int i = static_cast<int>(unsigned_data.size()) - 1; i >= 0; i--) {
      const auto b = static_cast<uint8_t>((unsigned_data[i] >> (byte_index * 8)) & kByteMask);
      buffer[--count[b]] = unsigned_data[i];
    }

    unsigned_data.swap(buffer);
  }

  return unsigned_data;
}

std::vector<int> PosternakARadixMergeSortMPI::ConvertToSigned(const std::vector<uint32_t> &unsigned_data) {
  std::vector<int> result(unsigned_data.size());
  for (size_t i = 0; i < unsigned_data.size(); i++) {
    result[i] = static_cast<int>(unsigned_data[i] ^ 0x80000000U);
  }
  return result;
}

void PosternakARadixMergeSortMPI::ComputeCountsAndDisplacements(int total_n, int world_size, std::vector<int> &counts,
                                                                std::vector<int> &displs) {
  const int base = total_n / world_size;
  const int extra = total_n % world_size;

  counts.resize(world_size);
  displs.resize(world_size, 0);

  for (int i = 0; i < world_size; i++) {
    counts[i] = base + (i < extra ? 1 : 0);
    if (i > 0) {
      displs[i] = displs[i - 1] + counts[i - 1];
    }
  }
}

void PosternakARadixMergeSortMPI::MergeChunks(std::vector<int> &global_result,
                                              const std::vector<std::vector<int>> &chunks) {
  std::vector<int> tmp;
  for (size_t i = 1; i < chunks.size(); i++) {
    tmp.resize(global_result.size() + chunks[i].size());
    std::merge(global_result.begin(), global_result.end(), chunks[i].begin(), chunks[i].end(), tmp.begin());
    global_result.swap(tmp);
  }
}

bool PosternakARadixMergeSortMPI::RunImpl() {
  int world_size = 0;
  int world_rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  int total_n = 0;
  std::vector<int> root_input;

  if (world_rank == 0) {
    root_input = GetInput();
    total_n = static_cast<int>(root_input.size());
  }

  MPI_Bcast(&total_n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (total_n == 0) {
    GetOutput() = std::vector<int>();
    return true;
  }

  std::vector<int> counts;
  std::vector<int> displs;
  ComputeCountsAndDisplacements(total_n, world_size, counts, displs);

  const int my_size = counts[world_rank];
  std::vector<int> input_local(my_size);

  if (world_rank == 0) {
    MPI_Scatterv(root_input.data(), counts.data(), displs.data(), MPI_INT, input_local.data(), my_size, MPI_INT, 0,
                 MPI_COMM_WORLD);
  } else {
    MPI_Scatterv(nullptr, counts.data(), displs.data(), MPI_INT, input_local.data(), my_size, MPI_INT, 0,
                 MPI_COMM_WORLD);
  }

  std::vector<uint32_t> unsigned_sorted = RadixSortLocal(input_local);
  std::vector<int> local_sorted = ConvertToSigned(unsigned_sorted);

  std::vector<int> global_result;
  if (world_rank == 0) {
    std::vector<std::vector<int>> chunks;
    chunks.reserve(static_cast<size_t>(world_size));
    chunks.push_back(std::move(local_sorted));

    for (int src = 1; src < world_size; src++) {
      std::vector<int> remote_chunk(counts[src]);
      MPI_Recv(remote_chunk.data(), counts[src], MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      chunks.push_back(std::move(remote_chunk));
    }

    global_result = std::move(chunks[0]);
    MergeChunks(global_result, chunks);
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
