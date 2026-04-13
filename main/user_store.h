/**
 * User store — persist family member profiles to NVS.
 * Each user has a name and owns their own calendar sources and streak data.
 */
#pragma once

#include <stdbool.h>

#define MAX_USERS         6
#define MAX_USER_NAME_LEN 20

typedef struct {
    char name[MAX_USER_NAME_LEN];
} user_profile_t;

/* Live user list — always kept in RAM */
extern user_profile_t users[MAX_USERS];
extern int            user_count;   /* 1..MAX_USERS */
extern int            active_user;  /* 0..user_count-1 */

/**
 * Load user list from NVS.
 * If no data exists (first boot), creates a single default user "Person 1"
 * so the app works identically to the single-user version until a second
 * user is added.
 */
void user_store_init(void);

/** Persist current users[], user_count, and active_user to NVS. */
void user_store_save(void);

/**
 * Append a new user. Returns the new user's index, or -1 if already at MAX_USERS.
 * Caller must call user_store_save() afterwards.
 */
int user_store_add(const char *name);

/**
 * Remove user at idx. Shifts remaining users down.
 * If the removed user was active, active_user is clamped to 0.
 * Caller must call user_store_save() afterwards.
 */
void user_store_remove(int idx);

/**
 * Rename user at idx.
 * Caller must call user_store_save() afterwards.
 */
void user_store_rename(int idx, const char *name);
