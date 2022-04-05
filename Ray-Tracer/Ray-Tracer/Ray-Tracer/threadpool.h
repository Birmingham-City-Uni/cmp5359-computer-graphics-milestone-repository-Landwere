#pragma once
#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <queue>
/*Source from Ryan Westwood (by email) starts here:*/
class ThreadPool {
public: 
	using Task = std::function<void()>;

	ThreadPool(short threadCount) {
		Start(threadCount);
}

	~ThreadPool() {
		Stop();
	}

	void Enqueue(Task task) {
		{
			std::unique_lock<std::mutex> lock(mMutex);
			mTasks.emplace(std::move(task));
		}
		mCondidion.notify_one();
	}

private:
	std::vector<std::thread> mThreads;
	std::condition_variable mCondidion;
	std::mutex mMutex;
	std::queue<Task> mTasks;

	bool mRunning = false;

	void Start(short threadCount) {
		for (short i = 0; i < threadCount; i++)
		{
			mThreads.emplace_back([=] {
				while (true)
				{
					Task task;
					{
						std::unique_lock<std::mutex> lock(mMutex);

						mCondidion.wait(lock, [=] {return mRunning || !mTasks.empty(); });

						if (mRunning && mTasks.empty())
							break;

						task = std::move(mTasks.front());
						mTasks.pop();
					}
					task();
				}
				});
		}
	}

	void Stop() {
		{
			std::unique_lock<std::mutex> lock(mMutex);
			mRunning = true;
		}

		mCondidion.notify_all();

		for (auto& thread : mThreads)
			thread.join();
	}



};