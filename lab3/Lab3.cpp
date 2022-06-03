#include <iostream>
#include "windows.h"
#include "stdlib.h"
#include "Vector_data.h"

HANDLE event;
CRITICAL_SECTION critical_section1, critical_section2;



void BubbleSorting(Vector_data* arr)
{
    //критическая секция №1; сортировка k элементов
    EnterCriticalSection(&critical_section1);
    char temp[30];
    std::cout << "Enter delay between elements sorting(entering non-positive integer leads to 0-delay): ";
    std::cin.getline(temp, 30);
    int delay = std::atoi(temp);
    if (delay < 0) 
        delay = 0;
    for (size_t i = 0; i < arr->k_ - 1; ++i)
    {
        for (size_t j = 0; j < arr->k_ - i - 1; ++j)
            if (arr->array_[j] - arr->array_[j + 1] >= arr->accuracy_)
                std::swap(arr->array_[j], arr->array_[j + 1]);
        Sleep(delay);
    }
    LeaveCriticalSection(&critical_section1);
}

void SumCounting(Vector_data* arr)
{
    //критическая секция #2; суммирование k элементов
    EnterCriticalSection(&critical_section2);

    //ожидание соытия от потока main
    WaitForSingleObject(event, INFINITE);
    for (size_t i = 0; i < arr->k_; ++i)
        arr->sum_ += arr->array_[i];
    /*std::cout << "\nSum of first " << arr->k_
        << " elements: " << arr->sum_ << std::endl;*/
    LeaveCriticalSection(&critical_section2);
}

int main()
{
    //Инициализация события и критических секций
    event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (event == NULL)
        return GetLastError();
    InitializeCriticalSection(&critical_section1);
    InitializeCriticalSection(&critical_section2);

    HANDLE work, SumElement;
    DWORD ID_work, ID_SumElement;

    //создание и инициализация массива с использованием консольного ввода
    char temp[30];
    int size;
    std::cout << "Enter array size: ";
    std::cin.getline(temp, 30);
    size = std::atoi(temp);
    if (size <= 0)
    {
        std::cerr << "Size should be positive integer\n";
        return -1;
    }
    Vector_data vec(new double[size], size, 0.01);
    for (size_t i = 0; i < size; ++i)
    {
        std::cout << "\nEnter " << i + 1 << " element: ";
        std::cin.getline(temp, 30);
        vec.array_[i] = std::strtod(temp, NULL);
    }

    //вывод размерности и элементов массива на консоль
    std::cout << "Size: " << size << std::endl;
    std::cout << "Array: ";
    for (size_t i = 0; i < size; ++i)
        std::cout << vec.array_[i] << ' ';
    std::cout << std::endl;

    //запуск потока work в подвешенном состоянии
    work = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(BubbleSorting), &vec, CREATE_SUSPENDED, &ID_work);
    if (work == NULL)
    {
        DeleteCriticalSection(&critical_section1);
        DeleteCriticalSection(&critical_section2);

        CloseHandle(event);
        return GetLastError();
    }

    //ввод числа k
    std::cout << "Enter k(wrong data leads to array size): ";
    std::cin.getline(temp, 30);
    vec.k_ = std::atoi(temp);
    if (vec.k_ <= 0 || vec.k_ > vec.size_)
        vec.k_ = vec.size_;
     
    //запуск потока work
    ResumeThread(work);

    //запуск потока SumElement
    SumElement = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(SumCounting), &vec, 0, &ID_SumElement); if (work == NULL)
        if (SumElement == NULL)
        {
            DeleteCriticalSection(&critical_section1);
            DeleteCriticalSection(&critical_section2);

            CloseHandle(event);
            CloseHandle(work);
            return GetLastError();
        }
   

    //приостановка main на 50 мс
    Sleep(50);

    //критическая секция №1; получение результата от work; вывод в консоль k отсортированных элементов 
    EnterCriticalSection(&critical_section1);
    std::cout << "First " << vec.k_ << " elements: ";
    for (size_t i = 0; i < vec.k_; ++i)
        std::cout << vec.array_[i] << ' ';
    std::cout << std::endl;
    LeaveCriticalSection(&critical_section1);

    //сигнал для SumElement для начала суммирования
    SetEvent(event);


    //критическая секция№2; получение результата от SumElement; вывод в консоль суммы первых k элементов
    EnterCriticalSection(&critical_section2);
    std::cout << "\nSum of first " << vec.k_
        << " elements: " << vec.sum_ << std::endl;
    LeaveCriticalSection(&critical_section2);
    
    //вывод оставшихся элементов массива
    std::cout << "Remaining elements of array: ";
    for (size_t i = vec.k_; i < vec.size_; ++i)
        std::cout << vec.array_[i] << ' ';
    std::cout << std::endl;


    //удаление критических секций
    DeleteCriticalSection(&critical_section1);
    DeleteCriticalSection(&critical_section2);

    //запрос на закрытие дескрипторов
    CloseHandle(event);
    CloseHandle(work);
    CloseHandle(SumElement);
}