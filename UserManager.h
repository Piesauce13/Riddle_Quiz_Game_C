#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>

#define MAX_USERS 100
#define MAX_LEN 50
#define SALT_LEN 16
#define HASH_LEN 65
#define USER_FILE "/Users/chanpanha/Documents/Riddle_Quiz_Game_C_1/users_data.txt"

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

void editProfile(User *users, int *userCount, char *username);
void changeUsername(User *users, int *userCount, char *currentUsername);
void changePassword(User *users, int *userCount, char *username);
int findUserIndex(User *users, int userCount, const char *username);
extern User users[MAX_USERS];
extern int userCount;
extern void showWelcomeScreen(User *users, int *userCount);

// Generate a random salt
void generateSalt(char *salt) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < SALT_LEN; i++) {
        salt[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    salt[SALT_LEN] = '\0';
}

// Hash (salt + password) using SHA-256
void hashPasswordWithSalt(const char *password, const char *salt, char *outputHash) {
    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", salt, password);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)combined, strlen(combined), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(outputHash + (i * 2), "%02x", hash[i]);
    }
    outputHash[64] = '\0';
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

void editProfile(User *users, int *userCount, char *username) {
    int choice;
    while (1) {
        printf("\n\n                                Edit Profile                                \n");
        printf("1. Change Username\n");
        printf("2. Change Password\n");
        printf("3. Back to Main Menu\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1:
                changeUsername(users, userCount, username);
                return; // force relogin
            case 2:
                changePassword(users, userCount, username);
                return; // force relogin
            case 3:
                printf("Returning to main menu...\n");
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void changeUsername(User *users, int *userCount, char *currentUsername) {
    char newUsername[MAX_LEN];
    char password[MAX_LEN];
    char computedHash[HASH_LEN];

    printf("Enter new username: ");
    scanf("%s", newUsername);
    getchar();

    // Check for existing username or same as current
    if (strcmp(currentUsername, newUsername) == 0) {
        printf("New username cannot be the same as the current username.\n");
        return;
    }
    for (int i = 0; i < *userCount; i++) {
        if (strcmp(users[i].username, newUsername) == 0) {
            printf("Username already exists. Try again.\n");
            return;
        }
    }

    int index = findUserIndex(users, *userCount, currentUsername);
    if (index == -1) {
        printf("User not found.\n");
        return;
    }

    // Ask for password to verify
    printf("Enter your password to verify: ");
    scanf("%s", password);
    getchar();

    hashPasswordWithSalt(password, users[index].salt, computedHash);
    if (strcmp(users[index].passwordHash, computedHash) != 0) {
        printf("Incorrect password. Username change aborted.\n");
        return;
    }

    strcpy(users[index].username, newUsername);
    saveUsers(users, *userCount);
    printf("Username changed successfully to: %s\n", newUsername);
    printf("Please login again with your new username.\n");
    showWelcomeScreen(users, userCount);
}

void changePassword(User *users, int *userCount, char *username) {
    char currentPassword[MAX_LEN], newPassword[MAX_LEN], confirmPassword[MAX_LEN];
    char computedHash[HASH_LEN], newSalt[SALT_LEN + 1], newHash[HASH_LEN];

    int index = findUserIndex(users, *userCount, username);
    if (index == -1) {
        printf("User not found.\n");
        return;
    }

    while (1) {
        printf("Enter current password: ");
        scanf("%s", currentPassword);
        getchar();

        hashPasswordWithSalt(currentPassword, users[index].salt, computedHash);
        if (strcmp(users[index].passwordHash, computedHash) != 0) {
            printf("Incorrect password. Try again.\n");
            continue;
        }

        printf("Enter new password: ");
        scanf("%s", newPassword);
        getchar();

        if (strcmp(newPassword, currentPassword) == 0) {
            printf("New password cannot be same as current. Try again.\n");
            continue;
        }

        printf("Confirm new password: ");
        scanf("%s", confirmPassword);
        getchar();

        if (strcmp(newPassword, confirmPassword) != 0) {
            printf("Passwords do not match. Try again.\n");
            continue;
        }

        generateSalt(newSalt);
        hashPasswordWithSalt(newPassword, newSalt, newHash);

        strcpy(users[index].salt, newSalt);
        strcpy(users[index].passwordHash, newHash);
        saveUsers(users, *userCount);
        printf("Password changed successfully.\n");
        printf("Please log in again with your new password.\n");
        showWelcomeScreen(users, userCount);
        return;
    }
}

int findUserIndex(User *users, int userCount, const char *username) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

void showWelcomeScreen(User *users, int *userCount);


