cmake_minimum_required(VERSION 3.30)
project(Riddle_Quiz_Game_Test)

set(CMAKE_CXX_STANDARD 20)

add_executable(Riddle_Quiz_Game_Test main.cpp
        UserManager.h
        Question.h
        QuizManager.h
        Leaderboard.h)
