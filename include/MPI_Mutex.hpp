#ifndef MPI_MUTEX_HPP
#define MPI_MUTEX_HPP

#include <fmt/format.h>

#include <mpi.h>

class MPI_Mutex {
public:
    MPI_Mutex() {
    }

    // constructor with communicator and target window
    MPI_Mutex(MPI_Comm &world_comm, MPI_Win &window) {
        this->world_comm = &world_comm;
        this->window = &window;
    }

    // set communicator and target window if not done at instance construction
    void set(MPI_Comm &world_comm, MPI_Win &window) {
        this->world_comm = &world_comm;
        this->window = &window;
    }

    /* start a critical section
        target_rank is where the remote memory is
        accessing_rank is the one acquiring the lock (optional) just debugging purposes
    */
    void lock(int target_rank, int accessing_rank = -1) {
        bool buffer = true;
        bool compare = false;
        bool result; // holds original value at target

        this->check_exception("lock");

        do {
            MPI_Win_lock(MPI::LOCK_EXCLUSIVE, target_rank, 0, *window);                           // open epoch
            MPI_Compare_and_swap(&buffer, &compare, &result, MPI::BOOL, target_rank, 0,
                                 *window); // perform MPI operation
            MPI_Win_flush(target_rank,
                          *window);                                                  // complete MPI operation
            MPI_Win_unlock(target_rank, *window);                                                 // close epoch
        } while (result == 1);

#ifdef DEBUG_COMMENTS
        fmt::print("MPI MUTEX ACQUIRED by rank {} \n", accessing_rank);
#endif
    }

    /* end critical section
        target_rank is where the remote memory is
        accessing_rank is the one acquiring the lock (optional) just debugging purposes
    */
    void unlock(int target_rank, int accessing_rank = -1) {
        bool buffer = false;
        bool compare = true;
        bool result; // holds original value at target

        this->check_exception("unlock");

        MPI_Win_lock(MPI::LOCK_EXCLUSIVE, target_rank, 0, *window);                           // open epoch
        MPI_Compare_and_swap(&buffer, &compare, &result, MPI::BOOL, target_rank, 0, *window); // perform MPI operation
        MPI_Win_flush(target_rank, *window);                                                  // complete MPI operation
        MPI_Win_unlock(target_rank, *window);                                                 // close epoch
#ifdef DEBUG_COMMENTS
        fmt::print("MPI MUTEX RELEASED by rank {} \n", accessing_rank);
#endif
    }

    void check_exception(std::string_view msg) {
        try {
            if (!world_comm || !window) {
                fmt::print("Exception about to occur in {}\n", msg);
                throw "communicator and/or window and/or mutex not initiliazed";
            }
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
        }
    }

    // Upon allocation [size = 1], this value should be initialized to false
    bool *mutex = nullptr; // It is strongly recommended to allocate using MPI_Win_allocate

    ~MPI_Mutex() {}

    MPI_Mutex(MPI_Mutex &) = delete;

    MPI_Mutex(MPI_Mutex &&) = delete;

    MPI_Mutex &operator=(MPI_Mutex &) = delete;

    MPI_Mutex &operator=(MPI_Mutex &&) = delete;

private:
    MPI_Comm *world_comm = nullptr;
    MPI_Win *window = nullptr;
};

#endif
