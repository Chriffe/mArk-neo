/**
 * Streak store — NVS with WAL pattern (ESP-IDF)
 *
 * Per-user NVS namespaces (max 15 chars each):
 *   Main : u0_stk .. u5_stk
 *   WAL  : u0_wal .. u5_wal
 *
 * User 0 migration: on first read, if u0_stk has no data the legacy
 * "streak" namespace is checked so existing streaks are preserved.
 * Similarly u0_wal falls back to "streak_wal" during WAL recovery.
 */
#include "streak_store.h"
#include "user_store.h"

#include <string.h>
#include <inttypes.h>
#include <time.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char *TAG = "STREAK";

static const streak_level_t LEVELS[] = {
    { "Starter",      0   },
    { "Consistent",   5   },
    { "Dedicated",    15  },
    { "Unstoppable",  30  },
    { "Legend",       50  },
    { "Titan",        100 },
};
#define NUM_LEVELS (sizeof(LEVELS) / sizeof(LEVELS[0]))

streak_data_t streak_data = {0};

/* Tracks which user's data is currently loaded */
static int s_loaded_user = -1;

/* ── Namespace helpers ── */

static void ns_main(int idx, char *buf, size_t len) {
    snprintf(buf, len, "u%d_stk", idx);
}

static void ns_wal(int idx, char *buf, size_t len) {
    snprintf(buf, len, "u%d_wal", idx);
}

/* ── Internal load / save / WAL ── */

static void load_for_user(int idx) {
    char ns[16];
    ns_main(idx, ns, sizeof(ns));

    nvs_handle_t h;
    bool loaded = false;

    if (nvs_open(ns, NVS_READONLY, &h) == ESP_OK) {
        /* Check if namespace has any streak data */
        int32_t s = 0;
        if (nvs_get_i32(h, "streak", &s) == ESP_OK) {
            streak_data.streak     = s;
            nvs_get_i32(h, "lastY", &streak_data.last_year);
            nvs_get_i32(h, "lastM", &streak_data.last_month);
            nvs_get_i32(h, "lastD", &streak_data.last_day);
            loaded = true;
        }
        nvs_close(h);
    }

    /* User 0: fall back to legacy "streak" namespace on first run */
    if (!loaded && idx == 0) {
        if (nvs_open("streak", NVS_READONLY, &h) == ESP_OK) {
            nvs_get_i32(h, "streak", &streak_data.streak);
            nvs_get_i32(h, "lastY",  &streak_data.last_year);
            nvs_get_i32(h, "lastM",  &streak_data.last_month);
            nvs_get_i32(h, "lastD",  &streak_data.last_day);
            nvs_close(h);
            ESP_LOGI(TAG, "User 0: migrated streak from legacy namespace");
        }
    }
}

static void save_with_wal_for_user(int idx) {
    char ns_m[16], ns_w[16];
    ns_main(idx, ns_m, sizeof(ns_m));
    ns_wal(idx,  ns_w, sizeof(ns_w));

    /* 1. Write WAL */
    nvs_handle_t wal;
    if (nvs_open(ns_w, NVS_READWRITE, &wal) == ESP_OK) {
        nvs_set_i32(wal, "streak", streak_data.streak);
        nvs_set_i32(wal, "lastY",  streak_data.last_year);
        nvs_set_i32(wal, "lastM",  streak_data.last_month);
        nvs_set_i32(wal, "lastD",  streak_data.last_day);
        nvs_set_u8(wal,  "valid",  1);
        nvs_commit(wal);
        nvs_close(wal);
    }

    /* 2. Write main */
    nvs_handle_t main_h;
    if (nvs_open(ns_m, NVS_READWRITE, &main_h) == ESP_OK) {
        nvs_set_i32(main_h, "streak", streak_data.streak);
        nvs_set_i32(main_h, "lastY",  streak_data.last_year);
        nvs_set_i32(main_h, "lastM",  streak_data.last_month);
        nvs_set_i32(main_h, "lastD",  streak_data.last_day);
        nvs_commit(main_h);
        nvs_close(main_h);
    }

    /* 3. Clear WAL */
    if (nvs_open(ns_w, NVS_READWRITE, &wal) == ESP_OK) {
        nvs_erase_all(wal);
        nvs_commit(wal);
        nvs_close(wal);
    }

    ESP_LOGI(TAG, "User %d saved (WAL-safe) streak=%" PRId32, idx, streak_data.streak);
}

static void recover_wal_for_user(int idx) {
    char ns_m[16], ns_w[16];
    ns_main(idx, ns_m, sizeof(ns_m));
    ns_wal(idx,  ns_w, sizeof(ns_w));

    /* Try per-user WAL first */
    nvs_handle_t wal;
    bool recovered = false;

    if (nvs_open(ns_w, NVS_READONLY, &wal) == ESP_OK) {
        uint8_t valid = 0;
        nvs_get_u8(wal, "valid", &valid);
        if (valid) {
            ESP_LOGW(TAG, "User %d: WAL recovery in progress", idx);
            int32_t s, y, m, d;
            nvs_get_i32(wal, "streak", &s);
            nvs_get_i32(wal, "lastY",  &y);
            nvs_get_i32(wal, "lastM",  &m);
            nvs_get_i32(wal, "lastD",  &d);
            nvs_close(wal);

            nvs_handle_t main_h;
            if (nvs_open(ns_m, NVS_READWRITE, &main_h) == ESP_OK) {
                nvs_set_i32(main_h, "streak", s);
                nvs_set_i32(main_h, "lastY",  y);
                nvs_set_i32(main_h, "lastM",  m);
                nvs_set_i32(main_h, "lastD",  d);
                nvs_commit(main_h);
                nvs_close(main_h);
            }
            if (nvs_open(ns_w, NVS_READWRITE, &wal) == ESP_OK) {
                nvs_erase_all(wal);
                nvs_commit(wal);
                nvs_close(wal);
            }
            ESP_LOGI(TAG, "User %d: WAL recovery complete", idx);
            recovered = true;
        } else {
            nvs_close(wal);
        }
    }

    /* User 0: also check legacy "streak_wal" namespace */
    if (!recovered && idx == 0) {
        if (nvs_open("streak_wal", NVS_READONLY, &wal) == ESP_OK) {
            uint8_t valid = 0;
            nvs_get_u8(wal, "valid", &valid);
            if (valid) {
                ESP_LOGW(TAG, "User 0: legacy WAL recovery in progress");
                int32_t s, y, m, d;
                nvs_get_i32(wal, "streak", &s);
                nvs_get_i32(wal, "lastY",  &y);
                nvs_get_i32(wal, "lastM",  &m);
                nvs_get_i32(wal, "lastD",  &d);
                nvs_close(wal);

                /* Write recovered data into per-user namespace */
                nvs_handle_t main_h;
                if (nvs_open(ns_m, NVS_READWRITE, &main_h) == ESP_OK) {
                    nvs_set_i32(main_h, "streak", s);
                    nvs_set_i32(main_h, "lastY",  y);
                    nvs_set_i32(main_h, "lastM",  m);
                    nvs_set_i32(main_h, "lastD",  d);
                    nvs_commit(main_h);
                    nvs_close(main_h);
                }
                if (nvs_open("streak_wal", NVS_READWRITE, &wal) == ESP_OK) {
                    nvs_erase_all(wal);
                    nvs_commit(wal);
                    nvs_close(wal);
                }
                ESP_LOGI(TAG, "User 0: legacy WAL recovery complete");
            } else {
                nvs_close(wal);
            }
        }
    }
}

/* ── Public API ── */

void streak_store_init(void) {
    /* Load whichever user was active at last shutdown */
    streak_set_active_user(active_user);
}

void streak_set_active_user(int user_idx) {
    if (user_idx < 0 || user_idx >= user_count) user_idx = 0;

    /* Save the outgoing user's streak before switching */
    if (s_loaded_user >= 0 && s_loaded_user != user_idx) {
        save_with_wal_for_user(s_loaded_user);
    }

    memset(&streak_data, 0, sizeof(streak_data));
    recover_wal_for_user(user_idx);
    load_for_user(user_idx);
    s_loaded_user = user_idx;

    ESP_LOGI(TAG, "Active user=%d (%s) streak=%" PRId32,
             user_idx, users[user_idx].name, streak_data.streak);
}

static bool is_consecutive(int y1, int m1, int d1, int y2, int m2, int d2) {
    struct tm t1 = {0}, t2 = {0};
    t1.tm_year = y1 - 1900; t1.tm_mon = m1 - 1; t1.tm_mday = d1;
    t2.tm_year = y2 - 1900; t2.tm_mon = m2 - 1; t2.tm_mday = d2;
    time_t e1 = mktime(&t1), e2 = mktime(&t2);
    if (e1 == -1 || e2 == -1) return false;
    return (e2 - e1) / 86400 == 1;
}

void streak_mark_day_complete(int day_offset) {
    time_t now;
    time(&now);
    struct tm t;
    localtime_r(&now, &t);
    t.tm_mday += day_offset;
    mktime(&t);

    int y = t.tm_year + 1900, m = t.tm_mon + 1, d = t.tm_mday;

    if (streak_data.last_year == y && streak_data.last_month == m && streak_data.last_day == d)
        return;  /* already marked today */

    if (is_consecutive(streak_data.last_year, streak_data.last_month,
                       streak_data.last_day, y, m, d)) {
        streak_data.streak++;
    } else {
        streak_data.streak = 1;
    }

    streak_data.last_year  = y;
    streak_data.last_month = m;
    streak_data.last_day   = d;
    save_with_wal_for_user(s_loaded_user);
}

/* Read any user's streak from NVS without disturbing the active streak_data */
void streak_read_user(int user_idx, streak_data_t *out) {
    memset(out, 0, sizeof(*out));
    if (user_idx < 0 || user_idx >= MAX_USERS) return;

    /* If it's the active user, just copy the live data (may be unsaved) */
    if (user_idx == s_loaded_user) {
        *out = streak_data;
        return;
    }

    char ns[16];
    ns_main(user_idx, ns, sizeof(ns));
    nvs_handle_t h;
    if (nvs_open(ns, NVS_READONLY, &h) == ESP_OK) {
        nvs_get_i32(h, "streak", &out->streak);
        nvs_get_i32(h, "lastY",  &out->last_year);
        nvs_get_i32(h, "lastM",  &out->last_month);
        nvs_get_i32(h, "lastD",  &out->last_day);
        nvs_close(h);
    }
}

/* Return the level name for an arbitrary streak count */
const char *streak_level_for(int32_t streak) {
    const streak_level_t *cur = &LEVELS[0];
    for (int i = 0; i < NUM_LEVELS; i++)
        if (streak >= LEVELS[i].threshold) cur = &LEVELS[i];
    return cur->name;
}

const streak_level_t *streak_get_level(void) {
    const streak_level_t *cur = &LEVELS[0];
    for (int i = 0; i < NUM_LEVELS; i++)
        if (streak_data.streak >= LEVELS[i].threshold) cur = &LEVELS[i];
    return cur;
}

const streak_level_t *streak_get_next_level(void) {
    for (int i = 0; i < NUM_LEVELS; i++)
        if (streak_data.streak < LEVELS[i].threshold) return &LEVELS[i];
    return NULL;
}

int streak_get_progress_to_next(void) {
    const streak_level_t *cur  = streak_get_level();
    const streak_level_t *next = streak_get_next_level();
    if (!next) return 100;
    int range    = next->threshold - cur->threshold;
    int progress = streak_data.streak - cur->threshold;
    return (progress * 100) / range;
}
