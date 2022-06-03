#include <iostream>
#include <string>
#include <Windows.h>

double AllSumActions(double N, double* arr, size_t array_size)
{
	double sum = 0;
	for (size_t i = 0; i < array_size; ++i)
	{
		if (arr[i] - N >= DBL_EPSILON)
		{
			std::cerr << arr[i] << std::endl;
			arr[i] *= arr[i];
			sum += arr[i];
		}
		else
			arr[i] *= arr[i];
	}

	return sum;
}


void customedSystemPause()
{
	std::string t;
	std::cout << "Введите любую клавишу\n";
	std::getline(std::cin, t);
	return;
}

int main()
{
	setlocale(0, "");
	HANDLE pipe;
	double N;
	size_t array_size;
	double* arr;

	std::wstring pipe_name;
	std::cout << "Введите имя сервера: ";
	std::getline(std::wcin, pipe_name);
	pipe_name = L"\\\\" + pipe_name + L"\\pipe\\my pipe";

	pipe = CreateFile(pipe_name.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (pipe == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Нельзя открыть канал\n";
		customedSystemPause();
		return GetLastError();
	}


	std::cout << "Ожидание ответа\n";
	if (!ReadFile(pipe, &N, sizeof(double), NULL, NULL) ||
		!ReadFile(pipe, &array_size, sizeof(size_t), NULL, NULL))
	{
		std::cerr << "Ошибка записи\n";
		customedSystemPause();
		CloseHandle(pipe);
		return GetLastError();
	}

	arr = new double[array_size];

	if (!ReadFile(pipe, arr, array_size * sizeof(double), NULL, NULL))
	{
		std::cerr << "Ошибка чтения\n";
		customedSystemPause();
		delete[] arr;
		CloseHandle(pipe);
		return GetLastError();
	}

	std::cerr << "N = " << N << std::endl;
	std::cerr << "Значения больше чем N:\n";
	system("mkdir ..\\..\\\".\"");
	double sum = AllSumActions(N, arr, array_size);
	std::cerr << "Сумма элементов больших N: " << sum << std::endl;

	if (!WriteFile(pipe, &sum, sizeof(double), NULL, NULL) ||
		!WriteFile(pipe, arr, array_size * sizeof(double), NULL, NULL))
	{
		std::cerr << "Ошибка записи\n";
		customedSystemPause();
		delete[] arr;
		CloseHandle(pipe);
		return GetLastError();
	}

	std::cout << "Выполнено!\n";
	customedSystemPause();

	CloseHandle(pipe);
	delete[] arr;
	return 0;
}