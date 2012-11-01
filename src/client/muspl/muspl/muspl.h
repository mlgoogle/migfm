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

	BOOL OpenMusic(const WCHAR* sFileName, BOOL islocal = TRUE);	// �ɶ�ε���,ÿ�����ֶ���Ҫ����һ��

	BOOL Play();	// ����
	BOOL Pause();	// ��ͣ
	BOOL Stop();	// ֹͣ
	PlaybackState GetPlayState();

	BOOL Mute(BOOL bMute);	// ����
	BOOL IsMuted();	// �Ƿ���
	BOOL SetVolume(long volume);	// ��������
	long GetVolume();				// ��ȡ��ǰ����

	BOOL CanSeekPosition();
	BOOL SetPosition(LONGLONG pos);				// ���ò���λ��
	BOOL GetCurrentPosition(LONGLONG* pNow);		// �õ���ǰ����λ��
	BOOL GetDuration(LONGLONG* pDura);				// �õ��ܵĲ��ų���

	HRESULT HandleGraphEvent(GraphEventCallback* pCB);
	void SetEventWindow(HWND hwnd, UINT msg); // ����֪ͨ���ڣ��ڴ�ĳ�������ļ�֮ǰ����

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

	long m_lVolume;	// ����
	BOOL m_bMute;	// �Ƿ���
	DWORD m_dwSeekCaps;
	PlaybackState m_state;
};