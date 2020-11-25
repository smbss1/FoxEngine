#pragma once

#include <thread>

class JobSystem;
class JobQueue;
class Job;

class Worker
{
	public:
		enum class State : unsigned int
		{
			RUNNING = 0,
			IDLE
		};

		Worker( JobSystem *, JobQueue * );
		Worker( const Worker & ) = delete;
		~Worker();

		void StartBackgroundThread();
		void Stop();
		void Submit( Job *job );
		void Wait( Job *sentinel );
		bool IsRunning();

		const std::thread::id& GetThreadId() const;

	private:
		State m_eState;
		JobQueue *m_pQueue;
		JobSystem *m_pSystem;
		std::thread *m_pThread;
		std::thread::id m_oThreadId;

		Job* GetJob();
		void Loop();
};