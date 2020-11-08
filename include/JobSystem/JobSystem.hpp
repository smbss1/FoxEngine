
#pragma once

#include <thread>
#include <vector>

#include "Job.hpp"

class JobQueue;
class Worker;
class FreeListAllocator;

class JobSystem
{
	public:
		JobSystem( size_t workersCount, size_t jobsPerWorker );
		JobSystem( size_t jobsPerWorker );

		~JobSystem();

		Job* CreateEmptyJob() const;
		Job* CreateJob( JobFunction function ) const;
		Job* CreateJob( JobFunction function, void * data ) const;
		Job* CreateJobAsChild( JobFunction function, Job *parent ) const;
		Job* CreateJobAsChild( JobFunction function, Job *parent, void *data ) const;

		void Run( Job *job );
		void Wait( Job *job );

		void ClearJobQueues();
		JobQueue* GetRandomJobQueue();
		// Job* AllocateJobs();

	private:
		size_t m_lWorkersCount;

		std::vector< Worker * > m_vWorkers;
		std::vector< JobQueue * > m_vQueues;
		FreeListAllocator* m_pFreeListAllocator;

		Worker * FindWorkerWithThreadID( const std::thread::id &id ) const;
};