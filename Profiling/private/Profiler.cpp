#include "Profiler.h"

#include "ProfilerMeta.h"
#include "ProfilerJobSystemMeta.h"

#include <iostream>

profiling::Profiler::Profiler() :
	BaseObject(profiling::ProfilerMeta::GetInstance())
{
	m_profilerJobSystem = new jobs::JobSystem(profiling::ProfilerJobSystemMeta::GetInstance(), 1);
}

profiling::Profiler::~Profiler()
{
}


jobs::JobSystem* profiling::Profiler::GetProfilerJobSystem()
{
	return m_profilerJobSystem;
}

void profiling::Profiler::RecordTime(const std::string& profileName, const std::string& stopwatchName, double time)
{
	auto profileIt = m_profiles.find(profileName);
	if (profileIt == m_profiles.end())
	{
		m_profiles[profileName] = Profile();
		profileIt = m_profiles.find(profileName);
	}

	Profile& profile = profileIt->second;

	auto stopwatchIt = profile.m_stopwatchData.find(stopwatchName);
	if (stopwatchIt == profile.m_stopwatchData.end())
	{
		profile.m_stopwatchData[stopwatchName] = StopwatchInfo();
		stopwatchIt = profile.m_stopwatchData.find(stopwatchName);
	}

	StopwatchInfo& stopwatchInfo = stopwatchIt->second;

	++stopwatchInfo.m_numberOfCalls;
	stopwatchInfo.m_accumulatedTime += time;
}

void profiling::Profiler::ResetProfile(const std::string& profileName)
{
	m_profiles.erase(profileName);
}

void profiling::Profiler::PrintProfile(const std::string& profileName)
{
	using namespace std;

	auto profileIt = m_profiles.find(profileName);
	if (profileIt == m_profiles.end())
	{
		cout << "No profile found!";
	}

	Profile& profile = profileIt->second;
	cout << profileName << ":" << endl;

	for (auto it = profile.m_stopwatchData.begin(); it != profile.m_stopwatchData.end(); ++it)
	{
		cout << it->first << '\t' << it->second.m_numberOfCalls << '\t' << it->second.m_accumulatedTime << endl;
	}
}