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
    char correctAnswer;
} Question;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Leaderboard.h"

void loadQuestions(const char *filename, Question *questions, int *count);
void shuffleQuestions(Question *questions, int count);
int askQuestions(const char *levelName, Question *questions, int count, int startQNum, int *quitFlag);
void playQuiz(const char *username, Question *easy, int eCount, Question *medium, int mCount, Question *hard, int hCount);


void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    printf("\n\n\nPress Enter to continue...");
    getchar();
}

void pause2() {
    printf("\n\n\nPress Enter to continue...");
    while(getchar() != '\n');
    getchar();
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

    printf("\n\n===== %s LEVEL =====\n\n", levelName);
    for (int i = 0; i < 3 && i < count; i++) {
        printf("Q%d: %s\n\n", startQNum + i, questions[i].question);
        printf("A. %s\n", questions[i].optionA);
        printf("B. %s\n", questions[i].optionB);
        printf("C. %s\n", questions[i].optionC);

        while (1) { // Input validation loop
            printf("\nYour answer (A/B/C or Q to quit): ");
            scanf(" %c", &userAnswer);
            userAnswer = tolower(userAnswer);

            if (userAnswer == 'a' || userAnswer == 'b' || userAnswer == 'c' || userAnswer == 'q') {
                break; // Valid input
            } else {
                printf("Invalid choice. Please enter A, B, C, or Q.\n");
            }
        }

        if (userAnswer == 'q') {
            printf("\nYou chose to quit. Your progress will not be saved.\n");
            *quitFlag = 1;
            return 0;
        }

        if (tolower(questions[i].correctAnswer) == userAnswer) {
            printf("Correct!\n\n");
            correct++;
        } else {
            printf("Wrong! Correct answer: %c\n\n", questions[i].correctAnswer);
        }
    }

    printf("You have finished the %s level.\n", levelName);
    printf("Score for this level: %d/3\n\n", correct);
    pause2();
    return correct;
}

void playQuiz(const char *username, Question *easy, int eCount, Question *medium, int mCount, Question *hard, int hCount) {
    srand(time(NULL));
    shuffleQuestions(easy, eCount);
    shuffleQuestions(medium, mCount);
    shuffleQuestions(hard, hCount);

    int score = 0;
    int quitFlag = 0;

    clearConsole();
    printf("Welcome, %s! Let's start the quiz.\n\n", username);
    printf("|///////////////////////////////////////////////////////////|\n");
    printf("|                                                           |\n");
    printf("|            Get ready to challenge your brain!             |\n");
    printf("|                                                           |\n");
    printf("|      You can type 'Q' at any time to quit (your score     |\n");
    printf("|                  will not be saved).                      |\n");
    printf("|                                                           |\n");
    printf("|///////////////////////////////////////////////////////////|\n\n");
    pause();

    clock_t start = clock();

    score += askQuestions("EASY", easy, eCount, 1, &quitFlag);
    if (quitFlag) return;

    score += askQuestions("MEDIUM", medium, mCount, 4, &quitFlag);
    if (quitFlag) return;

    score += askQuestions("HARD", hard, hCount, 7, &quitFlag);
    if (quitFlag) return;

    clock_t end = clock();
    double timeTaken = (double)(end - start) / CLOCKS_PER_SEC;

    FILE *leaderboard = fopen("leaderboard.txt", "r");
    int highScore = -1;
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
        FILE *appendFile = fopen("leaderboard.txt", "a");
        if (appendFile) {
            fprintf(appendFile, "%s, %d, %.2f\n", username, score, timeTaken);
            fclose(appendFile);
            printf("\nNew score added to leaderboard!\n");
        } else {
            printf("\nCould not write to leaderboard.txt\n");
        }
    } else if (score > highScore || (score == highScore && timeTaken < bestTime)) {
        updateScore((char *)username, score, timeTaken);
        printf("\n\nHigh score updated on leaderboard!\n");
    } else {
        printf("\n\nYou did not beat your previous high score of %d.\n", highScore);
    }

    printf("\n\n==============================================================\n");
    printf("QUIZ COMPLETE! \n\n");
    printf("CORRECT ANSWERS: %d out of 9\n", score);
    printf("TIME TAKEN: %.2f seconds\n", timeTaken);
    printf("==============================================================\n");
    pause();
}

#endif
