#include "mono_gc_handle.h"

#include "Core/Assert.hpp"

namespace fox::mono
{

void mono_gc_handle::lock()
{
	FOX_ASSERT(handle_ == 0);
	handle_ = mono_gchandle_new(object_, 1);
}

void mono_gc_handle::unlock()
{
	FOX_ASSERT(handle_ != 0);
	mono_gchandle_free(handle_);
	handle_ = 0;
}

} // namespace fox::mono
