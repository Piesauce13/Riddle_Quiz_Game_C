#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Preprocessor
#define MAX_PLAYERS 100
#define USERNAME_LEN 50
#define TIMESTAMP_LEN 30

// Function Prototypes
void getCurrentTimestamp(char* buffer);
int loadLeaderboard();
void saveLeaderboard();
void saveLeaderboard();
void updateScore(const char* username, int newScore, const char* timestamp);
void updateUsername(const char* oldUsername, const char* newUsername);
int compareScores(const void* a, const void* b);
void showLeaderboard(); 
void updateScore(const char* username, int newScore, const char* timestamp);

// Struct to hold leaderboard data
typedef struct {
    char username[USERNAME_LEN];
    int score;
    char timestamp[TIMESTAMP_LEN];
} Player;

Player players[MAX_PLAYERS];
int playerCount = 0;

// Helper function to get timestamp
void getCurrentTimestamp(char* buffer) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, TIMESTAMP_LEN, "%Y-%m-%d %H:%M:%S", t);
}

// Load leaderboard from file
int loadLeaderboard() {
    FILE* file = fopen("leaderboard.txt", "r");
    if (!file) return 0;

    playerCount = 0;
    while (fscanf(file, "%49[^,],%d,%29[^\n]\n",
                  players[playerCount].username,
                  &players[playerCount].score,
                  players[playerCount].timestamp) == 3) {
        playerCount++;
        if (playerCount >= MAX_PLAYERS) break;
    }
    fclose(file);
    return playerCount;
}

// Save leaderboard to file
void saveLeaderboard() {
    FILE* file = fopen("leaderboard.txt", "w");
    if (!file) return;
    for (int i = 0; i < playerCount; i++) {
        fprintf(file, "%s,%d,%s\n",
                players[i].username,
                players[i].score,
                players[i].timestamp);
    }
    fclose(file);
}

// Update score if new score is better, or add user if not found
void updateScore(const char* username, int newScore, const char* timestamp) {
    int found = 0;
    for (int i = 0; i < playerCount; i++) {
        if (strcmp(players[i].username, username) == 0) {
            if (newScore > players[i].score) {
                players[i].score = newScore;
                strncpy(players[i].timestamp, timestamp, TIMESTAMP_LEN);
            }
            found = 1;
            break;
        }
    }
    if (!found && playerCount < MAX_PLAYERS) {
        strncpy(players[playerCount].username, username, USERNAME_LEN);
        players[playerCount].score = newScore;
        strncpy(players[playerCount].timestamp, timestamp, TIMESTAMP_LEN);
        playerCount++;
    }
    saveLeaderboard();
}

// Update username in leaderboard
void updateUsername(const char* oldUsername, const char* newUsername) {
    for (int i = 0; i < playerCount; i++) {
        if (strcmp(players[i].username, oldUsername) == 0) {
            strncpy(players[i].username, newUsername, USERNAME_LEN);
            break;
        }
    }
    saveLeaderboard();
}

// Comparator for sorting leaderboard
int compareScores(const void* a, const void* b) {
    Player* p1 = (Player*)a;
    Player* p2 = (Player*)b;
    return p2->score - p1->score;
}

// Show leaderboard sorted by scor
void showLeaderboard() {
    loadLeaderboard();
    qsort(players, playerCount, sizeof(Player), compareScores); //sort
    printf("\n===== Leaderboard =====\n");
    for (int i = 0; i < playerCount && i < 10; i++) {
        printf("%d. %s - %d points (%s)\n",
               i + 1,
               players[i].username,
               players[i].score,
               players[i].timestamp);
    }
    printf("========================\n");
}

// Update leaderboard after quiz
void updateLeaderboard(const char* username, int score, const char* timestamp) {
    loadLeaderboard();
    updateScore(username, score, timestamp);
}

/*/
#define MAX_USERS 100

User users[MAX_USERS];
int userCount = 0;

int usernameExists(const char* username) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return 1; // Username exists
        }
    }
    return 0; // Username does not exist
}


int changeUsername(const char* oldUsername, const char* newUsername) {
    // 1. Validate newUsername doesn't already exist
    if (usernameExists(newUsername)) {
        return 0; // failure: username taken
    }

    // 2. Find the user in your user system and update it
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, oldUsername) == 0) {
            strncpy(users[i].username, newUsername, USERNAME_LEN);

            // 3. Reflect chasaveUsers()nge in the leaderboard
            updateUsername(oldUsername, newUsername);

            saveUsers(users, userCount); // Your existing function to save user data
            return 1; // success
        }
    }
    return 0; // failure: old username not found
}
/*/