#include "stdafx.h"
#include "CoreAudio.h"

CCoreAudio::CCoreAudio()
{
	m_pIMMEnumerator = 0;
	m_pIMMRenderDeivce = 0;
	m_pIMMInputDeivce = 0;
	m_pMicEndptVol = 0;
	m_pRenderEndptVol = 0;
}

CCoreAudio::~CCoreAudio()
{
	Uninit();
}

int CCoreAudio::Init()
{
	CoInitialize(0);
	try {
		HRESULT hr = S_OK;
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator),(void**)&m_pIMMEnumerator);
		if (FAILED(hr)) throw "CoCreateInstance";
		hr = m_pIMMEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_pIMMRenderDeivce);
		if (FAILED(hr)) throw "GetDefaultAudioEndpoint";
		hr = m_pIMMEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &m_pIMMInputDeivce);
		if (FAILED(hr)) throw "GetDefaultAudioEndpoint";
		hr = m_pIMMRenderDeivce->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&m_pRenderEndptVol);
		if (FAILED(hr)) throw "pDevice->Active";
		hr = m_pIMMInputDeivce->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&m_pMicEndptVol);
		if (FAILED(hr)) throw "pDevice->Active";
	}
	catch (...) {
		throw;
	}
	return 0;
}

void CCoreAudio::Uninit()
{
	if (m_pRenderEndptVol)
	{
		m_pRenderEndptVol.Release();
	}
	if (m_pMicEndptVol)
	{
		m_pMicEndptVol.Release();
	}
	if (m_pIMMInputDeivce)
	{
		m_pIMMInputDeivce.Release();
	}
	if (m_pIMMRenderDeivce)
	{
		m_pIMMRenderDeivce.Release();
	}
	if (m_pIMMEnumerator)
	{
		m_pIMMEnumerator.Release();
	}
	m_pIMMEnumerator = 0;
	m_pIMMRenderDeivce = 0;
	m_pIMMInputDeivce = 0;
	m_pMicEndptVol = 0;
	m_pRenderEndptVol = 0;
	CoUninitialize();
}

float CCoreAudio::GetMicLevelScalar()
{
	float level = 0.0f;
	if (m_pMicEndptVol)
	{
		m_pMicEndptVol->GetMasterVolumeLevelScalar(&level);
	}
	return level;
}

BOOL CCoreAudio::SetMicLevelScalar(float level)
{
	if (m_pMicEndptVol)
	{
		m_pMicEndptVol->SetMasterVolumeLevelScalar(level,NULL);
	}
	return TRUE;
}

float CCoreAudio::GetMicLevel()
{
	float level = 0.0f;
	if (m_pMicEndptVol)
	{
		m_pMicEndptVol->GetMasterVolumeLevel(&level);
	}
	return level;
}

BOOL CCoreAudio::SetMicLevel(float level)
{
	if (m_pMicEndptVol)
	{
		m_pMicEndptVol->SetMasterVolumeLevel(level,NULL);
	}
	return TRUE;
}
float CCoreAudio::GetSpeakerLevel()
{
	float level = 0.0f;
	if (m_pRenderEndptVol)
	{
		m_pRenderEndptVol->GetMasterVolumeLevelScalar(&level);
	}
	return level;
}

BOOL CCoreAudio::SetSpeakerLevel(float level)
{
	if (m_pRenderEndptVol)
	{
		m_pRenderEndptVol->SetMasterVolumeLevelScalar(level,NULL);
	}
	return TRUE;
}
