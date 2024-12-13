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
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSeconds); // �����Ϳ� �������� ���̿����� C��Ÿ���� ĳ������ �ϴ� ���� �Ϲ����̴�.
	mSecondsPerCount = 1.0 / static_cast<double>(countsPerSeconds); // �ʴ� Ŭ�� Ƚ�� -> T = 1/f /// f = 1/T
}

// �ν��Ͻ��� ���� ��쿡�� �����ؼ� ��ȯ�Ѵ�.
TimeManager* TimeManager::getInstance()
{
	if (!mpInstance)
	{
		mpInstance = new TimeManager();
	}
	return mpInstance;
}

// �ð谡 ������ �ð��� ī��Ʈ���� �ʰ� Reset()�� ȣ��� �� ����� �� �ð��� ��ȯ.
float TimeManager::TotalTime() const
{
	// �츮�� ���� ���, �츮�� ���� �ķκ��� ���� �ð��� ���� ����.
	// ����, �츮�� ������ �Ͻ� ������ �� ���, (mStopTime - mBaseTime)�� �Ÿ����� �Ͻ� ������ �ð��� ���ԵǸ�, �� �ð��� ������� �ʴ´�.
	// �̸� �����Ϸ���, mStopTime���� �Ͻ� ������ �ð��� �� �� �ִ�:  
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

	if (m_isStopped)
	{
		return static_cast<float>(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}

	// (mCurrTime - mBaseTime)�� �Ÿ��� �Ͻ� ������ �ð��� �����ϰ�, �� �ð��� ������� �ʴ´�.
	// �̸� �����Ϸ���, mCurrTime���� �Ͻ� ������ �ð��� �� �� �ִ�.
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
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime); // ���� ���ļ� �˾Ƴ���

	mBaseTime = currTime;
	mPreviousTime = currTime;
	mStopTime = 0;
	m_isStopped = false;
}

// ���⼭ �� �� ���ϰ� ����ϱ� ���� startTime�̶� ���� ��� m_currentTime�� �ᵵ ������, �̷��� TotalTime()�Լ��� ��׷�����.
// �׷��� ���ǻ� �׳� �и��ؼ� ����Ѵ�.
void TimeManager::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// ������ ���� ���� ��� �� �ð��� �����Ѵ�.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime     

	if (m_isStopped)
	{
		mPausedTime += (startTime - mStopTime); // ����� ���� �� ������ �� �� �ֱ� ������ �Ͻ� ���� �ð��� ������ �����Ѵ�.

		mPreviousTime = startTime; // ���� �ð�(��� �� �� ���������� ���� �ð�)�� ���� �ð��� �����ϰ�
		mStopTime = 0; // stopTime�� ���½�Ų��. -> �Ͻ� ���� �ĺ����� �ð��� �ٽ� �����ϱ� ���� ���°� ����ϰ� �ٲٴ� ���̴�.
		m_isStopped = false;
	}
}

void TimeManager::Stop()
{
	if (!m_isStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime; // ���� ���ļ��� stopTime�� ����
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

	// �� �����Ӱ� ���� ������ ������ �ð� ����.
	mDeltaTime = (mCurrentTime - mPreviousTime) * mSecondsPerCount;

	const float targetDeltaTime = 1.0f / 60.0f; // 60 FPS ����
	while (mDeltaTime < targetDeltaTime)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mCurrentTime = currTime;
		mDeltaTime = (mCurrentTime - mPreviousTime) * mSecondsPerCount;
	}

	// ���� �������� �غ�.
	mPreviousTime = mCurrentTime;

	// �������� ����ȭ.
	//  DXSDK�� CDXUTTimer�� ���μ����� ���� ���(power save mode)�� ��ȯ�ǰų�, �ٸ� ���μ����� ���õǰ� �Ǹ�, mDeltaTime�� ������ �� �� �ִٰ� ����Ѵ�.
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}