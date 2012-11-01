#include <Windows.h>
#include <Shlwapi.h>
#include <DShow.h>
#include "ulti.h"

const long MIN_VOLUME = -10000;
const long MAX_VOLUME = 0;

enum PlaybackState
{
	STATE_RUNNING = 1,
	STATE_PAUSED,
	STATE_STOPPED,
	STATE_CLOSED,
};

struct GraphEventCallback
{
	virtual void OnGraphEvent(long eventCode, LONG_PTR param1, LONG_PTR param2) = 0;
};

class CMigPlayer
{
public:
	CMigPlayer();
	virtual ~CMigPlayer();

	BOOL OpenMusic(const WCHAR* sFileName, BOOL islocal = TRUE);	// 可多次调用,每首音乐都需要调用一次

	BOOL Play();	// 播放
	BOOL Pause();	// 暂停
	BOOL Stop();	// 停止
	PlaybackState GetPlayState();

	BOOL Mute(BOOL bMute);	// 静音
	BOOL IsMuted();	// 是否静音
	BOOL SetVolume(long volume);	// 设置音量
	long GetVolume();				// 获取当前音量

	BOOL CanSeekPosition();
	BOOL SetPosition(LONGLONG pos);				// 设置播放位置
	BOOL GetCurrentPosition(LONGLONG* pNow);		// 得到当前播放位置
	BOOL GetDuration(LONGLONG* pDura);				// 得到总的播放长度

	HRESULT HandleGraphEvent(GraphEventCallback* pCB);
	void SetEventWindow(HWND hwnd, UINT msg); // 建立通知窗口，在打开某个音乐文件之前调用

private:
	BOOL InitializeGraph();
	void UnInitializeGraph();

	BOOL RenderStream(IBaseFilter* psource);
	BOOL UpdateVolume();

	BOOL AddFilterByCLSID(IGraphBuilder* pGraph, const GUID& clsid, IBaseFilter** ppF, LPCWSTR wszName = NULL);
	BOOL CheckFileName(const WCHAR* sFile);

private:
	IGraphBuilder* m_pGB;
	IMediaControl* m_pMC;
	IMediaEventEx* m_pME;
	IMediaPosition* m_pMP;
	IMediaSeeking* m_pSeek;
	IBasicAudio* m_pAudio;

	HWND m_hwndEvent;
	UINT m_uEventMsg;

	long m_lVolume;	// 音量
	BOOL m_bMute;	// 是否静音
	DWORD m_dwSeekCaps;
	PlaybackState m_state;
};