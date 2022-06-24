#include <iostream>
#include <thread>
#include <chrono>
#include<ctime>

void worker(int** mass_and_size)
{
	int temp = 0;
	int** mass_and_size_1 = (int**)mass_and_size;
	for (int i = 0; i < mass_and_size_1[0][0]; i++)
	{
		temp += mass_and_size_1[1][i];
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	std::cout << "Сумма =  " << temp << std::endl;
	return;
}


int main()
{
	setlocale(LC_ALL, "ru");
	
	int size;
	std::cout << "Введите размер массива: " << std::endl;
	std::cin >> size;

	int* mass;
	mass = new int[int(size)];
	srand(time(0));
	std::cout << "Созданный массив: " << std::endl;
	for (int i = 0; i<int(size); i++)
	{
		mass[i] = 1 + rand() % 10;
		std::cout << mass[i] << std::endl;
	}
	int** mass_and_size = new int* [2];
	mass_and_size[0] = new int[1];
	mass_and_size[0][0] = int(size);
	mass_and_size[1] = new int[mass_and_size[0][0]];
	for (int i = 0; i < mass_and_size[0][0]; i++)
	{
		mass_and_size[1][i] = mass[i];
	}

	std::thread tWorker(worker, mass_and_size);
	tWorker.join();

	delete[] mass;

	return 0;
}
