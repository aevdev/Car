#include <iostream>
#include <thread>
#include <conio.h>
#include <Windows.h>

using namespace std;
using namespace std::chrono_literals;

#define Escape 27
#define Enter 13

#define MIN_TANK_VOLUME 20
#define MAX_TANK_VOLUME 80

#define MIN_ENGINE_CONSUMPTION 3
#define MAX_ENGINE_CONSUMPTION 25
#define DEFAULT_ENGINE_CONSUMPTION 10

#define MAX_SPEED_LOW_LIMIT 150
#define MAX_SPEED_HIGH_LIMIT 300
#define DEFAULT_MAX_SPEED 250
class Tank
{
	const size_t VOLUME; //Характеристика бака - это его объём.
	double fuel_level;
public:
	size_t get_VOLUME()const
	{
		return this->VOLUME;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	void fill(double fuel_level)
	{
		if (fuel_level <= 0) return;
		if (this->fuel_level + fuel_level <= VOLUME)
			this->fuel_level += fuel_level;
		else this->fuel_level = VOLUME;
	}
	double give_fuel(double amount)
	{
		if (fuel_level - amount > 0) fuel_level -= amount;
		else fuel_level = 0;
		return fuel_level;
	}
	Tank(const size_t volume):VOLUME(volume >= MIN_TANK_VOLUME && volume <= MAX_TANK_VOLUME ? volume:60)
	{
		//Константный член класса можно проиниициализировать только в списке инициализации конструктора.
		//(только в заголовке)
		//if (volume >= MIN_TANK_VOLUME && volume <= MAX_TANK_VOLUME) this->VOLUME = volume;
		//else this->VOLUME = 60;
		this->fuel_level = 0;
		cout << "Tank is ready" << endl;
	}
	~Tank()
	{
		cout << "Tank is over" << endl;
	}
	void info()const
	{
		cout << "Tank volume:\t" << VOLUME << " liters.\n";
		cout << "Fuel level: " << fuel_level << " liters.\n";
	}
};

class Engine
{
	double consumption; //расход на 100км
	double consumption_per_second; //расход за одну секунду
	double default_consumption_per_second; //расход за одну секунду
	bool is_started; //двигатель заведен или заглушен
public:
	double get_consumption()const
	{
		return consumption;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	void set_consumption(double consumption)
	{
		if (consumption >= MIN_ENGINE_CONSUMPTION && consumption <= MAX_ENGINE_CONSUMPTION)
			this->consumption = consumption;
		else
			this->consumption = DEFAULT_ENGINE_CONSUMPTION;
		this->consumption_per_second = this->consumption * 3e-5;
		this->default_consumption_per_second = this->consumption * 3e-5;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}
	void correct_consumption(int speed)
	{
		if (speed >= 1 && speed <= 60 || speed >= 101 && speed <= 140)
			consumption_per_second = .002;
		if (speed >= 61 && speed <= 100) consumption_per_second = .0014;
		if (speed >= 141 && speed <= 200) consumption_per_second = .0025;
		if (speed >= 201 && speed <= 250) consumption_per_second = .003;
		if (speed == 0) consumption_per_second = .0003;
	}
	void info()const
	{
		cout << "Consumption per 100 km: " << consumption << " liters.\n";
		cout << "Consumption per second: " << consumption_per_second << " liters.\n";
	}
	Engine(double consumption)
	{
		set_consumption(consumption);
		is_started = false;
		cout << "Enfine is ready.";
	}
	~Engine()
	{
		cout << "Engine is over" << endl;
	}
};

class Car
{
	Engine engine;
	Tank tank;
	bool driver_inside;
	int speed;				//скорость
	const int MAX_SPEED;	//максимальная скорость
	int acceleration;		//ускорение
	struct Control
	{
		std::thread panel_thread; //отображение панели приборов
		std::thread engine_idle_thread; //холостой ход двигателя
		std::thread free_wheeling_thread; 
	}control;

public:
	Car(double engine_consumption, size_t tank_volume, int max_speed = DEFAULT_MAX_SPEED, int acceleration = 12
	)
		:engine(engine_consumption), tank(tank_volume),
		MAX_SPEED(max_speed >= MAX_SPEED_LOW_LIMIT  && max_speed <= MAX_SPEED_HIGH_LIMIT ? max_speed : DEFAULT_MAX_SPEED)
	{
		driver_inside = false;
		speed = 0;
		this->acceleration = acceleration >= 5 && acceleration <= 25 ? acceleration : 12;
		cout << "Car is ready." << endl;
	}
	~Car()
	{
		cout << "Car is over." << endl;
	}
	void get_in()
	{
		driver_inside = true;
		//panel();
		control.panel_thread = std::thread(&Car::panel, this); //для того, чтобы вызвать из метода класса для потока метод, нужно сначала указать из какого класса мы его вызываем и (через запятую) для какого объекта.
	}
	void get_out()
	{
		if (speed > 0)
		{
			cout << "You can't leave your car on the go." << endl;
			return;
		}
		driver_inside = false;
		if(control.panel_thread.joinable())control.panel_thread.join(); //освобождаем поток
		system("CLS"); //очистка  командной строки
		cout << "You are out of your car." << endl;
	}
	void start_engine()
	{
		if (driver_inside && tank.get_fuel_level())
		{
			engine.start();
			control.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
	}
	void stop_engine()
	{
		engine.stop();
		if (control.engine_idle_thread.joinable())control.engine_idle_thread.join();
	}
	void accelerate()
	{
		if (!control.free_wheeling_thread.joinable())
			control.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
		if (driver_inside && engine.started() && speed < MAX_SPEED)
		{
			speed += acceleration;
			std::this_thread::sleep_for(1s);
		}
	}
	void slow_down()
	{
		if (driver_inside && speed > 0)
		{
			speed -= acceleration;
			if (speed < 0) speed = 0;
			std::this_thread::sleep_for(1s);
		}
	}
	void control_car()
	{
		cout << "Press enter to get in" << endl;
		char key;
		do
		{
			key = _getch();
			switch (key)
			{
			case Enter:
				if (driver_inside)get_out();
				else get_in();
				break; //Вход/Выход из машины
			case 'I': case 'i': //i - ignition
				if (engine.started())
				{
					stop_engine();
				}
				else
				{
					start_engine();
				}
				break;
			case 'F': case 'f':
				if (driver_inside)
				{
					cout << "You need to leave your car to fill tank." << endl;
				}
				else
				{
					double amount; //количество заправляемого топлива
					cout << "Input amount of fuel you want to fill: "; cin >> amount;
					tank.fill(amount); 
					cout << "Completed.";
				}
				break;
			case 'W': case 'w': accelerate(); break;
			case 'S': case 's': slow_down(); break;
			case Escape:
				speed = 0; //чтобы программа не падала при нажатии Escape на полном ходу
				stop_engine();
				get_out();
			}
			if (speed == 0 && control.free_wheeling_thread.joinable())
				control.free_wheeling_thread.join();
		} while (key != Escape);
	}
	void panel()const
	{
		while (driver_inside)
		{
			system("CLS");
			for (int i = 0; i < speed / 3; ++i) cout << "|";
			cout << endl;
			cout << "Speed:\t" << speed << "km/h\n";
			cout << "Fuel level: " << tank.get_fuel_level() << " liters.\t";
			if (tank.get_fuel_level() < 5)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0x0C);
				cout << "LOW FUEL";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			cout << endl;
			cout << "Engine is " << (engine.started() ? "Started" : "Stopped") << endl;
			if(engine.started()) cout << "Consumption per second: " << engine.get_consumption_per_second() << " l/s\n";
			std::this_thread::sleep_for(1s); //каждую секунду у нас внешний вид панели будет обновляться.
		}
	}
	void engine_idle()
	{
		while (engine.started() && tank.give_fuel( engine.get_consumption_per_second() ))
		{
			std::this_thread::sleep_for(1s);
		}
	}
	void free_wheeling()
	{
		while (speed > 0)
		{
			speed--;
			if (speed < 0) speed = 0;
			std::this_thread::sleep_for(1s);
			engine.correct_consumption(speed);	//<--- если перенести вызов функции на одну строку выше, то она, по каким-то причинам, не будет отрабатывать корректно.
		}										//предположительно, это связано с тем, что когда speed == 0, то мы просто не заходим в цикл,
	}											//потому consumption сброситься до стандартного не может (т.к. вызывактся ф-ия при speed > 0 по условию цикла). но почему тогда отрабатывает такой вариант? но он работает.


	void info()const
	{
		engine.info();
		tank.info();
	}
};
//#define TANK_CHECK
//#define ENGINE_CHECK

int main()
{
#ifdef TANK_CHECK
	Tank tank(40);
	tank.info();
	int fuel;
	while (true)
	{
		cout << "Input fuel volume: "; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	}
#endif // TANK_CHECK
#ifdef ENGINE_CHECK
	Engine engine(10);
	engine.info();
	return 0;
#endif // ENGINE_CHECK
	Car bmw(12, 60);
	//bmw.info();
	bmw.control_car();
}