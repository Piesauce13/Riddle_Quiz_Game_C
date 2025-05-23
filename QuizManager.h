
#ifndef QUIZ_MANAGER_H
#define QUIZ_MANAGER_H

#define MAX_QUESTIONS 10
#define MAX_LINE_LENGTH 256
#include "Leaderboard.h"

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

void playQuiz(const char *username, Question *easy, int eCount, Question *medium, int mCount, Question *hard, int hCount) {
    srand(time(NULL));
    Question selected[9];
    int index = 0;

    // Shuffle each difficulty group
    shuffleQuestions(easy, eCount);
    shuffleQuestions(medium, mCount);
    shuffleQuestions(hard, hCount);

    // Pick first 3 from each group
    for (int i = 0; i < 3 && i < eCount; i++) selected[index++] = easy[i];
    for (int i = 0; i < 3 && i < mCount; i++) selected[index++] = medium[i];
    for (int i = 0; i < 3 && i < hCount; i++) selected[index++] = hard[i];

    int score = 0;
    char userAnswer;
    time_t start = time(NULL);

    printf("Welcome, %s! Let's start the quiz.\n\n", username);

    for (int i = 0; i < index; i++) {
        printf("Q%d: %s\n", i + 1, selected[i].question);
        printf("A. %s\n", selected[i].optionA);
        printf("B. %s\n", selected[i].optionB);
        printf("C. %s\n", selected[i].optionC);
        printf("Your answer (A/B/C): ");
        scanf(" %c", &userAnswer);

        if (userAnswer == selected[i].correctAnswer || userAnswer == selected[i].correctAnswer + 32) {
            printf("Correct!\n\n");
            score++;
        } else {
            printf("Wrong! Correct answer: %c\n\n", selected[i].correctAnswer);
        }
    }

    time_t end = time(NULL);
    double timeTaken = difftime(end, start); // returns double in seconds

    // Write to leaderboard
    FILE *leaderboard = fopen("leaderboard.txt", "a");
    if (leaderboard) {
        fprintf(leaderboard, "%s, %d, %.2f \n", username, score, timeTaken);
        fclose(leaderboard);
    } else {
        printf("Could not write to leaderboard.txt\n");
    }

    printf("Quiz complete! You got %d out of 9 correct.\n", score);
    printf("Time taken: %.2f seconds\n", timeTaken);
}

#endif //QUIZMANAGER_H

