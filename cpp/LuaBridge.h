#pragma once

#include "WebView.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class LuaBridge
{
public:
    LuaBridge();
    ~LuaBridge();

    bool initialize(WebView* webview);

    lua_State* state();

private:
    lua_State* lua_;
    WebView* webview_;

    static int luaWebViewOpen(lua_State* L);
    static int luaWebViewEval(lua_State* L);
    static int luaWebViewSend(lua_State* L);
};
