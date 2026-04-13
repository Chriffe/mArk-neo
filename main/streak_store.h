/**
 * Streak store — NVS persistence with WAL safety (ESP-IDF)
 * Supports up to MAX_USERS independent streaks via per-user NVS namespaces.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const char *name;
    int threshold;
} streak_level_t;

typedef struct {
    int32_t streak;
    int32_t last_year;
    int32_t last_month;
    int32_t last_day;
} streak_data_t;

/* Always holds the active user's streak — read by UI and completion logic */
extern streak_data_t streak_data;

/**
 * Load the active user's streak from NVS (with WAL recovery).
 * Call once at boot after user_store_init().
 */
void streak_store_init(void);

/**
 * Switch the active user: saves the current user's streak, then loads
 * the new user's streak into streak_data.
 * Call this whenever active_user changes.
 */
void streak_set_active_user(int user_idx);

/** Mark today complete for the current active user and persist. */
void streak_mark_day_complete(int day_offset);

const streak_level_t *streak_get_level(void);
const streak_level_t *streak_get_next_level(void);
int streak_get_progress_to_next(void);

/* Read any user's streak from NVS without affecting the active user */
void streak_read_user(int user_idx, streak_data_t *out);
const char *streak_level_for(int32_t streak);
