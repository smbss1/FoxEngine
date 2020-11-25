#include "JobSystem/Job.hpp"

// Job::Job( JobFunction function, Job *parent, void *data ) 
// 	: function( function ), parent ( parent ), data ( data ), pendingJobs { 1 } 
// {
// 	if ( parent != nullptr )
// 	{
// 		parent->pendingJobs++;
// 	}
// }

// Job::Job()
// 	: function( nullptr ), parent ( nullptr ), data ( nullptr ), pendingJobs { 1 } 
// {
// }

void Job::Init(JobFunction function, Job *parent, void *data)
{
	m_oFunction = function;
	m_pParent = parent;
	m_pData = data;
	m_lPendingJobs = 1;
	
	if ( parent != nullptr )
	{
		m_pParent->m_lPendingJobs++;
	}
}

bool Job::IsFinished() const
{
	return ( m_lPendingJobs == 0 );
}

void Job::Execute()
{
	if ( m_oFunction != nullptr )
	{
		m_oFunction( this, m_pData );
	}
	Finish();
}

void Job::Finish()
{
	m_lPendingJobs--;

	if ( IsFinished() && m_pParent != nullptr )
	{
		m_pParent->Finish();
	}
}