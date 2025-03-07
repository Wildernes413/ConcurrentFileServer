#include "Thread.h"

ThreadPool::ThreadPool(size_t threadnum)
{
	for (size_t i = 0; i < threadnum; ++i)
	{
		workers.emplace_back([this] {
			while (1)
			{
				std::function<void()> task;
				{
					//加锁，访问任务队列
					std::unique_lock<std::mutex> lock(this->queue_mutex);
					//等待任务队列不为空或线程池停止
					this->condition.wait(lock, [this] {
						return this->stop || !this->tasks.empty();
						});
					//如果线程池停止并且任务队列为空
					if (this->stop && this->tasks.empty()) {
						return;
						}
					//从任务队列中取出一个任务
					task = std::move(this->tasks.front());
					this->tasks.pop();
				}
				//执行任务	
				task();
			}
		});
	}
}

//析构函数，停止线程池
ThreadPool::~ThreadPool()
{
	{
		//加锁，设置停止标志
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread& worker : workers)
	{
		worker.join();
	}
}