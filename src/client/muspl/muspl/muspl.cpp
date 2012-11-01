#include "muspl.h"

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "shlwapi.lib")

CMigPlayer::CMigPlayer() : m_pGB(NULL),
	m_pMC(NULL),
	m_pME(NULL),
	m_pMP(NULL),
	m_pSeek(NULL),
	m_pAudio(NULL),
	m_bMute(FALSE),
	m_dwSeekCaps(0),
	m_lVolume(MAX_VOLUME)
{
	m_hwndEvent = NULL;
	m_uEventMsg = 0;
	CoInitialize(NULL);
}


CMigPlayer::~CMigPlayer()
{
	UnInitializeGraph();

	CoUninitialize();
}

BOOL CMigPlayer::OpenMusic(const WCHAR* sFileName, BOOL islocal)
{
	BOOL bRet = FALSE;

	// 初始化graph builder
	if (FALSE == InitializeGraph())
	{
		return bRet;
	}

	if( !islocal )
	{
		if( FALSE == CheckFileName(sFileName) )
			return bRet;
	}
 	else
 	{
 		if( !PathFileExists(sFileName) )
 			return bRet;
 	}

	// 添加source filter
	IBaseFilter* pSource = NULL;
	if (m_pGB)
	{
		if (FAILED(m_pGB->AddSourceFilter(sFileName, islocal ? NULL : TEXT("CLSID_URLReader"), &pSource)))
		{
			return bRet;
		}
	}

	// 播放source
	if (FALSE == RenderStream(pSource))
	{
		return bRet;
	}

// 	if( S_OK != m_pSeek->GetCapabilities(&m_dwSeekCaps) )
// 	{
// 		return bRet;
// 	}
	m_pSeek->GetCapabilities(&m_dwSeekCaps);

	// 更新音量
	UpdateVolume();

	// 释放ibasefilter拥有的source filter
	SAFE_RELEASE(pSource);

	m_state = STATE_STOPPED;

	bRet = TRUE;

	return bRet;
}

BOOL CMigPlayer::InitializeGraph()
{
	BOOL bRet = FALSE;
	HRESULT hr = S_OK;

	// 清除上一次使用过的数据
	UnInitializeGraph();

	// 创建一个graph builder，作为播放的容器
	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, reinterpret_cast<void**>(&m_pGB));

	// 得到媒体控制接口
	if( SUCCEEDED(hr) )
	{
		hr = m_pGB->QueryInterface( IID_IMediaControl, (void**)(&m_pMC) );
	}

	// 得到媒体时间通知接口
	if( SUCCEEDED(hr) )
	{
		hr = m_pGB->QueryInterface( IID_IMediaEventEx, (void**)(&m_pME) );
	}

	// 得到音频接口
	if( SUCCEEDED(hr) )
	{
		hr = m_pGB->QueryInterface( IID_IBasicAudio, (void**)(&m_pAudio) );
	}

	// 播放位置接口
	if( SUCCEEDED(hr) )
	{
		hr = m_pGB->QueryInterface( IID_IMediaSeeking, (void**)(&m_pSeek) );
	}

	if( SUCCEEDED(hr) )
	{
		hr = m_pME->SetNotifyWindow( (OAHWND)m_hwndEvent, m_uEventMsg, NULL );
	}

	if( SUCCEEDED(hr) )
	{
		bRet = TRUE;
	}

	if( FALSE == bRet )
	{
		UnInitializeGraph();
	}

	return bRet;
}

void CMigPlayer::UnInitializeGraph()
{
	SAFE_RELEASE(m_pSeek);
	SAFE_RELEASE(m_pME);
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pGB);
	SAFE_RELEASE(m_pMP);
	SAFE_RELEASE(m_pAudio);

	m_bMute = FALSE;
}

BOOL CMigPlayer::UpdateVolume()
{
	if (NULL == m_pAudio)
		return FALSE;

	if (m_bMute)
	{
		if (FAILED(m_pAudio->put_Volume(MIN_VOLUME)))
			return FALSE;
	}
	else
	{
		if (FAILED(m_pAudio->put_Volume(m_lVolume)))
			return FALSE;
	}

	return TRUE;
}

BOOL CMigPlayer::RenderStream(IBaseFilter* psource)
{
	IFilterGraph2* pGraph2 = NULL;
	IBaseFilter* pAudioRenderer = NULL;
	IEnumPins* pEnum = NULL;

	// 获取graph2接口
	if (FAILED(m_pGB->QueryInterface(IID_IFilterGraph2, reinterpret_cast<void**>(&pGraph2))))
	{
		return FALSE;
	}

	// 添加音频渲染器
	if (FALSE == AddFilterByCLSID(m_pGB, CLSID_DSoundRender, &pAudioRenderer, NULL/*TEXT("Audio Renderer")*/))
	{
		SAFE_RELEASE(pGraph2);
		return FALSE;
	}

	// 获取枚举接口
	if (FAILED(psource->EnumPins(&pEnum)))
	{
		SAFE_RELEASE(pAudioRenderer);
		SAFE_RELEASE(pGraph2);
		return FALSE;
	}

	// 遍历pin接口以播放音频
	IPin* pPin = NULL;
	BOOL bSucceed = FALSE;

	while (S_OK == pEnum->Next(1, &pPin, NULL))
	{
		HRESULT hr = pGraph2->RenderEx(pPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL);

		pPin->Release();

		if (SUCCEEDED(hr))
		{
			bSucceed = TRUE;
		}
	}

	SAFE_RELEASE(pEnum);
	SAFE_RELEASE(pAudioRenderer);
	SAFE_RELEASE(pGraph2);

	if (FALSE == bSucceed)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CMigPlayer::CheckFileName(const WCHAR* sFile)
{
	return TRUE;
}

BOOL CMigPlayer::Play()
{
	if (STATE_PAUSED != m_state && STATE_STOPPED != m_state)
		return FALSE;

	if (NULL == m_pGB)
		return FALSE;

	if (FAILED(m_pMC->Run()))
		return FALSE;

	m_state = STATE_RUNNING;

	return TRUE;
}

BOOL CMigPlayer::Pause()
{
	if (STATE_RUNNING != m_state)
		return FALSE;

	if (NULL == m_pGB)
		return FALSE;

	if (FAILED(m_pMC->Pause()))
		return FALSE;

	m_state = STATE_PAUSED;

	return TRUE;
}

BOOL CMigPlayer::Stop()
{
	if (STATE_RUNNING != m_state && STATE_PAUSED != m_state)
		return FALSE;

	if (NULL == m_pGB)
		return FALSE;

	if (FAILED(m_pMC->Stop()))
		return FALSE;

	m_state = STATE_STOPPED;

	return TRUE;
}

BOOL CMigPlayer::SetVolume(long volume)
{
	m_lVolume = volume;
	return UpdateVolume();
}

long CMigPlayer::GetVolume()
{
	return m_lVolume;
}

BOOL CMigPlayer::Mute(BOOL bMute)
{
	m_bMute = bMute;
	return UpdateVolume();
}

BOOL CMigPlayer::IsMuted()
{
	return m_bMute;
}

BOOL CMigPlayer::AddFilterByCLSID(IGraphBuilder* pGraph, const GUID& clsid, IBaseFilter** ppF, LPCWSTR wszName /* = NULL */)
{
	if (!pGraph || !ppF)
		return FALSE;

	*ppF = 0;

	IBaseFilter* pFilter = NULL;

	// 创建
	if (FAILED(CoCreateInstance(
		clsid, 
		NULL, 
		CLSCTX_INPROC,
		IID_IBaseFilter,
		reinterpret_cast<void**>(&pFilter))))
	{
		return FALSE;
	}

	// 添加
	if (FAILED(pGraph->AddFilter(pFilter, wszName)))
	{
		return FALSE;
	}

	*ppF = pFilter;
	(*ppF)->AddRef();
	SAFE_RELEASE(pFilter);

	return TRUE;
}

PlaybackState CMigPlayer::GetPlayState()
{
	return m_state;
}

BOOL CMigPlayer::CanSeekPosition()
{
	DWORD caps = AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanGetDuration;
	return ((m_dwSeekCaps & caps) == caps);
}

BOOL CMigPlayer::SetPosition(LONGLONG pos)
{
	if( NULL == m_pMC || NULL == m_pSeek)
		return FALSE;

	BOOL bRet = FALSE;

	if( FAILED( m_pSeek->SetPositions(&pos, AM_SEEKING_AbsolutePositioning,
		NULL, AM_SEEKING_NoPositioning) ))
	{
		return bRet;
	}

	if( STATE_STOPPED == m_state)
	{
		m_pMC->StopWhenReady();
	}

	bRet = TRUE;

	return bRet;
}

BOOL CMigPlayer::GetDuration(LONGLONG* pDura)
{
	if( NULL == m_pSeek )
		return FALSE;

	if( FAILED(m_pSeek->GetDuration(pDura)) )
		return FALSE;

	return TRUE;
}

BOOL CMigPlayer::GetCurrentPosition(LONGLONG* pNow)
{
	if( NULL == m_pSeek)
		return FALSE;

	if( FAILED(m_pSeek->GetCurrentPosition(pNow)))
		return FALSE;

	return TRUE;
}

HRESULT CMigPlayer::HandleGraphEvent(GraphEventCallback* pCB)
{
	if( NULL == pCB )
		return E_POINTER;

	if( NULL == m_pME )
		return E_UNEXPECTED;

	long evCode = 0;
	LONG_PTR param1 = 0;
	LONG_PTR param2 = 0;

	HRESULT hr = S_OK;

	while( SUCCEEDED(m_pME->GetEvent(&evCode, &param1, &param2, 0)) )
	{
		pCB->OnGraphEvent( evCode, param1, param2 );

		hr = m_pME->FreeEventParams( evCode, param1, param2 );
		if( FAILED(hr) )
			break;
	}

	return hr;
}

void CMigPlayer::SetEventWindow(HWND hwnd, UINT msg)
{
	m_hwndEvent = hwnd;
	m_uEventMsg = msg;
}