#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
class Singleton
{
public:
	static T& getInstance()
	{
		static T instance;
		return instance;
	}
private:
	Singleton()
	{

	}

	Singleton(Singleton const&) = delete;
	void operator=(Singleton const&) = delete;
};

#endif // SINGLETON_H
