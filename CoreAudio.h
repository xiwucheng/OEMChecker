#pragma once
#ifndef __CORE_AUDIO__
#define __CORE_AUDIO__
#include <windows.h> 
#include <mmdeviceapi.h> 
#include <endpointvolume.h>
#include <audioclient.h>
#pragma comment(lib,"Mmdevapi.lib")
class CCoreAudio
{
public:
	CCoreAudio();
	virtual ~CCoreAudio();
	int Init();
	void Uninit();
	float GetMicLevelScalar();
	BOOL SetMicLevelScalar(float level);
	float GetMicLevel();
	BOOL SetMicLevel(float level);
	float GetSpeakerLevel();
	BOOL SetSpeakerLevel(float level);
private:
	CComPtr<IMMDeviceEnumerator> 	m_pIMMEnumerator;   //主要用于枚举设备接口
	CComPtr<IMMDevice>				m_pIMMRenderDeivce;	//output device
	CComPtr<IMMDevice>				m_pIMMInputDeivce;	//input device
	CComPtr<IAudioEndpointVolume>	m_pMicEndptVol;		//麦克风音量控制接口
	CComPtr<IAudioEndpointVolume>	m_pRenderEndptVol;	//扬声器音量控制接口
	//CComPtr<IAudioVolumeLevel>		m_pMicBoost;		//麦克风硬件增强接口
	//CComPtr<IAudioAutoGainControl>	m_pAGC;				//麦克风自动增益接口
	//CComPtr<ISimpleAudioVolume>		m_pRenderSimpleVol;	//扬声器的会话音量控制接口

};

#endif//__CORE_AUDIO__