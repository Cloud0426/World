cmake_minimum_required(VERSION 3.10)
project(GameProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /utf-8 /permissive- /Zc:strictStrings- /wd4576 /wd4244 /wd4267 /wd4996")

# ===== raylib 配置 =====
set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/external/raylib")
set(RAYLIB_INCLUDE_DIR "${RAYLIB_DIR}/include")
set(RAYLIB_LIB_DIR "${RAYLIB_DIR}/lib")

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
)

add_executable(game ${SOURCES})

target_include_directories(game PRIVATE
    "src"
    "src/game"
    "src/item"
    "src/role"
    "src/role/character"
    "src/role/combatant"
    "src/role/enemy"
    "src/system"
    "src/system/backpack"
    "src/system/fight"
    "src/system/mission"
    "src/system/recruit"
    "src/system/store"
    # raylib
    "${RAYLIB_INCLUDE_DIR}"
)

# 链接 raylib
target_link_libraries(game PRIVATE
    "${RAYLIB_LIB_DIR}/raylib.lib"
    winmm.lib
    gdi32.lib
    opengl32.lib
)

# 将 raylib.dll 复制到输出目录
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${RAYLIB_LIB_DIR}/raylib.dll"
        "$<TARGET_FILE_DIR:game>"
)

# 将 resources 目录复制到输出目录（图片、字体等）
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:game>/resources"
)

# 将 All_resources 目录复制到输出目录
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/All_resources"
        "$<TARGET_FILE_DIR:game>/All_resources"
)

