//Threads
#include<iostream>
#include<thread>

using namespace std;
using namespace std::chrono_literals; //для того, чтобы регулировать задержку при работе потоков

bool finish = false;

void Hello()
{
	while (!finish)
	{
		cout << "Hello";
		std::this_thread::sleep_for(100ms);
	}

}

void World()
{
	while (!finish)
	{
		cout << "World";
		std::this_thread::sleep_for(100ms);
	}
}
int main()
{
	//Инициализация потоков при объявлении:
	/*std::thread hello_thread(Hello); 
	std::thread world_thread (World);*/

	//Объявление потоков:
	std::thread hello_thread;
	std::thread world_thread;
	//Иниициализация потоков после объявления:
	hello_thread = std::thread(Hello);
	world_thread = std::thread(World);

	cin.get(); //Ожидает нажатие Enter
	finish = true;
	//Чтобы программа не падала, потоки нужно синхронизировать:
	hello_thread.join(); //синхронизирует потоки
	world_thread.join();
	//Теперь программа не падает, но потоки все еще беспорядочно хватают ресурсы процессора и может вывестись 20,30 hello подряд
	//и лишь 5,10 world после и наоборот. Но не HelloWorld, как было задумано.
	//Чтобы потоки равномерно использовали ресурс процессора нужно внести задержку в их исполнение.
	return 0;
}