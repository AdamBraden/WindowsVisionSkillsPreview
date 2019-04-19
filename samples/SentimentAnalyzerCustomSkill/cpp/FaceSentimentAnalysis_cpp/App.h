#pragma once
#include <iostream>
#include <string>
#include <Windows.Foundation.h>
#include <windows.foundation.collections.h>
#include <wrl\implements.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl/client.h>
#include <wrl/event.h>
#include <windows.system.threading.h>
#include <windows.media.h>
#include <windows.media.capture.h>
#include <windows.media.capture.frames.h>
#include "Contoso.FaceSentimentAnalyzer.h"
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Contoso::FaceSentimentAnalyzer;
using namespace ABI::Microsoft::AI::Skills::SkillInterfacePreview;
using namespace ABI::Windows::System::Threading;
using namespace ABI::Windows::Media;
using namespace ABI::Windows::Media::Capture;
using namespace ABI::Windows::Media::Capture::Frames;

#define AwaitTypedResult(op,type,result) [&]() -> HRESULT                              \
{                                                                                                                               \
    HRESULT hr;                                                                                                                 \
    Event threadCompleted(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, WRITE_OWNER | EVENT_ALL_ACCESS));          \
    ComPtr<IAsyncOperationCompletedHandler<type>> cb                                                       \
    = Callback<Implements<RuntimeClassFlags<ClassicCom>, IAsyncOperationCompletedHandler<type>, FtmBase>>( \
        [&threadCompleted](IAsyncOperation<type>* asyncOperation, AsyncStatus status)->HRESULT                                   \
    {                                                                                                                           \
    SetEvent(threadCompleted.Get());                                                                                            \
    return S_OK;                                                                                                                \
    });                                                                                                                         \
    op->put_Completed(cb.Get());                                                                                             \
    WaitForSingleObject(threadCompleted.Get(), INFINITE);                                                                       \
    hr = op->GetResults(&result);                                                                                         \
    return hr;                                                                                                                  \
} ();

#define Await(op,result) [&]() -> HRESULT                              \
{                                                                                                                               \
    HRESULT hr;                                                                                                                 \
    Event threadCompleted(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, WRITE_OWNER | EVENT_ALL_ACCESS));          \
    ComPtr<IAsyncOperationCompletedHandler<decltype(result.Get())>> cb                                                       \
    = Callback<Implements<RuntimeClassFlags<ClassicCom>, IAsyncOperationCompletedHandler<decltype(result.Get())>, FtmBase>>( \
        [&threadCompleted](IAsyncOperation<decltype(result.Get())>* asyncOperation, AsyncStatus status)->HRESULT                                   \
    {                                                                                                                           \
        SetEvent(threadCompleted.Get());                                                                                            \
        return S_OK;                                                                                                                \
    });                                                                                                                         \
    op->put_Completed(cb.Get());                                                                                             \
    WaitForSingleObject(threadCompleted.Get(), INFINITE);                                                                       \
    hr = op->GetResults(&result);                                                                                         \
    return hr;                                                                                                                  \
} ();


#define AwaitAction(op) [&]() -> HRESULT                              \
{                                                                                                                               \
    HRESULT hr;                                                                                                                 \
    Event threadCompleted(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, WRITE_OWNER | EVENT_ALL_ACCESS));          \
    ComPtr<IAsyncActionCompletedHandler> cb                                                       \
    = Callback<Implements<RuntimeClassFlags<ClassicCom>, IAsyncActionCompletedHandler, FtmBase>>( \
        [&threadCompleted](decltype(op.Get()) asyncAction, AsyncStatus status)->HRESULT                                   \
    {                                                                                                                           \
        SetEvent(threadCompleted.Get());                                                                                            \
        return S_OK;                                                                                                                \
    });                                                                                                                         \
    op->put_Completed(cb.Get());                                                                                             \
    WaitForSingleObject(threadCompleted.Get(), INFINITE);                                                                       \
    hr = op->GetResults();                                                                                         \
    return hr;                                                                                                                  \
} ();

#define CHECKHR_GOTO( _hr, _lbl ) { hr = _hr; if( FAILED( hr ) ){ std::cout << std::endl << "Error at:" <<__FILE__ <<":" << __LINE__ << " hr: " << std::hex << hr; goto _lbl; } }

class App
{
    ComPtr<IMediaCapture> m_spMediaCapture;
    ComPtr<FaceSentimentAnalyzerBinding2> m_spFaceSentimentSkillBinding;
    ComPtr<ISkill> m_spSkill;
    EventRegistrationToken m_token;
    ComPtr<ITypedEventHandler<MediaFrameReader*, MediaFrameArrivedEventArgs*>> m_spFrameArrivedHandlerDelegate;
    ComPtr<IMediaFrameReader> m_spFrameReader;
    SRWLOCK m_lock;

    HRESULT initMediaCapture();
    HRESULT deInitMediaCapture();
    HRESULT FrameArrivedHandler(IMediaFrameReader* pFrameReader, IMediaFrameArrivedEventArgs*);
public:
    int AppMain();
};