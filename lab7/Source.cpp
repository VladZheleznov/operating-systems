#include <windows.h>
#include <iostream>
#include <random>
#include <time.h>

double getRandomProduct() {
    return 1.5 + rand() % 10;
}

class MonitorStack {
private:
    int nSize;
    int count;
    double* arr;
public:

    MonitorStack(int nSize) {
        this->nSize = nSize;
        this->arr = new double[nSize];
        count = 0;
    }
    MonitorStack() {
        this->nSize = 0;
        this->arr = nullptr;
        count = 0;
    }
    ~MonitorStack() {
        delete[] arr;
    }

    void addCount();
    void minusCount();
    void Push(double& nElement);
    double Pop();
};

void MonitorStack::addCount() {
    count++;
}
void MonitorStack::minusCount() {
    count--;
}

void MonitorStack::Push(double& nElement) {
    arr[count] = nElement;
    addCount();
}

double MonitorStack::Pop() {
    minusCount();
    return arr[count];
}

struct tagParam : public MonitorStack {
public:
    int count;
    int iterator;
    double* products;
    HANDLE hAccess;
    HANDLE hFull;
    HANDLE hEmpty;

    tagParam(double* products, int size, HANDLE hAccess, HANDLE hFull, HANDLE hEmpty) : MonitorStack(size)
    {
        count = 0;
        iterator = -1;
        this->products = products;
        this->hAccess = hAccess;
        this->hFull = hFull;
        this->hEmpty = hEmpty;
    }

    ~tagParam() {
    }

    double getProduct() {
        iterator++;
        return products[iterator];
    }
};

void Push(LPVOID lpParam) {
    setlocale(LC_ALL, "Rus");
    tagParam* param = (tagParam*)lpParam;
    double element = param->getProduct();

    for (int i = 0; i < param->count; i++) {

        WaitForSingleObject(param->hFull, INFINITE);
        Sleep(1000);
        WaitForSingleObject(param->hAccess, INFINITE);
        std::cout << "\n���������� ������� " << element;
        param->Push(element);
        ReleaseSemaphore(param->hAccess, 1, NULL);
        ReleaseSemaphore(param->hEmpty, 1, NULL);
    }
}

void Pop(LPVOID lpParam) {
    setlocale(LC_ALL, "Rus");
    tagParam* param = (tagParam*)lpParam;


    WaitForSingleObject(param->hEmpty, INFINITE);
    for (int i = 0; i < param->count; i++) {
        //Sleep(1000);
        WaitForSingleObject(param->hAccess, INFINITE);
        std::cout << "\n���������� ������� : " << param->Pop();
        ReleaseSemaphore(param->hAccess, 1, NULL);
        ReleaseSemaphore(param->hFull, 1, NULL);
    }
}


int main()
{
    srand(time(0));
    int sizeStack, countProducer, countConsumer;
    double* products = nullptr;

    std::cout << "input size Stack: ";
    std::cin >> sizeStack;
    std::cout << "input size countProducer: ";
    std::cin >> countProducer;
    std::cout << "input size countConsumer: ";
    std::cin >> countConsumer;

    products = new double[countProducer];
    for (int i = 0; i < countProducer; i++) {
        products[i] = getRandomProduct();
    }


    HANDLE* hProducer = new HANDLE[countProducer];
    HANDLE* hConsumer = new HANDLE[countConsumer];
    DWORD IDProducer, IDConsumer;
    HANDLE hAccess = CreateSemaphore(NULL, 1, 1, L"S1");
    HANDLE hFull = CreateSemaphore(NULL, sizeStack, sizeStack, L"S2");
   // HANDLE hFull = CreateSemaphore(NULL, 1, 1, L"S2");
    HANDLE hEmpty = CreateSemaphore(NULL, 0, 1, L"S3");

    tagParam param(products, sizeStack, hAccess, hFull, hEmpty);

    for (int i = 0; i < countProducer; i++) {
        param.count = 1;

        hProducer[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Push, &param, 0, &IDProducer);
        if (hProducer[i] == NULL) {
            return GetLastError();
        }
    }

    for (int i = 0; i < countConsumer; i++) {
        param.count = 1;

        hConsumer[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Pop, &param, 0, &IDConsumer);
        if (hConsumer[i] == NULL) {
            return GetLastError();
        }
    }

    for (int i = 0; i < countProducer; i++) {
        WaitForSingleObject(hProducer[i], INFINITE);
    }
    for (int i = 0; i < countConsumer; i++) {
        WaitForSingleObject(hConsumer[i], INFINITE);
    }

    CloseHandle(hAccess);
    CloseHandle(hFull);
    CloseHandle(hEmpty);

    for (int i = 0; i < countProducer; i++) {
        CloseHandle(hProducer[i]);
    }
    for (int i = 0; i < countConsumer; i++) {
        CloseHandle(hConsumer[i]);
    }

    delete[] products;
    return 0;
}
