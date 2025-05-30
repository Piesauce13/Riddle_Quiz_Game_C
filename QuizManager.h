#ifndef QUIZ_MANAGER_H
#define QUIZ_MANAGER_H

#define MAX_QUESTIONS 10
#define MAX_LINE_LENGTH 256

typedef struct {
    int id;
    char question[150];
    char optionA[50];
    char optionB[50];
    char optionC[50];
    char correctAnswer; // 'A', 'B', or 'C'
} Question;

void loadQuestions(const char *filename, Question *questions, int *count);
void playQuiz(const char *username, Question *easy, int eCount, Question *medium, int mCount, Question *hard, int hCount);
int askQuestions(const char *levelName, Question *questions, int count, int startQNum, int *quitFlag);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Leaderboard.h"

void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    printf("\n\nPress Enter to continue...");
    while (getchar() != '\n'); // To consume leftover newline
    getchar(); // Wait for Enter
}

void loadQuestions(const char *filename, Question *questions, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    *count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (*count >= MAX_QUESTIONS) break;

        Question q;
        sscanf(line, "%d,%[^,],%[^,],%[^,],%[^,],%c",
               &q.id, q.question, q.optionA, q.optionB, q.optionC, &q.correctAnswer);

        questions[*count] = q;
        (*count)++;
    }

    fclose(file);
}

void shuffleQuestions(Question *questions, int count) {
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Question temp = questions[i];
        questions[i] = questions[j];
        questions[j] = temp;
    }
}

int askQuestions(const char *levelName, Question *questions, int count, int startQNum, int *quitFlag) {
    int correct = 0;
    char userAnswer;

    printf("===== %s LEVEL =====\n", levelName);
    for (int i = 0; i < 3 && i < count; i++) {
        printf("Q%d: %s\n", startQNum + i, questions[i].question);
        printf("A. %s\n", questions[i].optionA);
        printf("B. %s\n", questions[i].optionB);
        printf("C. %s\n", questions[i].optionC);
        printf("Your answer (A/B/C or Q to quit): ");
        scanf(" %c", &userAnswer);

        if (userAnswer == 'q' || userAnswer == 'Q') {
            printf("\nYou chose to quit. Your progress will not be saved.\n");
            *quitFlag = 1;
            return 0;
        }

        if (userAnswer == questions[i].correctAnswer || userAnswer == questions[i].correctAnswer + 32) {
            printf("Correct!\n\n");
            correct++;
        } else {
            printf("Wrong! Correct answer: %c\n\n", questions[i].correctAnswer);
        }
    }

    printf("You have finished the %s level.\n", levelName);
    printf("Score for this level: %d/3\n\n", correct);
    pause();
    return correct;
}

void playQuiz(const char *username, Question *easy, int eCount, Question *medium, int mCount, Question *hard, int hCount) {
    srand(time(NULL));
    shuffleQuestions(easy, eCount);
    shuffleQuestions(medium, mCount);
    shuffleQuestions(hard, hCount);

    int score = 0;
    int quitFlag = 0;
    clock_t start = clock();

    clearConsole();
    printf("Welcome, %s! Let's start the quiz.\n", username);
    printf("You can type 'Q' at any time to quit (your score will not be saved).\n\n");
    pause();

    score += askQuestions("EASY", easy, eCount, 1, &quitFlag);
    if (quitFlag) return;

    score += askQuestions("MEDIUM", medium, mCount, 4, &quitFlag);
    if (quitFlag) return;

    score += askQuestions("HARD", hard, hCount, 7, &quitFlag);
    if (quitFlag) return;

    clock_t end = clock();
    double timeTaken = (double)(end - start) / CLOCKS_PER_SEC;

    // Check if user already exists and has a higher score
    FILE *leaderboard = fopen("leaderboard.txt", "r");
    int highScore = -1;  // Assume no previous score
    double bestTime = 0.0;

    if (leaderboard) {
        char line[256], fileUsername[50];
        int fileScore;
        double fileTime;
        while (fgets(line, sizeof(line), leaderboard)) {
            if (sscanf(line, "%49[^,], %d, %lf", fileUsername, &fileScore, &fileTime) == 3) {
                if (strcmp(fileUsername, username) == 0) {
                    highScore = fileScore;
                    bestTime = fileTime;
                    break;
                }
            }
        }
        fclose(leaderboard);
    }

    if (highScore == -1) {
        // User not found, add new entry
        FILE *appendFile = fopen("leaderboard.txt", "a");
        if (appendFile) {
            fprintf(appendFile, "%s, %d, %.2f\n", username, score, timeTaken);
            fclose(appendFile);
            printf("New score added to leaderboard!\n");
        } else {
            printf("Could not write to leaderboard.txt\n");
        }
    } else if (score > highScore || (score == highScore && timeTaken < bestTime)) {
        // New high score or better time at the same score, update leaderboard
        char timeStr[20];
        snprintf(timeStr, sizeof(timeStr), "%.2f", timeTaken);
        updateScore((char *)username, score, timeStr);
        printf("High score updated on leaderboard!\n");
    } else {
        // No new high score
        printf("You did not beat your previous high score of %d.\n", highScore);
    }

    printf("Quiz complete! You got %d out of 9 correct.\n", score);
    printf("Time taken: %.2f seconds\n", timeTaken);
    pause();
}


#endif //QUIZMANAGER_H
