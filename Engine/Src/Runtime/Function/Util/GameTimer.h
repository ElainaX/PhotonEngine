#pragma once

namespace photon 
{

	class GameTimer
	{
	public:
		GameTimer();

		float TotalTime()const; // in seconds
		float DeltaTime()const; // in seconds

		void Reset(); // Call before message loop.
		void Start(); // Call when unpaused.
		void Stop();  // Call when paused.
		void Tick();  // Call every frame.

		void SetTimeScale(float timeScale = 1.0f);
		float GetTimeScale() const;

	private:
		double m_SecondsPerCount;
		double m_DeltaTime;

		__int64 m_BaseTime;
		__int64 m_PausedTime;
		__int64 m_StopTime;
		__int64 m_PrevTime;
		__int64 m_CurrTime;

		float m_TimeScale = 1.0;

		bool m_Stopped;
	};
}

