//Threads
#include<iostream>
#include<thread>

using namespace std;
using namespace std::chrono_literals; //��� ����, ����� ������������ �������� ��� ������ �������

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
	//������������� ������� ��� ����������:
	/*std::thread hello_thread(Hello); 
	std::thread world_thread (World);*/

	//���������� �������:
	std::thread hello_thread;
	std::thread world_thread;
	//�������������� ������� ����� ����������:
	hello_thread = std::thread(Hello);
	world_thread = std::thread(World);

	cin.get(); //������� ������� Enter
	finish = true;
	//����� ��������� �� ������, ������ ����� ����������������:
	hello_thread.join(); //�������������� ������
	world_thread.join();
	//������ ��������� �� ������, �� ������ ��� ��� ������������ ������� ������� ���������� � ����� ��������� 20,30 hello ������
	//� ���� 5,10 world ����� � ��������. �� �� HelloWorld, ��� ���� ��������.
	//����� ������ ���������� ������������ ������ ���������� ����� ������ �������� � �� ����������.
	return 0;
}