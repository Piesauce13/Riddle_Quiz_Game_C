cmake_minimum_required(VERSION 3.30)
project(Riddle_Quiz_Game_Test)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED)
include_directories(/opt/homebrew/opt/openssl@3/include)
link_directories(/opt/homebrew/opt/openssl@3/lib)

add_executable(Riddle_Quiz_Game_Test main.c
        UserManager.h
        QuizManager.h
        Leaderboard.h)

target_link_libraries(Riddle_Quiz_Game_Test OpenSSL::SSL OpenSSL::Crypto)
