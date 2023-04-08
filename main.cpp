#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
#include <mutex>
#include <windows.h>
using namespace std;

mutex mtx;
const int QUEUE_SIZE = 10;
queue<int> requests;

int capacity;
int num_atms;

bool is_generator_active = true;

bool is_queue_full()
{
    return requests.size() >= QUEUE_SIZE;
}

bool is_queue_empty()
{
    return requests.size() == 0;
}

void generator()
{
    while (is_generator_active)
    {
        if (!is_queue_full())
        {
            int value = rand() % 1000;
            mtx.lock();
            requests.push(value);
            cout << "Генератор: запрос " << value << " добавлен в очередь. Размер очереди: " << requests.size() << endl;
            mtx.unlock();
        }

        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

void atm(int id)
{
    while (true)
    {
        int request = 0;
        mtx.lock();
        if (!is_queue_empty())//Получаем запрос из очереди если очередь не пуста
        {
            request = requests.front();
            requests.pop();
            cout << "Банкомат " << id << " : выполняет запрос " << request << endl;
        }
        mtx.unlock();

        if (request > 0)
        {
            int processing_time = rand() % 1000 + 1000;
            this_thread::sleep_for(chrono::milliseconds(processing_time));
            cout << "Банкомат " << id << ": запрос " << request << " выполнен за " << processing_time << " ms." << endl;
        }

        int sleep_time = rand() % 500 + 500;
        this_thread::sleep_for(chrono::milliseconds(sleep_time)); //моделировуем случайное время ожидания клиента перед тем, как он попадёт в очередь
    }
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    cout << "Введите вместимость очереди банкомата: ";
    cin >> capacity;

    cout << "Введите количество банкоматов: ";
    cin >> num_atms;

    thread generator_thread(generator);
    thread* atm_threads = new thread[num_atms];

    for (int i = 0; i < num_atms; i++)
    {
        atm_threads[i] = thread(atm, i);
    }

    generator_thread.join();//Ожидание завершения генератора

    for (int i = 0; i < num_atms; i++)
    {
        atm_threads[i].join();//ожидания завершения каждого банкомата
    }

    delete[] atm_threads;
    return 0;
}