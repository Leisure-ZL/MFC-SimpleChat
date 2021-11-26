#include "stdlib.h"
#include <windows.h>  
#include <stdio.h>  
#include <mmsystem.h>  
#include "pch.h"
#include "SimpleChatClient.h"
#include "SimpleChatClientDlg.h"
#pragma comment(lib, "winmm.lib")    
#define BUFFER_SIZE (44100*16*2/8*5)    // ¼����������
//#define BUFFER_SIZE 44100    // ¼����������
#define FRAGMENT_SIZE 1024              // ��������С
#define FRAGMENT_NUM 4                  // ���������� 
static char buffer[BUFFER_SIZE] = { 0 };
static int buf_count = 0;

int nReturn;
WAVEINCAPS wic;  //WAVEINCAPS�ṹ����������Ƶ�����豸������
WAVEFORMATEX wavform;//WAVEFORMATEX�ṹ�����˲�����Ƶ���ݸ�ʽ������������ṹ��Ψһ�ĸ�ʽ��Ϣ����ͬ���в�����Ƶ���ݸ�ʽ��������Ҫ�������Ϣ�ĸ�ʽ������ṹ��������һ���ṹ�ĵ�һ����Ա���Լ���������Ϣ

// ��������
void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
// ¼���ص�����
void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    LPWAVEHDR pwh = (LPWAVEHDR)dwParam1;
    if ((WIM_DATA == uMsg) && (buf_count < BUFFER_SIZE))
    {
        int temp = BUFFER_SIZE - buf_count;
        temp = (temp > pwh->dwBytesRecorded) ? pwh->dwBytesRecorded : temp;
        memcpy(buffer + buf_count, pwh->lpData, temp);
        buf_count += temp;
        waveInAddBuffer(hwi, pwh, sizeof(WAVEHDR));
    }
}
void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
// �����ص�����
void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (WOM_DONE == uMsg)
    {
        buf_count = BUFFER_SIZE;
    }
}

void InitWave()
{
    wavform.wFormatTag = WAVE_FORMAT_PCM;  //WAVE_FORMAT_PCM���������
    wavform.nChannels = 2;  // ����
    wavform.nSamplesPerSec = 44100; // ����Ƶ��
    wavform.nAvgBytesPerSec = 44100 * 16 * 2 / 8;  // ÿ��������
    wavform.nBlockAlign = 4;
    wavform.wBitsPerSample = 16; // ������С
    wavform.cbSize = 0;  //��С�����ֽڣ����Ӷ���ĸ�ʽ��ϢWAVEFORMATEX�ṹ
}


void Record(CSimpleChatClientDlg* dlg) {
    /* ¼��*/ // Device  
    nReturn = waveInGetNumDevs();//���������豸����Ŀ
    printf("�����豸��Ŀ��%d\n", nReturn);

    //ʶ��������豸
    for (int i = 0; i < nReturn; i++)
    {

        waveInGetDevCaps(i, &wic, sizeof(WAVEINCAPS)); //waveInGetDevCaps���ܼ���һ�������Ĳ�����Ƶ�����豸������
        printf("#%d\t�豸����%s\n", i, wic.szPname);
    }
    // open  

    HWAVEIN hWaveIn;//������Ƶ���ݸ�ʽWave_audio���ݸ�ʽ



    //��¼���豸����
    waveInOpen(&hWaveIn, WAVE_MAPPER, &wavform, (DWORD_PTR)waveInProc, 0, CALLBACK_FUNCTION);

    //ʶ��򿪵�¼���豸
    WAVEINCAPS wic;
    waveInGetDevCaps((UINT_PTR)hWaveIn, &wic, sizeof(WAVEINCAPS));
    printf("�򿪵������豸��%s\n", wic.szPname);

    // prepare buffer
    static WAVEHDR wh[FRAGMENT_NUM];
    for (int i = 0; i < FRAGMENT_NUM; i++)
    {
        wh[i].lpData = new char[FRAGMENT_SIZE];
        wh[i].dwBufferLength = FRAGMENT_SIZE;
        wh[i].dwBytesRecorded = 0;
        wh[i].dwUser = NULL;
        wh[i].dwFlags = 0;
        wh[i].dwLoops = 1;
        wh[i].lpNext = NULL;
        wh[i].reserved = 0;

        //Ϊ¼���豸׼�����溯����
        //MMRESULT waveInPrepareHeader(  HWAVEIN hwi,  LPWAVEHDR pwh, UINT bwh );
        waveInPrepareHeader(hWaveIn, &wh[i], sizeof(WAVEHDR));

        //�������豸����һ�����棺
        //MMRESULT waveInAddBuffer(  HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh );
        waveInAddBuffer(hWaveIn, &wh[i], sizeof(WAVEHDR));
    }
    // record  
    OutputDebugString("Start to Record...");
    buf_count = 0; //�տ�ʼ¼����ʱ�򻺳����ĸ�����ʼ��Ϊ
    //��ʼ¼������
    //MMRESULT waveInStart(  HWAVEIN hwi  );  
    waveInStart(hWaveIn); //��ʼ¼��

    while (buf_count < BUFFER_SIZE)
    {
        Sleep(1);
    }
    dlg->MessageBox("¼���ѷ��ͣ�");
    OutputDebugString("Record Over!");
    waveInStop(hWaveIn);//waveInStop����ֹͣ�Ĳ�����Ƶ����
    //ֹͣ¼��������
    //MMRESULT waveInReset( HWAVEIN hwi ); 
    waveInReset(hWaveIn);//ֹͣ¼��
    //������溯����
    //MMRESULT waveInUnprepareHeader( HWAVEIN hwi,LPWAVEHDR pwh, UINT cbwh);  
    for (int i = 0; i < FRAGMENT_NUM; i++)
    {
        waveInUnprepareHeader(hWaveIn, &wh[i], sizeof(WAVEHDR));
        delete wh[i].lpData;
    }
    //�ر�¼���豸������
    //MMRESULT waveInClose( HWAVEIN hwi );
    waveInClose(hWaveIn);
    //system("pause"); 
    printf("\n");


}


void Play(char* data) {
    // Device  
    nReturn = waveOutGetNumDevs();  //��������豸����Ŀ
    printf("\n����豸��Ŀ��%d\n", nReturn);
    for (int i = 0; i < nReturn; i++)
    {
        WAVEOUTCAPS woc;  //WAVEINCAPS�ṹ����������Ƶ����豸������
        waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS));
        printf("#%d\t�豸����%s\n", i, wic.szPname);
    }

    // open  
    HWAVEOUT hWaveOut;//�򿪻ط��豸������
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wavform, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);

    WAVEOUTCAPS woc; //WAVEINCAPS�ṹ����������Ƶ����豸������
    waveOutGetDevCaps((UINT_PTR)hWaveOut, &woc, sizeof(WAVEOUTCAPS));
    printf("�򿪵�����豸��%s\n", wic.szPname);

    // prepare buffer  
    WAVEHDR wavhdr;
    wavhdr.lpData = (LPSTR)data;
    wavhdr.dwBufferLength = BUFFER_SIZE;  //MMRESULT waveOutPrepareHeader(HWAVEOUT hwo,LPWAVEHDR pwh,UINT cbwh);
    wavhdr.dwFlags = 0;  //Ϊ�ط��豸׼���ڴ�麯����
    wavhdr.dwLoops = 0;
    waveOutPrepareHeader(hWaveOut, &wavhdr, sizeof(WAVEHDR));
    // play  
    OutputDebugString("Start to Play...");
    buf_count = 0;
    //MMRESULT waveOutWrite(HWAVEOUT hwo,LPWAVEHDR pwh,UINT cbwh);
    waveOutWrite(hWaveOut, &wavhdr, sizeof(WAVEHDR)); //д���ݣ�����������
    //�����ļ���û�з���Ļ����в����˳�
    while (buf_count < BUFFER_SIZE)
    {
        Sleep(1);
    }
    // clean  
    waveOutReset(hWaveOut); //ֹͣ����
    //MMRESULT waveOutPrepareHeader(HWAVEOUT hwo,LPWAVEHDR pwh,UINT cbwh);
    waveOutUnprepareHeader(hWaveOut, &wavhdr, sizeof(WAVEHDR)); //Ϊ�ط��豸׼���ڴ�麯����
    waveOutClose(hWaveOut);  //�رշ����豸����
    OutputDebugString("Play Over!");
}
