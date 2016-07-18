// NOTE: CODE TAKEN FROM MSDN TUTORIAL ON TRANSCODE API
// https://msdn.microsoft.com/en-us/library/windows/desktop/ff819476(v=vs.85).aspx

#include <new>
#include <iostream>
#include <windows.h>
#include <mfapi.h>
#include <Mfidl.h>
#include <shlwapi.h>

#pragma comment(lib, "mfplat")
#pragma comment(lib, "mf")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "shlwapi")

enum eAVEncH264VProfile {
	eAVEncH264VProfile_unknown = 0,
	eAVEncH264VProfile_Simple = 66,
	eAVEncH264VProfile_Base = 66,
	eAVEncH264VProfile_Main = 77,
	eAVEncH264VProfile_High = 100,
	eAVEncH264VProfile_422 = 122,
	eAVEncH264VProfile_High10 = 110,
	eAVEncH264VProfile_444 = 144,
	eAVEncH264VProfile_Extended = 88,
	eAVEncH264VProfile_ScalableBase = 83,
	eAVEncH264VProfile_ScalableHigh = 86,
	eAVEncH264VProfile_MultiviewHigh = 118,
	eAVEncH264VProfile_StereoHigh = 128,
	eAVEncH264VProfile_ConstrainedBase = 256,
	eAVEncH264VProfile_UCConstrainedHigh = 257,
	eAVEncH264VProfile_UCScalableConstrainedBase = 258,
	eAVEncH264VProfile_UCScalableConstrainedHigh = 259

};

struct H264ProfileInfo {
	UINT32 profile;
	MFRatio fps;
	MFRatio frame_size;
	UINT32 bitrate;
};

H264ProfileInfo h264_profiles[] = {
	{ eAVEncH264VProfile_Base,{ 15, 1 },{ 176, 144 },   128000 },
	{ eAVEncH264VProfile_Base,{ 15, 1 },{ 352, 288 },   384000 },
	{ eAVEncH264VProfile_Base,{ 30, 1 },{ 352, 288 },   384000 },
	{ eAVEncH264VProfile_Base,{ 29970, 1000 },{ 320, 240 },   528560 },
	{ eAVEncH264VProfile_Base,{ 15, 1 },{ 720, 576 },  4000000 },
	{ eAVEncH264VProfile_Main,{ 25, 1 },{ 720, 576 }, 10000000 },
	{ eAVEncH264VProfile_Main,{ 30, 1 },{ 352, 288 }, 10000000 },
};

struct AACProfileInfo {
	UINT32 samplesPerSec;
	UINT32 numChannels;
	UINT32 bitsPerSample;
	UINT32 bytesPerSec;
	UINT32 aacProfile;
};

AACProfileInfo aac_profiles[] = {
	{ 96000, 2, 16, 24000, 0x29 },
	{ 48000, 2, 16, 24000, 0x29 },
	{ 44100, 2, 16, 16000, 0x29 },
	{ 44100, 2, 16, 12000, 0x29 },
};

HRESULT EncodeFile(PCWSTR pszInput, PCWSTR pszOutput);
HRESULT CreateMediaSource(PCWSTR pszURL, IMFMediaSource **ppSource);
HRESULT GetSourceDuration(IMFMediaSource *pSource, MFTIME *pDuration);
HRESULT CreateTranscodeProfile(IMFTranscodeProfile **ppProfile);
HRESULT CreateH264Profile(DWORD index, IMFAttributes **ppAttributes);
HRESULT CreateAACProfile(DWORD index, IMFAttributes **ppAttributes);
HRESULT RunEncodingSession(CSession *pSession, MFTIME duration);

int video_profile = 0;
int audio_profile = 0;

class CSession : public IMFAsyncCallback {
public:
	static HRESULT Create(CSession **ppSession);

	// IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IMFAsyncCallback methods
	STDMETHODIMP GetParameters(DWORD* pdwFlags, DWORD* pdwQueue) {
		// Implementation of this method is optional.
		return E_NOTIMPL;
	}
	STDMETHODIMP Invoke(IMFAsyncResult *pResult);

	// Other methods
	HRESULT StartEncodingSession(IMFTopology *pTopology);
	HRESULT GetEncodingPosition(MFTIME *pTime);
	HRESULT Wait(DWORD dwMsec);

private:
	CSession() : m_cRef(1), m_pSession(NULL), m_pClock(NULL), m_hrStatus(S_OK), m_hWaitEvent(NULL) {
	}
	virtual ~CSession() {
		if (m_pSession)
			m_pSession->Shutdown();

		SafeRelease(&m_pClock);
		SafeRelease(&m_pSession);
		CloseHandle(m_hWaitEvent);
	}

	HRESULT Initialize();

private:
	IMFMediaSession      *m_pSession;
	IMFPresentationClock *m_pClock;
	HRESULT m_hrStatus;
	HANDLE  m_hWaitEvent;
	long    m_cRef;
};


int wmain(int argc, wchar_t* argv[]) {
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
	if (argc < 3 || argc > 5) {
		std::cout << "Usage:" << std::endl;
		std::cout << "input output [ audio_profile video_profile ]" << std::endl;
		return 1;
	}

	if (argc > 3)
		audio_profile = _wtoi(argv[3]);

	if (argc > 4)
		video_profile = _wtoi(argv[4]);

	HRESULT hr = CoInitializeEx(NULL, COINT_APARTMENTTHREADED);
	if (SUCCEEDED(hr)) {
		hr = MFStartup(MF_VERSION);
		if (SUCCEEDED(hr)) {
			hr = EncodeFile(argv[1], argv[2]);
			MFShutdown();
		}
		CoUninitialize();
	}

	if (SUCCEEDED(hr))
		std::cout << "Done." << std::endl;
	else
		std::cout << "Error: " << std:hex << hr << std::endl;

	return 0;
}

HRESULT EncodeFile(PCWSTR pszInput, PCWSTR pszOutput) {
	IMFTranscodeProfile *pProfile = NULL;
	IMFMediaSource *pSource = NULL;
	IMFTopology *pTopology = NULL;
	CSession *pSession = NULL;

	MFTIME duration = 0;

	HRESULT hr = CreateMediaSource(pszInput, &pSource);
	if (FAILED(hr))
		goto done;

	hr = GetSourceDuration(pSource, &duration);
	if (FAILED(hr))
		goto done;

	hr = CreateTranscodeProfile(&pProfile);
	if (FAILED(hr))
		goto done;

	hr = MFCreateTranscodeTopology(pSource, pszOutput, pProfile, &pTopology);
	if (FAILED(hr))
		goto done;

	hr = CSession::Create(&pSession);
	if (FAILED(hr))
		goto done;

	hr = pSession->StartEncodingSession(pTopology);
	if (FAILED(hr))
		goto done;

	hr = RunEncodingSession(pSession, duration);

done:
	if (pSource)
		pSource->Shutdown();

	SafeRelease(&pSession);
	SafeRelease(&pProfile);
	SafeRelease(&pSource);
	SafeRelease(&pTopology);
	return hr;
}

HRESULT CreateMediaSource(PCWSTR pszURL, IMFMediaSource **ppSource) {
	MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;

	IMFSourceResolver* pResolver = NULL;
	IUnknown* pSource = NULL;

	// Create the source resolver.
	HRESULT hr = MFCreateSourceResolver(&pResolver);
	if (FAILED(hr))
		goto done;

	// Use the source resolver to create the media source
	hr = pResolver->CreateObjectFromURL(pszURL, MF_RESOLUTION_MEDIASOURCE,
		NULL, &ObjectType, &pSource);
	if (FAILED(hr))
		goto done;

	// Get the IMFMediaSource interface from the media source.
	hr = pSource->QueryInterface(IID_PPV_ARGS(ppSource));

done:
	SafeRelease(&pResolver);
	SafeRelease(&pSource);
	return hr;
}

HRESULT GetSourceDuration(IMFMediaSource *pSource, MFTIME *pDuration) {
	*pDuration = 0;

	IMFPresentationDescriptor *pPD = NULL;

	HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
	if (SUCCEEDED(hr)) {
		hr = pPD->GetUINT64(MF_PD_DURATION, (UINT64*)pDuration);
		pPD->Release();
	}
	return hr;
}

HRESULT CreateTranscodeProfile(IMFTranscodeProfile **ppProfile) {
	IMFTranscodeProfile *pProfile = NULL;
	IMFAttributes *pAudio = NULL;
	IMFAttributes *pVideo = NULL;
	IMFAttributes *pContainer = NULL;

	HRESULT hr = MFCreateTranscodeProfile(&pProfile);
	if (FAILED(hr))
		goto done;

	// Audio attributes.
	hr = CreateAACProfile(audio_profile, &pAudio);
	if (FAILED(hr))
		goto done;

	hr = pProfile->SetAudioAttributes(pAudio);
	if (FAILED(hr))
		goto done;

	// Video attributes.
	hr = CreateH264Profile(video_profile, &pVideo);
	if (FAILED(hr))
		goto done;

	hr = pProfile->SetVideoAttributes(pVideo);
	if (FAILED(hr))
		goto done;

	// Container attributes.
	hr = MFCreateAttributes(&pContainer, 1);
	if (FAILED(hr))
		goto done;

	hr = pContainer->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4);
	if (FAILED(hr))
		goto done;

	hr = pProfile->SetContainerAttributes(pContainer);
	if (FAILED(hr))
		goto done;

	*ppProfile = pProfile;
	(*ppProfile)->AddRef();

done:
	SafeRelease(&pProfile);
	SafeRelease(&pAudio);
	SafeRelease(&pVideo);
	SafeRelease(&pContainer);
	return hr;
}

HRESULT CreateH264Profile(DWORD index, IMFAttributes **ppAttributes) {
	if (index >= ARRAYSIZE(h264_profiles))
		return E_INVALIDARG;

	IMFAttributes *pAttributes = NULL;

	const H264ProfileInfo& profile = h264_profiles[index];

	HRESULT hr = MFCreateAttributes(&pAttributes, 5);
	if (SUCCEEDED(hr))
		hr = pAttributes->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
	if (SUCCEEDED(hr))
		hr = pAttributes->SetUINT32(MF_MT_MPEG2_PROFILE, profile.profile);
	if (SUCCEEDED(hr)) {
		hr = MFSetAttributeSize(
			pAttributes, MF_MT_FRAME_SIZE,
			profile.frame_size.Numerator, profile.frame_size.Numerator);
	}
	if (SUCCEEDED(hr)) {
		hr = MFSetAttributeRatio(
			pAttributes, MF_MT_FRAME_RATE,
			profile.fps.Numerator, profile.fps.Denominator);
	}
	if (SUCCEEDED(hr))
		hr = pAttributes->SetUINT32(MF_MT_AVG_BITRATE, profile.bitrate);
	if (SUCCEEDED(hr)) {
		*ppAttributes = pAttributes;
		(*ppAttributes)->AddRef();
	}
	SafeRelease(&pAttributes);
	return hr;
}

HRESULT CreateAACProfile(DWORD index, IMFAttributes **ppAttributes) {
	if (index >= ARRAYSIZE(h264_profiles))
		return E_INVALIDARG;

	const AACProfileInfo& profile = aac_profiles[index];

	IMFAttributes *pAttributes = NULL;

	HRESULT hr = MFCreateAttributes(&pAttributes, 7);
	if (SUCCEEDED(hr))
		hr = pAttributes->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
	if (SUCCEEDED(hr)) {
		hr = pAttributes->SetUINT32(
			MF_MT_AUDIO_BITS_PER_SAMPLE, profile.bitsPerSample);
	}
	if (SUCCEEDED(hr)) {
		hr = pAttributes->SetUINT32(
			MF_MT_AUDIO_SAMPLES_PER_SECOND, profile.samplesPerSec);
	}
	if (SUCCEEDED(hr)) {
		hr = pAttributes->SetUINT32(
			MF_MT_AUDIO_NUM_CHANNELS, profile.numChannels);
	}
	if (SUCCEEDED(hr)) {
		hr = pAttributes->SetUINT32(
			MF_MT_AUDIO_AVG_BYTES_PER_SECOND, profile.bytesPerSec);
	}
	if (SUCCEEDED(hr))
		hr = pAttributes->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 1);
	if (SUCCEEDED(hr)) {
		hr = pAttributes->SetUINT32(
			MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, profile.aacProfile);
	}
	if (SUCCEEDED(hr)) {
		*ppAttributes = pAttributes;
		(*ppAttributes)->AddRef();
	}
	SafeRelease(&pAttributes);
	return hr;
}

HRESULT RunEncodingSession(CSession *pSession, MFTIME duration) {
	const DWORD WAIT_PERIOD = 500;
	const int   UPDATE_INCR = 5;

	HRESULT hr = S_OK;
	MFTIME pos;
	LONGLONG prev = 0;
	while (1) {
		hr = pSession->Wait(WAIT_PERIOD);
		if (hr == E_PENDING) {
			hr = pSession->GetEncodingPosition(&pos);

			LONGLONG percent = (100 * pos) / duration;
			if (percent >= prev + UPDATE_INCR) {
				std::cout << percent << "% .. ";
				prev = percent;
			}
		}
		else {
			std::cout << std::endl;
			break;
		}
	}
	return hr;
}







                                          
                                          