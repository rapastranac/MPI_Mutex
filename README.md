# MPI_Mutex
Mutex emulation for MPI

#Usage

This mutex is aimed to lock a specific remote memory (RM), say rank 0. This means that the boolean that acts as a mutex is located in a specific rank. However, when a critical section is acquired, any other communication can be process safely.

Then, be sure of creating a communicator and window that will be used with the corresponding ranks. If all ranks will have to syncrhonised with the RM at rank 0, then it can be any world_communicator.

To allocate use it this way.
'''

      MPI_Mutex mpi_mutex;
      
      MPI_Comm mutex_Comm;
      
      MPI_Win win_mutex;   
      
      MPI_Comm_dup(MPI_COMM_WORLD, &mutex_Comm);
      
      if (world_rank == 0)
			{
				// mpi mutex **********************************************************************************************
				MPI_Win_allocate(sizeof(bool), sizeof(bool), MPI::INFO_NULL, mutex_Comm, &mpi_mutex.mutex, &win_mutex);
				// ********************************************************************************************************
			}
			else
			{
				// it is not required to allocate buffer memory for the other processes

				// mpi mutex **********************************************************************************************
				MPI_Win_allocate(0, sizeof(bool), MPI::INFO_NULL, mutex_Comm, &mpi_mutex.mutex, &win_mutex);
				// ********************************************************************************************************
			}
      
      mpi_mutex.set(mutex_Comm, win_mutex);
      
'''

to use it, where all participant ranks are able to reach it.

'''

  // RM in rank 0
  
  mpi_mutex.lock(0); 
  
  /*
  critical section
  */
  
  mpi_mutex.lock(0);


'''

That's all folks
