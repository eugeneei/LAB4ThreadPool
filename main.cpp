#include <stdio.h>
#include "TaskQueue.h"
#include "TasksExecutor.h"
#include <thread>
#include <string>
#include <iostream>
#include <fstream>
#include <functional>

#define THREADS_COUNT 10
#define FILE_PATH "C:\\Users\\Angelina\\Desktop\\test.txt"

#define TYPE std::function<void(void)>

std::vector<std::string>* vecOfStr = new std::vector<std::string>();
std::vector < std::vector<std::string>*>* vectorOfParts = new std::vector < std::vector<std::string>*>();


bool getFileContent(std::string fileName, std::vector<std::string> *vecOfStrs)
{
	//класс входного потока для работы с файлами
	std::ifstream dict_file(FILE_PATH);
	std::string line;

	//возможно ли получить доступ к файлу
	if (!dict_file.good()) {
		return false;
	}

	//до конца файла
	while (std::getline(dict_file, line))
	{
		std::string new_line;
		new_line = line + "\n";
	
		//добавляем в вектор строк
		if (new_line.size() != 0)
			vecOfStrs->push_back(new_line);
	}

	return true;
}


void addVector(std::vector<std::string>* vect, TaskQueue queue) {

	queue.addTask([vect]() {
		//задача для очереди потоков
		std::vector<std::string>* copy = vect;
		std::sort(vect->begin(), vect->end());
	});
}

#define Vector std::vector<std::string>
Vector mergeTwo(Vector A, Vector B)
{
	//получает размер векторов 
	int m = A.size();
	int n = B.size();

	//вектор для хранения результата сортировки
	Vector D;
	//запрашивает, чтобы емкость вектора была как минимум достаточной для размещения m+n элементов.
	D.reserve(m + n);

	int i = 0, j = 0;
	while (i < m && j < n) {
		//определение порядка следования частей
		if (A[i] <= B[j])
			D.push_back(A[i++]);
		else
			D.push_back(B[j++]);
	}
	while (i < m)
		D.push_back(A[i++]);
	while (j < n)
		D.push_back(B[j++]);
	return D;
}

Vector mergeVectors() {
	Vector tmpVector;
	if (vectorOfParts->size() > 0) {
		tmpVector = *(*vectorOfParts)[0];
	}
	//на протяжении количества частей сортируем два подряд идущих вектора
	for (int i = 1; i < vectorOfParts->size(); i++) {
		tmpVector = mergeTwo(tmpVector, *(*vectorOfParts)[i]);
	}
	return tmpVector;
}

void outPutVector(Vector vector) {
	for (int i = 0; i < vector.size(); i++) {
		printf("   %s", vector[i].c_str());
	}
}

void splitAndSortVectors(TaskQueue taskQueue, int threadsCount) {
	//определяем размер части для сортировки
	int onePartCount = floor((((double)vecOfStr->size()) / threadsCount) + .5);

	//выполняется для каждой части
	for (int i = 0; i < vecOfStr->size(); i += onePartCount) {
		std::vector<std::string>* newVector = new std::vector<std::string>;
		vectorOfParts->push_back(newVector);
		for (int j = i; j < i + onePartCount; j++) {
			if (j < vecOfStr->size()) {
				std::string str = (*vecOfStr)[j];
				//вектор части файла из строк для сортировки
				newVector->push_back(str);
			}
		}
		//добавление задания
		addVector(newVector, taskQueue);
	}
}

int main() {
	//в вектор помещаем содержимое файла
	bool result = getFileContent(FILE_PATH, vecOfStr);
	if (!result) {
		//если не удалось файл считать
		printf("File isn't exist");
		return -1;
	}

	//объявление пула потоков
	TaskQueue taskQueue;
	TasksExecutor taskExecutor(taskQueue);
	//задание количества потоков
	int threadsCount = THREADS_COUNT > vecOfStr->size() ? vecOfStr->size() : THREADS_COUNT;

	//дробление текста на части
	splitAndSortVectors(taskQueue, threadsCount);
	//выполнение заданий
	taskExecutor.startExecution(threadsCount);

	//соединение полученных векторов
	Vector vector = mergeVectors();
	//вывод векторов в файл
	outPutVector(vector);
}
