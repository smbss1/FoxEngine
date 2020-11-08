#include "JobSystem/Worker.hpp"

#include <vector>
#include <thread>

#include "JobSystem/JobQueue.hpp"
#include "JobSystem/JobSystem.hpp"
#include "JobSystem/Job.hpp"

Worker::Worker( JobSystem *system, JobQueue *queue ) 
	: m_pSystem( system ), m_pQueue( queue ), m_pThread( nullptr ), m_oThreadId(std::this_thread::get_id()) {}

Worker::~Worker()
{
	Stop();
	if ( m_pThread != nullptr )
	{
		m_pThread->join();
		delete m_pThread;
	}
}

void Worker::StartBackgroundThread()
{
	m_eState = State::RUNNING;
	m_pThread = new std::thread( &Worker::Loop, this );
	m_oThreadId = m_pThread->get_id();
}

void Worker::Stop()
{
	m_eState = State::IDLE;
}

void Worker::Submit( Job *job )
{
	m_pQueue->Push( job );
}

void Worker::Wait( Job *sentinel )
{
	while ( !sentinel->IsFinished() )
	{
		Job *job = GetJob();
		if ( job != nullptr )
		{
			job->Execute();
			if ( job->IsFinished() )
			{
				// delete job;
				job = nullptr;
			}
		}
	}
}

void Worker::Loop()
{
	while ( IsRunning() )
	{
		Job *job = GetJob();
		if ( job != nullptr )
		{
			job->Execute();
			if ( job->IsFinished() )
			{
				// delete job;
				job = nullptr;
			}
		}
	}
}

Job* Worker::GetJob()
{
	Job *job = m_pQueue->Pop();

	if ( job == nullptr )
	{
		JobQueue *randomQueue = m_pSystem->GetRandomJobQueue();
		if ( randomQueue == nullptr )
		{
			std::this_thread::yield();
			return nullptr;
		}

		if ( m_pQueue == randomQueue )
		{
			std::this_thread::yield();
			return nullptr;
		}

		job = randomQueue->Steal();
		if ( job == nullptr )
		{
			std::this_thread::yield();
			return nullptr;
		}
	}
	return job;
}

bool Worker::IsRunning()
{
	return ( m_eState == State::RUNNING );
}

const std::thread::id& Worker::GetThreadId() const
{
	return m_oThreadId;
}
