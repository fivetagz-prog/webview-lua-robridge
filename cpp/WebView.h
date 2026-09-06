#pragma once

#include <windows.h>
#include <wrl.h>
#include <WebView2.h>
#include <functional>
#include <string>

class WebView
{
public:
    using MessageCallback = std::function<void(const std::string&)>;

    WebView(HWND hwnd);
    ~WebView();

    bool initialize();
    void navigate(const std::wstring& url);
    void executeScript(const std::wstring& script);

    void setMessageCallback(MessageCallback callback);

private:
    HWND hwnd_;

    Microsoft::WRL::ComPtr<ICoreWebView2Controller> controller_;
    Microsoft::WRL::ComPtr<ICoreWebView2> webview_;

    MessageCallback messageCallback_;

    void setupEvents();
};
