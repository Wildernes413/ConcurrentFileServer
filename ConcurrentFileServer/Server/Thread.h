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
	//�ύ�����̳߳�
	//�ύ�����̳߳�
	template <class F>
	void enqueue(F&& f)
	{
		{
			//�����������������
			std::unique_lock<std::mutex> lock(queue_mutex);
			//��������ӵ��������
			tasks.emplace(std::forward<F>(f));
		}
		//֪ͨһ���ȴ����߳�
		condition.notify_one();
	}
	
	~ThreadPool();
private:
	std::vector<std::thread> workers;//�����߳�
	std::queue<std::function<void()>> tasks;//�������
	std::mutex queue_mutex;//����������еĻ�����
	std::condition_variable condition;//���������������̼߳�ͨ��

	std::atomic <bool> stop;//�߳��Ƿ�ֹͣ
};

#endif // !THREADPOOL_H
