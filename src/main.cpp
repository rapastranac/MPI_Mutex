#include <fmt/format.h>
#include "mpi.h"
#include "MPI_Mutex.hpp"
#include <thread>
#include <chrono>

int main() {
    MPI_Init(nullptr, nullptr);

    MPI_Comm world_Comm;

    MPI_Mutex mpi_mutex;
    MPI_Win win_mutex;
    MPI_Comm_dup(MPI_COMM_WORLD, &world_Comm);

    int world_rank;      // get the rank of the process
    int world_size;

    MPI_Comm_size(world_Comm, &world_size);
    MPI_Comm_rank(world_Comm, &world_rank);

    char processor_name[128];
    int namelen;
    MPI_Get_processor_name(processor_name, &namelen);

    fmt::print("Process {} of {} is on {}\n", world_rank, world_size, processor_name);

    if (world_rank == 0) {
        // mpi mutex **********************************************************************************************
        MPI_Win_allocate(sizeof(bool), sizeof(bool), MPI::INFO_NULL, world_Comm, &mpi_mutex.mutex, &win_mutex);
        // ********************************************************************************************************
        mpi_mutex.mutex[0] = false; // initialization
    } else {
        // it is not required to allocate buffer memory for the other processes

        // mpi mutex **********************************************************************************************
        MPI_Win_allocate(0, sizeof(bool), MPI::INFO_NULL, world_Comm, &mpi_mutex.mutex, &win_mutex);
        // ********************************************************************************************************
    }
    //fmt::print("rank {} about to initialize mutex\n", world_rank);
    mpi_mutex.set(world_Comm, win_mutex); // mutex initialization
    MPI_Barrier(world_Comm);

    // RM in rank 0
    mpi_mutex.lock(0, world_rank);

   //fmt::print("line passed by rank {}\n", world_rank);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    fmt::print("rank {} entered critical section \n", world_rank);

    mpi_mutex.unlock(0, world_rank);
    fmt::print("rank {} left critical section \n", world_rank);

    MPI_Win_free(&win_mutex);
    MPI_Comm_free(&world_Comm);

    MPI_Finalize();
    return 0;
}
