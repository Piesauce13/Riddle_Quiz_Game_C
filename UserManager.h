#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <wincrypt.h>
#include <time.h>

#define MAX_USERS 100
#define MAX_LEN 50
#define SALT_LEN 16
#define HASH_LEN 65
#define USER_FILE "users_data.txt" // Use relative path for Windows portability

typedef struct {
    char username[MAX_LEN];
    char salt[SALT_LEN + 1];
    char passwordHash[HASH_LEN];
} User;

// Function Prototypes
void generateSalt(char *salt);
void hashPasswordWithSalt(const char *password, const char *salt, char *outputHash);
void saveUsers(User *users, int userCount);
int loadUsers(User *users);
void signUp(User *users, int *userCount);
int login(User *users, int userCount, char *username);
void editProfile(User *users, int *userCount, char *currentUsername);

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

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        printf("Error acquiring crypt context: %ld\n", GetLastError());
        strcpy(outputHash, ""); // fallback to empty string
        return;
    }

    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        printf("Error creating hash: %ld\n", GetLastError());
        CryptReleaseContext(hProv, 0);
        strcpy(outputHash, "");
        return;
    }

    if (!CryptHashData(hHash, (BYTE*)combined, (DWORD)strlen(combined), 0)) {
        printf("Error hashing data: %ld\n", GetLastError());
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        strcpy(outputHash, "");
        return;
    }

    if (!CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0)) {
        printf("Error getting hash param: %ld\n", GetLastError());
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        strcpy(outputHash, "");
        return;
    }

    for (DWORD i = 0; i < hashLen; i++) {
        sprintf(outputHash + (i * 2), "%02x", hash[i]);
    }
    outputHash[hashLen * 2] = '\0';

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
}

// Save users to file
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

// Load users from file
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

// Sign up function
void signUp(User *users, int *userCount) {
    if (*userCount >= MAX_USERS) {
        printf("Maximum number of users reached.\n");
        return;
    }

    char username[MAX_LEN], password[MAX_LEN];
    char salt[SALT_LEN + 1], passwordHash[HASH_LEN];

    printf("Create a username: ");
    scanf("%s", username);
    printf("Create a password: ");
    scanf("%s", password);
    getchar(); // consume newline

    // Check for existing username
    for (int i = 0; i < *userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Username already exists. Try again.\n");
            return;
        }
    }

    generateSalt(salt);
    hashPasswordWithSalt(password, salt, passwordHash);

    strcpy(users[*userCount].username, username);
    strcpy(users[*userCount].salt, salt);
    strcpy(users[*userCount].passwordHash, passwordHash);
    (*userCount)++;

    saveUsers(users, *userCount);
    printf("Account created successfully! You can now log in.\n");
}

// Login function
int login(User *users, int userCount, char *username) {
    char inputUsername[MAX_LEN], inputPassword[MAX_LEN];
    char computedHash[HASH_LEN];

    printf("Username: ");
    scanf("%s", inputUsername);
    printf("Password: ");
    scanf("%s", inputPassword);
    getchar(); // consume newline

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, inputUsername) == 0) {
            hashPasswordWithSalt(inputPassword, users[i].salt, computedHash);

            if (strcmp(users[i].passwordHash, computedHash) == 0) {
                strcpy(username, inputUsername);
                printf("Login successful! Welcome, %s.\n", username);
                return 1;
            }
        }
    }

    printf("Incorrect username or password.\n");
    return 0;
}

// Now add this function definition (after login)
void editProfile(User *users, int *userCount, char *currentUsername) {
    int choice;
    char newUsername[MAX_LEN];
    char newPassword[MAX_LEN];
    char newSalt[SALT_LEN + 1];
    char newHash[HASH_LEN];

    int userIndex = -1;
    for (int i = 0; i < *userCount; i++) {
        if (strcmp(users[i].username, currentUsername) == 0) {
            userIndex = i;
            break;
        }
    }

    if (userIndex == -1) {
        printf("User not found.\n");
        return;
    }

    while (1) {
        printf("\nEdit Profile Menu\n");
        printf("1. Change Username\n");
        printf("2. Change Password\n");
        printf("3. Back to Main Menu\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        if (choice == 1) {
            printf("Enter new username: ");
            scanf("%s", newUsername);
            getchar(); // consume newline

            // Check if new username already exists
            int exists = 0;
            for (int i = 0; i < *userCount; i++) {
                if (strcmp(users[i].username, newUsername) == 0) {
                    exists = 1;
                    break;
                }
            }

            if (exists) {
                printf("Username already exists. Try another one.\n");
            } else {
                strcpy(users[userIndex].username, newUsername);
                strcpy(currentUsername, newUsername); // Update current session
                saveUsers(users, *userCount);
                printf("Username updated successfully!\n");
            }

        } else if (choice == 2) {
            printf("Enter new password: ");
            scanf("%s", newPassword);
            getchar(); // consume newline

            generateSalt(newSalt);
            hashPasswordWithSalt(newPassword, newSalt, newHash);

            strcpy(users[userIndex].salt, newSalt);
            strcpy(users[userIndex].passwordHash, newHash);

            saveUsers(users, *userCount);
            printf("Password updated successfully!\n");

        } else if (choice == 3) {
            printf("Returning to Main Menu...\n");
            return;
        } else {
            printf("Invalid choice. Try again.\n");
        }
    }
}


#endif