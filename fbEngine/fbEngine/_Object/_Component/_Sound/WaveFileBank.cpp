/*!
 *@brief	波形データバンク。
 */

#include "WaveFile.h"
#include "WaveFileBank.h"


WaveFileBank::WaveFileBank()
{
}
WaveFileBank::~WaveFileBank()
{
	ReleaseAll();
}
void WaveFileBank::Release(int groupID)
{
	for (auto waveFile : m_waveFileMap[groupID]) {
		waveFile.second->Release();
	}
	m_waveFileMap[groupID].clear();
}
void WaveFileBank::RegistWaveFile(int groupID, WaveFilePtr waveFile)
{
	FB_ASSERT(groupID < MAX_GROUP, "groupID is invalid");
	m_waveFileMap[groupID].insert({ waveFile->GetFilePathHash(), waveFile });
}
WaveFilePtr WaveFileBank::FindWaveFile(int groupID, const char* filePath)
{
	FB_ASSERT(groupID < MAX_GROUP, "groupID is invalid");
	auto value = m_waveFileMap[groupID].find(Support::MakeHash((char*)filePath));
	if (value != m_waveFileMap[groupID].end()) {
		return value->second;
	}
	return WaveFilePtr();
}
WaveFilePtr WaveFileBank::FindWaveFile(int groupID, const NameKey& nameKey)
{
	FB_ASSERT(groupID < MAX_GROUP, "groupID is invalid");
	auto value = m_waveFileMap[groupID].find(nameKey.GetHashCode());
	if (value != m_waveFileMap[groupID].end()) {
		return value->second;
	}
	return WaveFilePtr();
}
