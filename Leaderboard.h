#ifndef LEADERBOARD_H
#define LEADERBOARD_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern char username[50];
extern int score;
extern double timeTaken;

void loadLeaderboard();
void saveLeaderboard();
void updateScore(char* username, int newScore, const char* newTimeTaken);
void updateUsernameInLeaderboard(const char* newUsername, int score, double timeTaken);
void showLeaderboard();
void showUserHighscore();
int compareScores();
void updateLeaderbaord();
void deleteUserFromLeaderboard(const char *username);

void loadLeaderboard() {
    FILE *leaderboard = fopen("leaderboard.txt", "r");
    if (!leaderboard) {
        printf("Error: Could not open leaderboard file.\n");
        return;
    }
    fclose(leaderboard);
}

void saveLeaderboard() {
    FILE *leaderboard = fopen("leaderboard.txt", "a+");
    if (!leaderboard) {
        printf("Error: Could not open leaderboard file.\n");
        return;
    }
    fprintf(leaderboard, "%s, %d, %.2f\n", username, score, timeTaken);
    fclose(leaderboard);
}

typedef struct {
    char username[50];
    int score;
    double timeTaken;
} LeaderboardEntry;

#define MAX_LEADERBOARD_ENTRIES 100
#define USERNAME_LEN 50

void updateScore(char* username, int newScore, double newTimeTaken) {
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    int count = 0;

    FILE *file = fopen("leaderboard.txt", "r");
    if (!file) {
        printf("Failed to open leaderboard.txt for reading.\n");
        return;
    }

    // Load entries
    while (fscanf(file, "%49[^,], %d, %lf\n",
                  leaderboard[count].username,
                  &leaderboard[count].score,
                  &leaderboard[count].timeTaken) == 3) {
        count++;
                  }
    fclose(file);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(leaderboard[i].username, username) == 0) {
            leaderboard[i].score = newScore;
            leaderboard[i].timeTaken = newTimeTaken;
            found = 1;
        }
    }

    if (!found) {
        printf("Username not found in leaderboard.\n");
        return;
    }

    file = fopen("leaderboard.txt", "w");
    if (!file) {
        printf("Failed to open leaderboard.txt for writing.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s, %d, %.2f\n",
                leaderboard[i].username,
                leaderboard[i].score,
                leaderboard[i].timeTaken);
    }

    fclose(file);
}


void updateUsernameInLeaderboard(const char *oldUsername, const char *newUsername) {
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    int count = 0;

    FILE *file = fopen("leaderboard.txt", "r");
    if (!file) {
        printf("Failed to open leaderboard.txt for reading.\n");
        return;
    }

    while (fscanf(file, "%49[^,], %d, %lf\n",
                  leaderboard[count].username,
                  &leaderboard[count].score,
                  &leaderboard[count].timeTaken) == 3) {
        count++;
        if (count >= MAX_LEADERBOARD_ENTRIES) {
            break;
        }
                  }
    fclose(file);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(leaderboard[i].username, oldUsername) == 0) {
            strcpy(leaderboard[i].username, newUsername);
            found = 1;
        }
    }

    if (!found) {
        printf("\nUsername not found in leaderboard.\n");
        return;
    }

    file = fopen("leaderboard.txt", "w");
    if (!file) {
        printf("\nFailed to open leaderboard.txt for writing.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s, %d, %.2lf\n",
                leaderboard[i].username,
                leaderboard[i].score,
                leaderboard[i].timeTaken);
    }

    fclose(file);
    // printf("\nUsername updated in leaderboard successfully.\n");
}

int compareEntries(const void* a, const void* b) {
    LeaderboardEntry* entryA = (LeaderboardEntry*)a;
    LeaderboardEntry* entryB = (LeaderboardEntry*)b;

    if (entryA->score != entryB->score) {
        return entryB->score - entryA->score;
    }

    if (entryA->timeTaken < entryB->timeTaken) return -1;
    else if (entryA->timeTaken > entryB->timeTaken) return 1;
    else return 0;
}

void showLeaderboard() {
    FILE *leaderboard = fopen("leaderboard.txt", "r");
    if (!leaderboard) {
        printf("No leaderboard file found.\n");
        return;
    }

    LeaderboardEntry entries[100];
    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), leaderboard) && count < 100) {
        if (sscanf(line, "%49[^,], %d, %lf",
                  entries[count].username,
                  &entries[count].score,
                  &entries[count].timeTaken) == 3) {
            count++;
        }
    }

    fclose(leaderboard);

    qsort(entries, count, sizeof(LeaderboardEntry), compareEntries);
    printf("\n===== Leaderboard =====\n\n");
    printf("%-5s %-20s %-10s %-10s\n\n", "Rank", "Username", "Score", "Time");

    for (int i = 0; i < count && i < 10; i++) {
        printf("%-5d %-20s %-10d %-10.2f\n\n",
               i + 1,
               entries[i].username,
               entries[i].score,
               entries[i].timeTaken);
    }
}

void showUserHighscore() {
    FILE *leaderboard = fopen("leaderboard.txt", "r");
    if (!leaderboard) {
        printf("No leaderboard file found.\n");
        return;
    }

    char line[256];
    char fileUsername[50];
    int fileScore;
    double fileTime;
    int found = 0;

    while (fgets(line, sizeof(line), leaderboard)) {
        if (sscanf(line, "%49[^,], %d, %lf", fileUsername, &fileScore, &fileTime) == 3) {
            if (strcmp(fileUsername, username) == 0) {
                found = 1;
                printf("\nYour Highscore:\n");
                printf("Username : %s\n", fileUsername);
                printf("Score    : %d\n", fileScore);
                printf("Time     : %.2f seconds\n", fileTime);
                break;
            }
        }
    }

    fclose(leaderboard);

    if (!found) {
        printf("\nYou don't have a highscore yet. Play now to set your record!\n");
    }
}

#include "Leaderboard.h"

void deleteUserFromLeaderboard(const char *username) {
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    int count = 0;

    FILE *file = fopen("leaderboard.txt", "r");
    if (!file) {
        printf("Failed to open leaderboard.txt for reading.\n");
        return;
    }

    // Load into memory
    while (fscanf(file, "%49[^,], %d, %lf\n",
                  leaderboard[count].username,
                  &leaderboard[count].score,
                  &leaderboard[count].timeTaken) == 3) {
        count++;
                  }
    fclose(file);

    // Filter out user's entries
    int newCount = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(leaderboard[i].username, username) != 0) {
            leaderboard[newCount++] = leaderboard[i];
        }
    }

    // Write back
    file = fopen("leaderboard.txt", "w");
    if (!file) {
        printf("Failed to open leaderboard.txt for writing.\n");
        return;
    }
    for (int i = 0; i < newCount; i++) {
        fprintf(file, "%s, %d, %.2lf\n",
                leaderboard[i].username,
                leaderboard[i].score,
                leaderboard[i].timeTaken);
    }
    fclose(file);

    printf("Deleted %s's entries from leaderboard.\n", username);
}


#endif

