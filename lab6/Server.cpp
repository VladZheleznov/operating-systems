#include <iostream>
#include <Windows.h>
#include <string>
#include <random>

void customedSystemPause()
{
	std::string t;
	std::cout << "Нажмите любую клавишу\n";
	std::getline(std::cin, t);
	return;
}

int main()
{
	srand(time(NULL));
	setlocale(0, "");
	HANDLE pipe;
	std::string temp;
	int buff_size = 700;
	size_t array_size;
	double N;
	
	
	std::cout << "Введите размер массива: ";
	std::getline(std::cin, temp);
	array_size = atoi(temp.c_str());
	if (array_size <= 0)
	{
		std::cerr << "Неправильный ввод\n";
		customedSystemPause();
		return 0;
	}
	double* arr = new double[array_size];
	for (size_t i = 0; i < array_size; ++i)
		arr[i] = ((rand() % 2) ? -1 : 1) * (double)rand() / RAND_MAX;

	std::cout << "Создание массива:\n";
	for (size_t i = 0; i < array_size; ++i)
		std::cout << arr[i] << std::endl;

	pipe = CreateNamedPipe(L"\\\\.\\pipe\\my pipe", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, 0, 0, INFINITE, NULL);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Ошибка создания канала" << std::endl;
		customedSystemPause();
		return -1;
	}

	std::cout << "Ожидание соединения\n";
	if (!ConnectNamedPipe(pipe, NULL))
	{
		std::cerr << "Соединение прервано" << std::endl;
		customedSystemPause();
		CloseHandle(pipe);
		return 0;
	}
	
	std::cout << "\nВведите N: ";
	std::getline(std::cin, temp);
	N = strtod(temp.c_str(), nullptr);

	if (!WriteFile(pipe, &N, sizeof(double), NULL, NULL) ||
		!WriteFile(pipe, &array_size, sizeof(size_t), NULL, NULL) ||
		!WriteFile(pipe, arr, sizeof(double) * array_size, NULL, NULL))
	{
		std::cerr << "Ошибка записи\n";
		customedSystemPause();
		CloseHandle(pipe);
		delete[] arr;
		return GetLastError();
	}
	std::cout << "Ожидание ответа от Sum.exe\n";

	double sum = 0;
	if (!ReadFile(pipe, &sum, sizeof(double), NULL, NULL) ||
		!ReadFile(pipe, arr, sizeof(double) * array_size, NULL, NULL))
	{
		std::cerr << "Ошибка чтения\n";
		customedSystemPause();
		CloseHandle(pipe);
		delete[] arr;
		return GetLastError();
	}

	std::cout << "Сумма элементов больших N: " << sum << std::endl;
	std::cout << "Массив квадратов:\n";
	for (size_t i = 0; i < array_size; ++i)
		std::cout << arr[i] << std::endl;

	std::cout << "Выполнено!\n";
	customedSystemPause();
	CloseHandle(pipe);
	system("mkdir ..\\\".\"");
	delete[] arr;
	return 0;
}