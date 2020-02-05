#pragma once

class ThreadPool
{
public:
	ThreadPool(size_t threadCount);
	~ThreadPool();

	void WorkerThread();
	void EnqueueJob(std::function<void()> job);

	template <class F, class... Args>
	std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(F&& f, Args&&... args);

private:
	size_t m_threadCount;
	std::vector<std::thread> m_workerThreads;

	std::queue<std::function<void()>> m_jobs;
	std::mutex m_mutexJobs;
	std::condition_variable m_cvJobs;

	bool m_stopAll;
};

