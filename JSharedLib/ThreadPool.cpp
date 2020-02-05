#include "stdafx.h"
#include "ThreadPool.h"


ThreadPool::ThreadPool(size_t threadCount)
	: m_threadCount(threadCount), m_stopAll(false)
{
	m_workerThreads.reserve(threadCount);
	for (size_t i = 0; i < threadCount; i++)
	{
		m_workerThreads.emplace_back([this]() {this->WorkerThread(); });
	}
}

ThreadPool::~ThreadPool()
{
	m_stopAll = true;
	m_cvJobs.notify_all();

	for (auto& t : m_workerThreads)
	{
		t.join();
	}
}

void ThreadPool::WorkerThread()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m_mutexJobs);
		m_cvJobs.wait(lock, [this]() {return !this->m_jobs.empty() || m_stopAll; });
		if (m_stopAll && this->m_jobs.empty())
			return;

		std::function<void()> job = std::move(m_jobs.front());
		m_jobs.pop();
		lock.unlock();
		job();
	}
}

void ThreadPool::EnqueueJob(std::function<void()> job)
{
	if (m_stopAll)
	{
		throw std::runtime_error("thread »ç¿ë ÁßÁö µÊ");
	}
	{
		std::lock_guard<std::mutex> lock(m_mutexJobs);
		m_jobs.push(std::move(job));
	}
	m_cvJobs.notify_one();
}

template<class F, class ...Args>
std::future<typename std::result_of<F(Args...)>::type> ThreadPool::EnqueueJob(F&& f, Args&& ...args)
{
	if (m_stopAll)
	{
		throw std::runtime_error("thread »ç¿ë ÁßÁö µÊ");
	}
	using job_result_type = typename std::result_of<F(Args...)>::type;
	auto job = std::make_shared<std::packaged_task<job_result_type()>>(std::bind(f, args...));
	std::future<job_result_type> job_result_future = job.get_future();
	{
		std::lock_guard<std::mutex> lock(m_mutexJobs);
		m_jobs.push([job](){ (*job)(); });
	}
	m_cvJobs.notify_one();
	return job_result_future;
}