#pragma once



#include "FMonoObject.hpp"

namespace fox::mono
{

class mono_gc_handle : public FMonoObject
{
public:
	using FMonoObject::FMonoObject;

	void lock();
	void unlock();

private:
	std::uint32_t handle_ = 0;
};

class mono_scoped_gc_handle
{
public:
	mono_scoped_gc_handle(const mono_scoped_gc_handle&) noexcept = delete;
	auto operator=(const mono_scoped_gc_handle&) noexcept -> mono_scoped_gc_handle& = delete;

	explicit mono_scoped_gc_handle(mono_gc_handle& handle)
		: handle_(handle)
	{
		handle_.lock();
	}

	~mono_scoped_gc_handle()
	{
		handle_.unlock();
	}

	auto get_handle() const -> mono_gc_handle&
	{
		return handle_;
	}

private:
	mono_gc_handle& handle_;
};

} // namespace fox::mono
