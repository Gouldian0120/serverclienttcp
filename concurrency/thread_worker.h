#pragma once

#include "job_priorities.h"

#include <mutex>
#include <atomic>
#include <thread>
#include <vector>
#include <condition_variable>

namespace concurrency
{
	class job;
	class thread_worker
	{
	public:
		thread_worker(const priorities& priority, const std::vector<priorities>& others = {});
		~thread_worker(void);

	public:
		void start(void);
		void stop(void);

	public:
		const priorities priority(void);

	protected:
		void run(void);
		void notification(const priorities& priority);

	protected:
		virtual void working(std::shared_ptr<job> current_job);

	protected:
		bool check_condition(const bool& ignore_job);

	private:
		std::atomic<bool> _thread_stop{ false };

	private:
		priorities _priority;
		std::vector<priorities> _others;

	private:
		std::mutex _mutex;
		std::thread _thread;
		std::condition_variable _condition;
	};
}

