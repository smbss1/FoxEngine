
#include <random>
#include <thread>
#include <iostream>

#include "JobSystem/JobSystem.hpp"
#include "Allocator/FreeListAllocator.h"

#include "JobSystem/JobQueue.hpp"
#include "JobSystem/Worker.hpp"

JobSystem::JobSystem( size_t workersCount, size_t jobsPerWorker) : m_lWorkersCount( workersCount )  //, m_uAllocatedJobs(0u)
{
	m_vQueues.reserve( workersCount );
	m_vWorkers.reserve( workersCount );

	// Add a main thread worker
	JobQueue *queue = new JobQueue( jobsPerWorker );
	m_vQueues.push_back( queue );
	Worker *mainThreadWorker = new Worker( this, queue );
	m_vWorkers.push_back( mainThreadWorker );
    m_pFreeListAllocator = new FreeListAllocator(workersCount * jobsPerWorker * sizeof(Job), FreeListAllocator::PlacementPolicy::FIND_FIRST);
	m_pFreeListAllocator->Init();

	// Add the rest of the workers
	for ( size_t i = 0; i < workersCount; ++i )
	{
		JobQueue *queue = new JobQueue( jobsPerWorker );
		m_vQueues.push_back( queue );
		Worker *worker = new Worker( this, queue );
		m_vWorkers.push_back( worker );
	}

	// Start background thread for all the workers except the main thread one (first on the vector)
	for ( size_t i = 1; i <= workersCount; ++i )
	{
		m_vWorkers[i]->StartBackgroundThread();
	}
}

JobSystem::JobSystem( size_t jobsPerWorker )
{
	// Retrieve the number of hardware threads in this system:
	auto numCores = std::thread::hardware_concurrency();
	// Calculate the actual number of worker threads we want:
	size_t lWorkersCount = std::max(1u, numCores);

	m_lWorkersCount = lWorkersCount;

	m_vQueues.reserve( m_lWorkersCount );
	m_vWorkers.reserve( m_lWorkersCount );

	// Add a main thread worker
	JobQueue *queue = new JobQueue( jobsPerWorker );
	m_vQueues.push_back( queue );
	Worker *mainThreadWorker = new Worker( this, queue );
	m_vWorkers.push_back( mainThreadWorker );
    m_pFreeListAllocator = new FreeListAllocator(m_lWorkersCount * jobsPerWorker * sizeof(Job), FreeListAllocator::PlacementPolicy::FIND_FIRST);
	m_pFreeListAllocator->Init();

	// Add the rest of the workers
	for ( size_t i = 0; i < m_lWorkersCount; ++i )
	{
		JobQueue *queue = new JobQueue( jobsPerWorker );
		m_vQueues.push_back( queue );
		Worker *worker = new Worker( this, queue );
		m_vWorkers.push_back( worker );
	}

	// Start background thread for all the workers except the main thread one (first on the vector)
	for ( size_t i = 1; i <= m_lWorkersCount; ++i )
	{
		m_vWorkers[i]->StartBackgroundThread();
	}
}

JobSystem::~JobSystem()
{
	for ( Worker *worker : m_vWorkers)
	{
		delete worker;
	}
	m_vWorkers.clear();

	for ( JobQueue *queue : m_vQueues )
	{
		delete queue;
	}
	m_vQueues.clear();
	delete m_pFreeListAllocator;
}

Job* JobSystem::CreateEmptyJob() const
{
	return CreateJob( nullptr, nullptr );
}

Job* JobSystem::CreateJob( JobFunction function ) const
{
	return CreateJob( function, nullptr );
}

Job* JobSystem::CreateJob( JobFunction function, void *data ) const
{
	Job* pJob = New<Job>(*m_pFreeListAllocator);
	pJob->Init(function, nullptr, data);
	return pJob;
}

Job* JobSystem::CreateJobAsChild( JobFunction function, Job *parent ) const
{
	return CreateJobAsChild( function, parent, nullptr );
}

Job* JobSystem::CreateJobAsChild( JobFunction function, Job *parent, void *data ) const
{
	Job* pJob = New<Job>(*m_pFreeListAllocator);
	pJob->Init(function, parent, data);
	return pJob;
}

void JobSystem::Run( Job *job )
{
	Worker * worker = FindWorkerWithThreadID( std::this_thread::get_id() );
	if ( worker != nullptr )
	{
		worker->Submit( job );
	}
}

void JobSystem::Wait( Job *job )
{
	Worker * worker = FindWorkerWithThreadID( std::this_thread::get_id() );
	if ( worker != nullptr )
	{
		worker->Wait( job );
	}
}

JobQueue* JobSystem::GetRandomJobQueue()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distribution( 0, m_lWorkersCount );

	size_t index = static_cast<size_t> ( std::round( distribution( gen ) ) );
	return m_vQueues[ index ];
}

Worker * JobSystem::FindWorkerWithThreadID( const std::thread::id &id ) const
{
	for ( Worker *worker : m_vWorkers )
	{
		if ( id == worker->GetThreadId() )
		{
			return worker;
		}
	}
	return nullptr;
}

void JobSystem::ClearJobQueues()
{
	for ( JobQueue *queue : m_vQueues )
	{
		queue->Clear();
	}
}