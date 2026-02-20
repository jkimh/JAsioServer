#pragma once

class ThreadPool
{
public:
	ThreadPool(size_t threadCount);
	~ThreadPool();

	void WorkerThread();
	void EnqueueJob(std::function<void()> job);

	template <class F, class... Args>
	std::future<std::invoke_result_t<F, Args...>> EnqueueJob(F&& f, Args&&... args)
	{
		if (m_stopAll)
		{
			throw std::runtime_error("thread »ç¿ë ÁßÁö µÊ");
		}

		using job_result_type = std::invoke_result_t<F, Args...>;
		auto job = std::make_shared<std::packaged_task<job_result_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<job_result_type> job_result_future = job->get_future();
		{
			std::lock_guard<std::mutex> lock(m_mutexJobs);
			m_jobs.push([job]() { (*job)(); });
		}
		m_cvJobs.notify_one();
		return job_result_future;
	}

private:
	size_t m_threadCount;
	std::vector<std::thread> m_workerThreads;

	std::queue<std::function<void()>> m_jobs;
	std::mutex m_mutexJobs;
	std::condition_variable m_cvJobs;

	bool m_stopAll;
};

