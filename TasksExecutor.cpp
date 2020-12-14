#include "TasksExecutor.h"
#include <thread>

//исполнитель заданий
TasksExecutor::TasksExecutor(TaskQueue queue)
{
	_queue = queue;
}

void threadFunction(TaskQueue queue, int count)
{
	std::vector<std::thread> arr;
	while (count) {
		//очередь заданий
		TTask task = queue.popTask();
		if (task != NULL) {
			//добавление в вектор нового потока
			std::thread thr(task);
			arr.push_back(move(thr));
			count--;
		}
	}

	//ожидание выполнения всех потоков
	for (int i = 0; i < arr.size(); i++) {
		arr[i].join();
	}
}

void TasksExecutor::startExecution(int maxThreadsCount) {
	//создание потоков
	std::thread thr(threadFunction, _queue, maxThreadsCount);
	thr.join();
}




