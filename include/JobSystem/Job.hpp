#pragma once

#include <atomic>
#include <functional>


class Job;

using JobFunction = std::function<void( Job*, void * )>;

class Job
{
	public:
		// Job();
		// Job( JobFunction function, Job *parent, void *data );
		
		void Init(JobFunction function, Job *parent, void *data);
		void Execute();
		void Finish();
		bool IsFinished() const;

	private:
		JobFunction m_oFunction;
		Job *m_pParent;
		std::atomic_size_t m_lPendingJobs;
		void *m_pData;
};