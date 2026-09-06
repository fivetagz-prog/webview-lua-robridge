#include "WebView.h"

#include <windows.h>
#include <wrl.h>
#include <WebView2.h>

WebView::WebView(HWND hwnd)
    : hwnd_(hwnd)
{
}

WebView::~WebView()
{
    if (controller_)
        controller_->Close();
}

bool WebView::initialize()
{
    HRESULT result = CreateCoreWebView2EnvironmentWithOptions(
        nullptr,
        nullptr,
        nullptr,
        Microsoft::WRL::Callback<
            ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result,
                   ICoreWebView2Environment* environment) -> HRESULT
            {
                if (FAILED(result) || !environment)
                    return result;

                return environment->CreateCoreWebView2Controller(
                    hwnd_,
                    Microsoft::WRL::Callback<
                        ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this](HRESULT result,
                               ICoreWebView2Controller* controller) -> HRESULT
                        {
                            if (FAILED(result) || !controller)
                                return result;

                            controller_ = controller;

                            HRESULT hr =
                                controller_->get_CoreWebView2(
                                    &webview_);

                            if (FAILED(hr))
                                return hr;

                            RECT bounds;
                            GetClientRect(hwnd_, &bounds);

                            controller_->put_Bounds(bounds);

                            setupEvents();

                            navigate(
                                L"file:///" +
                                std::wstring(
                                    L"web/index.html"));

                            return S_OK;
                        })
                        .Get());
            })
            .Get());

    return SUCCEEDED(result);
}

void WebView::setupEvents()
{
    webview_->add_WebMessageReceived(
        Microsoft::WRL::Callback<
            ICoreWebView2WebMessageReceivedEventHandler>(
            [this](ICoreWebView2*,
                   ICoreWebView2WebMessageReceivedEventArgs* args)
            {
                LPWSTR message = nullptr;

                if (SUCCEEDED(
                        args->TryGetWebMessageAsString(&message)))
                {
                    if (messageCallback_)
                        messageCallback_(message);

                    CoTaskMemFree(message);
                }

                return S_OK;
            })
            .Get(),
        nullptr);
}

void WebView::navigate(const std::wstring& url)
{
    if (webview_)
        webview_->Navigate(url.c_str());
}

void WebView::executeScript(const std::wstring& script)
{
    if (webview_)
    {
        webview_->ExecuteScript(
            script.c_str(),
            nullptr);
    }
}

void WebView::setMessageCallback(
    MessageCallback callback)
{
    messageCallback_ = std::move(callback);
}
