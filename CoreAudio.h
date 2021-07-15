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
	CComPtr<IMMDeviceEnumerator> 	m_pIMMEnumerator;   //��Ҫ����ö���豸�ӿ�
	CComPtr<IMMDevice>				m_pIMMRenderDeivce;	//output device
	CComPtr<IMMDevice>				m_pIMMInputDeivce;	//input device
	CComPtr<IAudioEndpointVolume>	m_pMicEndptVol;		//��˷��������ƽӿ�
	CComPtr<IAudioEndpointVolume>	m_pRenderEndptVol;	//�������������ƽӿ�
	//CComPtr<IAudioVolumeLevel>		m_pMicBoost;		//��˷�Ӳ����ǿ�ӿ�
	//CComPtr<IAudioAutoGainControl>	m_pAGC;				//��˷��Զ�����ӿ�
	//CComPtr<ISimpleAudioVolume>		m_pRenderSimpleVol;	//�������ĻỰ�������ƽӿ�

};

#endif//__CORE_AUDIO__