#include <windows.h>

#include "WebView.h"
#include "LuaBridge.h"

static WebView* g_webview = nullptr;
static LuaBridge* g_lua = nullptr;

LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
        if (g_webview)
        {
            // WebView2 controller resizing can be added here.
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(
        hwnd,
        message,
        wParam,
        lParam);
}

int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE,
    LPSTR,
    int show)
{
    const wchar_t CLASS_NAME[] =
        L"WebViewLuaBridgeWindow";

    WNDCLASSW wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"WebView + Lua Bridge",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        700,
        nullptr,
        nullptr,
        instance,
        nullptr);

    if (!hwnd)
        return 1;

    ShowWindow(hwnd, show);

    g_webview = new WebView(hwnd);

    if (!g_webview->initialize())
        return 1;

    g_lua = new LuaBridge();

    if (!g_lua->initialize(g_webview))
        return 1;

    const char* luaCode = R"(
        print("Lua bridge initialized")

        WebView.send("Hello from Lua")

        WebView.eval([[
            document.body.dataset.luaLoaded = "true";
        ]])
    )";

    if (luaL_dostring(
            g_lua->state(),
            luaCode) != LUA_OK)
    {
        const char* error =
            lua_tostring(
                g_lua->state(),
                -1);

        MessageBoxA(
            hwnd,
            error ? error : "Lua error",
            "Lua",
            MB_ICONERROR);
    }

    MSG msg{};

    while (GetMessageW(
        &msg,
        nullptr,
        0,
        0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    delete g_lua;
    delete g_webview;

    return 0;
}
