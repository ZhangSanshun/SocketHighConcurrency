#ifndef _CELLTimestamp_hpp_
#define _CELLTimestamp_hpp_

#include <chrono>

using namespace std::chrono;

class CELLTimestamp
{
public:

	CELLTimestamp()
	{
		Update();
	}

	~CELLTimestamp()
	{

	}

	void Update()
	{
		_begin = high_resolution_clock::now();
	}

	/**
	* @brief  ªÒ»°√Î
	* @return  √Î
	*/
	double GetElapsedSecond()
	{
		return this->GetElapsedTimeInMicroSec() * 0.000001;
	}

	/**
	* @brief  ªÒ»°∫¡√Î
	* @return  ∫¡√Î
	*/
	double GetElapsedTimeInMilliSec()
	{
		return this->GetElapsedTimeInMicroSec() * 0.001;
	}

	/**
	* @brief  ªÒ»°Œ¢√Î
	* @return  Œ¢√Î
	*/
	long long GetElapsedTimeInMicroSec()
	{
		return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
	}

private:

	time_point<high_resolution_clock> _begin;

};


#endif !_CELLTimestamp_hpp_