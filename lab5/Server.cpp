#include <iostream>
#include <Windows.h>
#include <string>
#include <random>

int main()
{
	srand(time(NULL));
	setlocale(0, "");
	std::string temp;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	HANDLE hWpipe, hRpipe, 
		event = CreateEvent(NULL, FALSE, FALSE, L"event");
	int buff_size = 700;


	si.dwFlags = STARTF_USESTDHANDLES;
	si.cb = sizeof(si);
	si.lpTitle = const_cast <wchar_t*>(L"Sum");
	sa.bInheritHandle = TRUE;

	size_t array_size;
	double N;

	std::cout << "������� ������ �������: ";
	std::getline(std::cin, temp);
	array_size = atoi(temp.c_str());
	if (array_size <= 0)
	{
		std::cerr << "������������ ����\n";
		return 0;
	}

	double* arr = new double[array_size];
	for (size_t i =0; i < array_size; ++i)
		arr[i] = ((rand() % 2) ? -1 : 1 ) *  (double)rand() / RAND_MAX;

	std::cout << "�������� �������:\n";
	for (size_t i = 0; i < array_size; ++i)
		std::cout << arr[i] << std::endl;

	if (!CreatePipe(&hRpipe, &hWpipe, &sa, buff_size))
	{
		std::cerr << "������ �������� ���������� ������\n";
		return GetLastError();
	}

	si.hStdInput = hRpipe;
	si.hStdOutput = hWpipe;
	std::wstring cmd_str = L"Sum.exe";
	if (!CreateProcess(NULL, const_cast<wchar_t*>(cmd_str.c_str()), NULL, &sa, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		std::cerr << "������ �������� �������� Sum.exe\n";
		CloseHandle(hWpipe);
		CloseHandle(hRpipe);
		CloseHandle(event);
		delete [] arr;
		return GetLastError();
	}

	std::cout << "\n������� N: ";
	std::getline(std::cin, temp);
	N = strtod(temp.c_str(), nullptr);

	if (!WriteFile(hWpipe, &N, sizeof(double), NULL, NULL) ||
		!WriteFile(hWpipe, &array_size, sizeof(size_t), NULL, NULL) ||
		!WriteFile(hWpipe, arr, sizeof(double) * array_size, NULL, NULL))
	{
		std::cerr << "������ ������ �� ���������� ������\n";
		CloseHandle(hWpipe);
		CloseHandle(hRpipe);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(event);
		delete[] arr;
		return GetLastError();
	}
	std::cout << "�������� ������ �� Sum.exe\n";

	WaitForSingleObject(event, INFINITE);
	double sum = 0;
	if (!ReadFile(hRpipe, &sum, sizeof(double), NULL, NULL) ||
		!ReadFile(hRpipe, arr, sizeof(double) * array_size, NULL, NULL))
	{
		std::cerr << "������ ������ �� ���������� ������\n";
		CloseHandle(hWpipe);
		CloseHandle(hRpipe);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(event);
		delete[] arr;
		return GetLastError();
	}

	std::cout << "����� ��������� ������� N: " << sum << std::endl;
	std::cout << "������ ���������:\n";
	for (size_t i = 0; i < array_size; ++i)
		std::cout << arr[i] << std::endl;

	CloseHandle(hWpipe);
	CloseHandle(hRpipe);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(event);
	delete[] arr;

	return 0;
}