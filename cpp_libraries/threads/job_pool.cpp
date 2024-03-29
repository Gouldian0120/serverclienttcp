﻿#include "job_pool.h"

#include "job.h"
#include "logging.h"

#include "fmt/format.h"

namespace threads
{
	using namespace logging;

	job_pool::job_pool(void) : _lock_condition(false)
	{

	}

	job_pool::~job_pool(void)
	{
		_jobs.clear();
	}

	std::shared_ptr<job_pool> job_pool::get_ptr(void)
	{
		return shared_from_this();
	}

	void job_pool::set_push_lock(const bool& lock_condition)
	{
		_lock_condition = lock_condition;
	}

	void job_pool::push(std::shared_ptr<job> new_job)
	{
		if (new_job == nullptr)
		{
			return;
		}

		if (_lock_condition)
		{
			return;
		}

		new_job->set_job_pool(get_ptr());

		std::scoped_lock<std::mutex> guard(_mutex);

		auto iterator = _jobs.find(new_job->priority());
		if (iterator != _jobs.end())
		{
			iterator->second.push(new_job);

			logger::handle().write(logging::logging_level::parameter, fmt::format(L"push new job: priority - {}", new_job->priority()));
		}
		else
		{
			std::queue<std::shared_ptr<job>> queue;
			queue.push(new_job);

			_jobs.insert({ new_job->priority(), queue });

			logger::handle().write(logging::logging_level::parameter, fmt::format(L"push new job: priority - {}", new_job->priority()));
		}

		notification(new_job->priority());
	}

	std::shared_ptr<job> job_pool::pop(const priorities& priority, const std::vector<priorities>& others)
	{
		std::scoped_lock<std::mutex> guard(_mutex);

		auto iterator = _jobs.find(priority);
		if (iterator != _jobs.end() && !iterator->second.empty())
		{
			std::shared_ptr<job> temp = iterator->second.front();
			iterator->second.pop();

			logger::handle().write(logging::logging_level::parameter, fmt::format(L"pop a job: priority - {}", temp->priority()));

			return temp;
		}

		for (auto& other : others)
		{
			auto iterator2 = _jobs.find(other);
			if (iterator2 != _jobs.end() && !iterator2->second.empty())
			{
				std::shared_ptr<job> temp = iterator2->second.front();
				iterator2->second.pop();

				logger::handle().write(logging::logging_level::parameter, fmt::format(L"pop a job: priority - {}", temp->priority()));

				return temp;
			}
		}

		return nullptr;
	}

	bool job_pool::contain(const priorities& priority, const std::vector<priorities>& others)
	{
		std::scoped_lock<std::mutex> guard(_mutex);

		auto iterator = _jobs.find(priority);
		if (iterator != _jobs.end() && !iterator->second.empty())
		{
			std::shared_ptr<job> temp = iterator->second.front();

			return temp != nullptr;
		}

		for (auto& other : others)
		{
			auto iterator = _jobs.find(priority);
			if (iterator != _jobs.end() && !iterator->second.empty())
			{
				std::shared_ptr<job> temp = iterator->second.front();

				return temp != nullptr;
			}
		}

		return false;
	}

	void job_pool::append_notification(const std::function<void(const priorities&)>& notification)
	{
		_notifications.push_back(notification);
	}

	void job_pool::notification(const priorities& priority)
	{
		for (auto& notification : _notifications)
		{
			if (notification == nullptr)
			{
				continue;
			}

			notification(priority);
		}
	}
}