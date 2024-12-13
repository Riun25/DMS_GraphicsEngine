#include <windows.h>
#include <iostream>
#include "TimeManager.h"

//***************************************************************************************
// GameTimer.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

TimeManager* TimeManager::mpInstance = nullptr;

TimeManager::TimeManager() : mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0), mPausedTime(0),
							 mStopTime(0), mPreviousTime(0), mCurrentTime(0), m_isStopped(false)
{
	__int64 countsPerSeconds;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSeconds); // 포인터와 비포인터 사이에서는 C스타일의 캐스팅을 하는 것이 일반적이다.
	mSecondsPerCount = 1.0 / static_cast<double>(countsPerSeconds); // 초당 클록 횟수 -> T = 1/f /// f = 1/T
}

// 인스턴스가 없는 경우에만 생성해서 반환한다.
TimeManager* TimeManager::getInstance()
{
	if (!mpInstance)
	{
		mpInstance = new TimeManager();
	}
	return mpInstance;
}

// 시계가 정지된 시간을 카운트하지 않고 Reset()이 호출된 후 경과한 총 시간을 반환.
float TimeManager::TotalTime() const
{
	// 우리가 멈춘 경우, 우리가 멈춘 후로부터 지난 시간을 세지 말자.
	// 또한, 우리가 이전에 일시 중지를 한 경우, (mStopTime - mBaseTime)의 거리에는 일시 중지된 시간이 포함되며, 이 시간은 계산하지 않는다.
	// 이를 수정하려면, mStopTime에서 일시 중지된 시간을 뺄 수 있다:  
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

	if (m_isStopped)
	{
		return static_cast<float>(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}

	// (mCurrTime - mBaseTime)의 거리는 일시 중지된 시간을 포함하고, 이 시간은 계산하지 않는다.
	// 이를 수정하려면, mCurrTime에서 일시 중지된 시간을 뺄 수 있다.
	//  (mCurrTime - mPausedTime) - mBaseTime 
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mCurrTime

	else
	{
		return static_cast<float>(((mCurrentTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
}

float TimeManager::DeltaTime()const
{
	//std::cout << static_cast<float>(m_deltaTime) << std::endl;
	return static_cast<float>(mDeltaTime);
}

float TimeManager::FPS() const
{
	//std::cout << 1.0 / static_cast<float>(m_deltaTime) << std::endl;
	return 1.0f / static_cast<float>(mDeltaTime);
}

void TimeManager::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime); // 현재 주파수 알아내기

	mBaseTime = currTime;
	mPreviousTime = currTime;
	mStopTime = 0;
	m_isStopped = false;
}

// 여기서 좀 더 편하게 사용하기 위해 startTime이란 변수 대신 m_currentTime을 써도 되지만, 이러면 TotalTime()함수가 어그러진다.
// 그래서 편의상 그냥 분리해서 사용한다.
void TimeManager::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// 정지와 시작 간에 경과 된 시간을 누적한다.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime     

	if (m_isStopped)
	{
		mPausedTime += (startTime - mStopTime); // 사람이 여러 번 정지를 할 수 있기 때문에 일시 정지 시간은 무조건 누적한다.

		mPreviousTime = startTime; // 시작 시간(사실 상 이 시점에서는 현재 시간)을 이전 시간에 저장하고
		mStopTime = 0; // stopTime을 리셋시킨다. -> 일시 정지 후부터의 시간을 다시 측정하기 위해 리셋과 비슷하게 바꾸는 것이다.
		m_isStopped = false;
	}
}

void TimeManager::Stop()
{
	if (!m_isStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime; // 현재 주파수를 stopTime에 저장
		m_isStopped = true;
	}
}

void TimeManager::Tick()
{
	if (m_isStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrentTime = currTime;

	// 이 프레임과 이전 프레임 사이의 시간 차이.
	mDeltaTime = (mCurrentTime - mPreviousTime) * mSecondsPerCount;

	const float targetDeltaTime = 1.0f / 60.0f; // 60 FPS 기준
	while (mDeltaTime < targetDeltaTime)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mCurrentTime = currTime;
		mDeltaTime = (mCurrentTime - mPreviousTime) * mSecondsPerCount;
	}

	// 다음 프레임을 준비.
	mPreviousTime = mCurrentTime;

	// 강제적인 음수화.
	//  DXSDK의 CDXUTTimer는 프로세스가 절전 모드(power save mode)로 전환되거나, 다른 프로세서로 셔플되게 되면, mDeltaTime이 음수가 될 수 있다고 언급한다.
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}