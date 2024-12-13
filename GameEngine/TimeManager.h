#pragma once

/// <summary>
/// �̱������� �����ߴ�.
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
	// �̱��� ��ü�� ������ �ν��Ͻ��� �����ϱ� ���� ���� ��� ����
	static TimeManager* mpInstance;

public:
	// ���� ��� �Լ��� ���� �̱��� ��ü�� ���� �� �ֵ��� ��
	static TimeManager* getInstance();

	float TotalTime() const;  // in seconds
	float DeltaTime() const; // in seconds, ������ �� �ð�
	float FPS() const; // in seconds, �ð� �� ������

	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame.
};

