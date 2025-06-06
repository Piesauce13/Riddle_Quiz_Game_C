#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <wincrypt.h>
#include <conio.h>
#include "Leaderboard.h"
#include "QuizManager.h"

#define MAX_USERS 100
#define MAX_LEN 50
#define SALT_LEN 16
#define HASH_LEN 65
#define USER_FILE "users_data.txt"

typedef struct {
    char username[MAX_LEN];
    char salt[SALT_LEN + 1];
    char passwordHash[HASH_LEN];
} User;

void toLowerCase(char *str);
void generateSalt(char *salt);
void hashPasswordWithSalt(const char *password, const char *salt, char *outputHash);
void saveUsers(User *users, int userCount);
int loadUsers(User *users);
void signUp(User *users, int *userCount);
int login(User *users, int userCount, char *username);
void editProfile(User *users, int *userCount, char *currentUsername);
int deleteUserAccount(User *users, int *userCount, const char *currentUsername);

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Generate a random salt
void generateSalt(char *salt) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < SALT_LEN; i++) {
        salt[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    salt[SALT_LEN] = '\0';
}

// Hash (salt + password) using Windows CryptoAPI SHA-256
void hashPasswordWithSalt(const char *password, const char *salt, char *outputHash) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE hash[32]; // SHA-256 produces 32 bytes
    DWORD hashLen = sizeof(hash);

    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", salt, password);

    CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);

    CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash);

    CryptHashData(hHash, (BYTE*)combined, (DWORD)strlen(combined), 0);

    CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0);

    for (DWORD i = 0; i < hashLen; i++) {
        sprintf(outputHash + (i * 2), "%02x", hash[i]);
    }
    outputHash[hashLen * 2] = '\0';
}

void saveUsers(User *users, int userCount) {
    FILE *file = fopen(USER_FILE, "w");
    if (!file) {
        perror("Failed to open user file for saving");
        return;
    }

    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s:%s:%s\n",
                users[i].username,
                users[i].salt,
                users[i].passwordHash);
        fflush(file);
    }

    fclose(file);
}

int loadUsers(User *users) {
    FILE *file = fopen(USER_FILE, "r");
    if (!file) return 0;

    int count = 0;
    char line[256];

    while (count < MAX_USERS && fgets(line, sizeof(line), file)) {
        char *username = strtok(line, ":");
        char *salt = strtok(NULL, ":");
        char *hash = strtok(NULL, "\n");

        if (username && salt && hash) {
            strcpy(users[count].username, username);
            strcpy(users[count].salt, salt);
            strcpy(users[count].passwordHash, hash);
            count++;
        }
    }

    fclose(file);
    return count;
}

void getPassword(char *password, size_t maxLength) {
    int i = 0;
    char ch;
    while (i < maxLength - 1 && (ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
}

void signUp(User *users, int *userCount) {
    if (*userCount >= MAX_USERS) {
        printf("Maximum number of users reached.\n");
        return;
    }

    char username[MAX_LEN], password[MAX_LEN], confirmPassword[MAX_LEN], salt[SALT_LEN + 1], passwordHash[HASH_LEN];

    printf("\nCreate a username: ");
    scanf("%s", username);
    getchar();

    // Check if username already exists
    for (int i = 0; i < *userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("\nUsername already exists. Try again.\n");
            pause();
            return;
        }
    }

    // Password entry and confirmation loop
    while (1) {
        printf("\nCreate a password: ");
        getPassword(password, sizeof(password));

        printf("\nConfirm your password: ");
        getPassword(confirmPassword, sizeof(confirmPassword));

        if (strcmp(password, confirmPassword) == 0) {
            break;
        } else {
            printf("\nPasswords do not match. Please try again.\n");
        }
    }

    generateSalt(salt);
    hashPasswordWithSalt(password, salt, passwordHash);

    strcpy(users[*userCount].username, username);
    strcpy(users[*userCount].salt, salt);
    strcpy(users[*userCount].passwordHash, passwordHash);
    (*userCount)++;

    saveUsers(users, *userCount);
    printf("\nAccount created successfully!\n");
    pause();
}

int login(User *users, int userCount, char *username) {
    char inputUsername[MAX_LEN], inputPassword[MAX_LEN], computedHash[HASH_LEN];

    printf("\nUsername: ");
    scanf("%s", inputUsername);
    getchar(); // consume newline
    printf("\nPassword: ");
    getPassword(inputPassword, sizeof(inputPassword));

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, inputUsername) == 0) {
            hashPasswordWithSalt(inputPassword, users[i].salt, computedHash);
            if (strcmp(users[i].passwordHash, computedHash) == 0) {
                strcpy(username, inputUsername);
                printf("\nLogin successful! Welcome, %s.\n", username);
                pause();

                return 1;
            }
        }
    }
    printf("\nIncorrect username or password.\n");
    pause();
    return 0;
}

void editProfile(User *users, int *userCount, char *currentUsername) {
    int choice;
    char newUsername[MAX_LEN], newPassword[MAX_LEN], confirmPassword[MAX_LEN];
    char currentPassword[MAX_LEN];
    char newSalt[SALT_LEN + 1], newHash[HASH_LEN];

    int userIndex = -1;
    for (int i = 0; i < *userCount; i++) {
        if (strcmp(users[i].username, currentUsername) == 0) {
            userIndex = i;
            break;
        }
    }
    if (userIndex == -1) {
        printf("\nUser not found.\n");
        return;
    }

    while (1) {
        printf("\nEdit Profile Menu\n\n");
        printf("1. Change Username\n");
        printf("2. Change Password\n");
        printf("3. Back to Main Menu\n\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        if (choice == 1) {
            printf("\nEnter new username: ");
            scanf("%s", newUsername);
            getchar(); // consume newline

            int exists = 0;
            for (int i = 0; i < *userCount; i++) {
                if (strcmp(users[i].username, newUsername) == 0) {
                    exists = 1;
                    break;
                }
            }
            if (exists) {
                printf("\nUsername already exists. Try another.\n");
            } else {
                updateUsernameInLeaderboard(currentUsername, newUsername);
                strcpy(users[userIndex].username, newUsername);
                strcpy(currentUsername, newUsername);
                saveUsers(users, *userCount);
                printf("\nUsername updated successfully!\n");
            }
        } else if (choice == 2) {
            printf("\nEnter current password: ");
            getPassword(currentPassword, sizeof(currentPassword));

            char verifyHash[HASH_LEN];
            hashPasswordWithSalt(currentPassword, users[userIndex].salt, verifyHash);

            if (strcmp(verifyHash, users[userIndex].passwordHash) != 0) {
                printf("\nIncorrect current password. Cannot change password.\n");
                continue;
            }

            printf("\nEnter new password: ");
            getPassword(newPassword, sizeof(newPassword));

            printf("\nConfirm new password: ");
            getPassword(confirmPassword, sizeof(confirmPassword));

            if (strcmp(newPassword, confirmPassword) != 0) {
                printf("\nPasswords do not match. Try again.\n");
                continue;
            }

            generateSalt(newSalt);
            hashPasswordWithSalt(newPassword, newSalt, newHash);
            strcpy(users[userIndex].salt, newSalt);
            strcpy(users[userIndex].passwordHash, newHash);
            saveUsers(users, *userCount);
            printf("\nPassword updated successfully!\n");
        } else if (choice == 3) {
            return;
        } else {
            printf("\nInvalid choice. Try again.\n");
        }
    }
}

int deleteUserAccount(User *users, int *userCount, const char *currentUsername) {
    char confirmation[10];
    char password[MAX_LEN];
    int userIndex = -1;

    // Find the user index
    for (int i = 0; i < *userCount; i++) {
        if (strcmp(users[i].username, currentUsername) == 0) {
            userIndex = i;
            break;
        }
    }
    if (userIndex == -1) {
        printf("\nUser not found.\n");
        return 0;
    }

    while (1) {
        printf("\nAre you sure you want to delete your account? (yes/no): ");
        scanf("%s", confirmation);
        getchar();

        toLowerCase(confirmation);

        if (strcmp(confirmation, "yes") == 0 || strcmp(confirmation, "y") == 0) {
            break;
        } else if (strcmp(confirmation, "no") == 0 || strcmp(confirmation, "n") == 0) {
            printf("\nAccount deletion cancelled.\n");
            return 0;
        } else {
            printf("Invalid input. Please enter 'yes' or 'no'.\n");
        }
    }


    printf("\nEnter your password to confirm deletion: ");
    getPassword(password, sizeof(password));

    char verifyHash[HASH_LEN];
    hashPasswordWithSalt(password, users[userIndex].salt, verifyHash);

    if (strcmp(verifyHash, users[userIndex].passwordHash) != 0) {
        printf("\nIncorrect password. Account deletion cancelled.\n");
        return 0;
    }

    // Delete user from array
    for (int i = userIndex; i < *userCount - 1; i++) {
        users[i] = users[i + 1];
    }
    (*userCount)--;

    // Save updated users
    saveUsers(users, *userCount);
    printf("\nAccount deleted successfully.\n");
    deleteUserFromLeaderboard(currentUsername);

    return 1; // Signal success
}



#endif