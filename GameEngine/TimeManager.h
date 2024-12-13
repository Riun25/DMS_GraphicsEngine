#pragma once

/// <summary>
/// 싱글톤으로 제작했다.
/// 2024.4.23.
/// </summary>
class TimeManager
{
public:
	TimeManager();

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPreviousTime;
	__int64 mCurrentTime;

	bool m_isStopped;

public:
	// 싱글톤 객체의 유일한 인스턴스를 저장하기 위한 정적 멤버 변수
	static TimeManager* mpInstance;

public:
	// 정적 멤버 함수를 통해 싱글톤 객체를 얻을 수 있도록 함
	static TimeManager* getInstance();

	float TotalTime() const;  // in seconds
	float DeltaTime() const; // in seconds, 프레임 당 시간
	float FPS() const; // in seconds, 시간 당 프레임

	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame.
};

