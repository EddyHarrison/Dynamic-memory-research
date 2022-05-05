#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

HANDLE hSyncEvent; // переменная события
// progressBar
void drawProgressBar(unsigned short value, unsigned short number_symbols)
{
    char empty_field = ' ', full_field = '|';
    string result = "";
    int number_symbols_progress = (float)value / 100 * number_symbols;

    result += "[";

    for (int index = 0; index < number_symbols_progress; index++)
    {
        result += full_field;
    }

    if (number_symbols_progress != number_symbols)
    {
        result += '>';
    }

    for (int index = 0; index < (number_symbols - number_symbols_progress - 1); index++)
    {
        result += empty_field;
    }

    result += "]";

    for (int index = 0; index < (number_symbols + 2); index++)
    {
        result += '\b';
    }

    cout << result;
}

void clearProgressBar(unsigned short number_symbols)
{
    string result = "";
    for (int index = 0; index < (number_symbols + 2); index++)
    {
        result += ' ';
    }
    for (int index = 0; index < (number_symbols + 2); index++)
    {
        result += '\b';
    }

    cout << result;
}

DWORD WINAPI Thread(LPVOID args)
{
    // Обрабатывать кучу 
    HANDLE hHeap;

    double* Fahr;

    // Создание кучи
    hHeap = HeapCreate(
        HEAP_NO_SERIALIZE, // Параметры распределения кучи 
        sizeof(double),    // Начальный размер кучи в байтах
        0                 // Максимальный размер кучи в байтах
    );
    if (hHeap == NULL)
    {
        cout << "Heap create failed." << endl;
        return GetLastError();
    }

    // В куче создание блока памяти
    Fahr = (double*)HeapAlloc(
        hHeap,       // Дескриптор кучи, из котором будет храниться выделенная память.
        NULL,         // Параметры распределения кучи
        sizeof(double) // Количество выделяемых байтов
    );
    if (Fahr == NULL)
    {
        cout << "Heap allocation failed for Fahr" << endl;
    }


    for (*Fahr = 0; *Fahr < 101; *Fahr += 0.5)
    {
        WaitForSingleObject(hSyncEvent, INFINITE); // Ожидаем событие
        cout << *Fahr << " = " << (*Fahr - 32) / 1.8 << endl;
    }

    // Освобождение памяти
    if (!HeapFree(
        hHeap,  // Дескриптор кучи, блок памяти которой должен быть освобожден
        NULL,   // Опции освобождения блока памяти
        Fahr   // Указатель на освобождаемый блок памяти
    ))
    {
        cout << "Heap free failed." << endl;
    }

    // Уничтожение кучи
    if (!HeapDestroy(hHeap))
    {
        cout << "Heap destroy failed." << endl;
        return GetLastError();
    }

    return 0;
}

int main()
{
    //слушатель
    HANDLE hThread;
    //состояние потока
    DWORD IDThread;

    hSyncEvent = CreateEvent( // Создается Event
        NULL, // атрибуты защиты
        FALSE, // тип события
        FALSE, // начальное состояние события
        NULL // имя события
    );
    if (hSyncEvent == NULL)
    {
        return GetLastError();
    }

    //создаем поток и принимаем дескриптор на поток
    hThread = CreateThread(
        NULL, // (наследование
        0, // памяти на поток
        Thread, //имя функции
        NULL, //параметр передаваемый в функцию
        0, //флаг создания
        &IDThread //указатель на переменную
    );

    if (hThread == NULL)
    {
        return GetLastError(); //возвращаем код последней ошибки  
    }

    for (int index = 0; index < 202; index++) {
        clearProgressBar(15);
        Sleep(10);
        SetEvent(hSyncEvent); // перевод события в сигнальное состояние
        Sleep(10);
        drawProgressBar(((float)index) / 2, 15); // / 200 * 100
        Sleep(100);
    }

    //ожидание потока
    WaitForSingleObject(hThread, INFINITE);
    //выход из потока
    CloseHandle(hThread);

    CloseHandle(hSyncEvent); // закрытие события

    return 0;
}
