#pragma once
#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <functional>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>

class ThreadPool
{
public:
	ThreadPool(size_t threadnum);
	//提交任务到线程池
	//提交任务到线程池
	template <class F>
	void enqueue(F&& f)
	{
		{
			//加锁，保护任务队列
			std::unique_lock<std::mutex> lock(queue_mutex);
			//将任务添加到任务队列
			tasks.emplace(std::forward<F>(f));
		}
		//通知一个等待的线程
		condition.notify_one();
	}
	
	~ThreadPool();
private:
	std::vector<std::thread> workers;//工作线程
	std::queue<std::function<void()>> tasks;//任务队列
	std::mutex queue_mutex;//保护任务队列的互斥锁
	std::condition_variable condition;//条件变量，用于线程间通信

	std::atomic <bool> stop;//线程是否停止
};

#endif // !THREADPOOL_H
