cmake_minimum_required(VERSION 3.20)

project(WebViewLuaBridge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(unofficial-webview2 CONFIG REQUIRED)
find_package(Lua REQUIRED)

add_executable(WebViewLuaBridge WIN32
    cpp/main.cpp
    cpp/WebView.cpp
    cpp/WebView.h
    cpp/LuaBridge.cpp
    cpp/LuaBridge.h
)

target_include_directories(WebViewLuaBridge PRIVATE
    ${LUA_INCLUDE_DIR}
    cpp
)

target_link_libraries(WebViewLuaBridge PRIVATE
    unofficial::webview2::webview2
    ${LUA_LIBRARIES}
)

add_custom_command(TARGET WebViewLuaBridge POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/web"
        "$<TARGET_FILE_DIR:WebViewLuaBridge>/web"
)
