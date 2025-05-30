#ifndef LEADERBOARD_H
#define LEADERBOARD_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Global variables
extern char username[50];
extern int score;
extern double timeTaken;

// Function prototypes
void loadLeaderboard();
void saveLeaderboard();
void updateScore(char* username, int newScore, const char* newTimeTaken);
void updateUsername(const char* newUsername, int score, const char* timeTaken);
void showLeaderboard();
int compareScores(); // this is to compare score, and write the highest score in (idk if we need or not)
void updateLeaderbaord(); // idk if we gonna need this or not.

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

void updateScore(char* username, int newScore, const char* newTimeTaken) {
    FILE *leaderboard = fopen("leaderboard.txt", "r");
    if (!leaderboard) {
        printf("Failed to open leaderboard.txt for reading.\n");
        return;
    }

    FILE *tmpfile = fopen("temp.txt", "w");
    if (!tmpfile) {
        printf("Failed to open temp.txt for writing.\n");
        fclose(leaderboard);
        return;
    }

    char buffer[256];
    char fileUsername[50];
    int fileScore;
    double fileTime;

    while (fgets(buffer, sizeof(buffer), leaderboard)) {
        // Parse the line: expected format is "username, score, time"
        if (sscanf(buffer, "%49[^,], %d, %lf", fileUsername, &fileScore, &fileTime) == 3) {
            if (strcmp(fileUsername, username) == 0) {
                // Match found — update the line
                fprintf(tmpfile, "%s, %d, %s\n", username, newScore, newTimeTaken);
            } else {
                // Not the target user — keep the line as is
                fputs(buffer, tmpfile);
            }
        } else {
            // Line format not matched — copy as is
            fputs(buffer, tmpfile);
        }
    }

    fclose(leaderboard);
    fclose(tmpfile);

    // Replace the original file with updated content
    remove("leaderboard.txt");
    rename("temp.txt", "leaderboard.txt");
}

void updateUsername(const char* newUsername, int score, const char* timeTaken) {
    FILE *leaderboard = fopen("leaderboard.txt", "r");
    if (!leaderboard) {
        printf("Failed to open leaderboard.txt for reading.\n");
        return;
    }

    FILE *tmpfile = fopen("temp.txt", "w");
    if (!tmpfile) {
        printf("Failed to open temp.txt for writing.\n");
        fclose(leaderboard);
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), leaderboard)) {
        char oldLine[256];
        strcpy(oldLine, buffer); // Backup original line

        // If the line contains the current username, replace it
        if (strstr(buffer, username)) {
            fprintf(tmpfile, "%s, %d, %s\n", newUsername, score, timeTaken);
        } else {
            fputs(oldLine, tmpfile); // Copy unchanged line
        }
    }

    fclose(leaderboard);
    fclose(tmpfile);

    // Replace original file with updated file
    remove("leaderboard.txt");
    rename("temp.txt", "leaderboard.txt");

    // Update global username variable
    strcpy(username, newUsername);
}

typedef struct {
    char username[50];
    int score;
    double timeTaken;
} LeaderboardEntry;

int compareEntries(const void* a, const void* b) {
    LeaderboardEntry* entryA = (LeaderboardEntry*)a;
    LeaderboardEntry* entryB = (LeaderboardEntry*)b;

    // First, sort by score (descending)
    if (entryA->score != entryB->score) {
        return entryB->score - entryA->score;
    }


    // Then, sort by timeTaken (ascending)
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

    LeaderboardEntry entries[100]; // max 100 users
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

    // Sort the entries
    qsort(entries, count, sizeof(LeaderboardEntry), compareEntries);

    // Print the sorted leaderboard
    printf("\n===== Leaderboard =====\n");
    printf("%-5s %-20s %-10s %-10s\n", "Rank", "Username", "Score", "Time");

    for (int i = 0; i < count && i < 10; i++) {
        printf("%-5d %-20s %-10d %-10.2f\n",
               i + 1,
               entries[i].username,
               entries[i].score,
               entries[i].timeTaken);
    }
}

#endif

