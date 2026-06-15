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
    { "Lärling",      0   },
    { "Upptäckare",   5   },
    { "Magiker",      15  },
    { "Mästare",      30  },
    { "Stormästare",  50  },
    { "Tidsväktare",  100 },
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
    streak_check_missed_day();

    ESP_LOGI(TAG, "Active user=%d (%s) streak=%" PRId32,
             user_idx, users[user_idx].name, streak_data.streak);
}

static bool is_consecutive(int y1, int m1, int d1, int y2, int m2, int d2) {
    /* Advance d1 by 1 calendar day using mktime normalisation (DST-safe). */
    struct tm t = {0};
    t.tm_year = y1 - 1900; t.tm_mon = m1 - 1; t.tm_mday = d1 + 1; t.tm_isdst = -1;
    mktime(&t);
    return (t.tm_year + 1900 == y2 && t.tm_mon + 1 == m2 && t.tm_mday == d2);
}

void streak_check_missed_day(void) {
    if (streak_data.last_year == 0) return;  /* never completed */
    if (streak_data.streak == 0) return;     /* already zero */

    time_t now = time(NULL);
    if (now < 1700000000) return;  /* NTP not yet synced */

    struct tm t;
    localtime_r(&now, &t);
    int y = t.tm_year + 1900, m = t.tm_mon + 1, d = t.tm_mday;

    if (streak_data.last_year == y && streak_data.last_month == m && streak_data.last_day == d)
        return;  /* completed today — streak intact */

    if (is_consecutive(streak_data.last_year, streak_data.last_month,
                       streak_data.last_day, y, m, d))
        return;  /* completed yesterday — still waiting for today */

    ESP_LOGI(TAG, "Streak reset: last=%04" PRId32 "-%02" PRId32 "-%02" PRId32
             " today=%04d-%02d-%02d",
             streak_data.last_year, streak_data.last_month, streak_data.last_day,
             y, m, d);
    streak_data.streak = 0;
    save_with_wal_for_user(s_loaded_user);
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

void streak_shift_down(int removed_idx, int new_count) {
    /* When a user at removed_idx is deleted, shift all higher-index NVS
     * namespaces down by 1 so indices stay in sync with users[]. */
    for (int i = removed_idx; i < new_count; i++) {
        char src_m[16], src_w[16], dst_m[16], dst_w[16];
        ns_main(i + 1, src_m, sizeof(src_m));
        ns_wal( i + 1, src_w, sizeof(src_w));
        ns_main(i,     dst_m, sizeof(dst_m));
        ns_wal( i,     dst_w, sizeof(dst_w));

        /* Apply any pending WAL for the source slot before reading */
        recover_wal_for_user(i + 1);

        /* Read from i+1 */
        int32_t s = 0, y = 0, mo = 0, d = 0;
        nvs_handle_t h;
        if (nvs_open(src_m, NVS_READONLY, &h) == ESP_OK) {
            nvs_get_i32(h, "streak", &s);
            nvs_get_i32(h, "lastY",  &y);
            nvs_get_i32(h, "lastM",  &mo);
            nvs_get_i32(h, "lastD",  &d);
            nvs_close(h);
        }

        /* Write to i */
        if (nvs_open(dst_m, NVS_READWRITE, &h) == ESP_OK) {
            nvs_set_i32(h, "streak", s);
            nvs_set_i32(h, "lastY",  y);
            nvs_set_i32(h, "lastM",  mo);
            nvs_set_i32(h, "lastD",  d);
            nvs_commit(h);
            nvs_close(h);
        }

        /* Clear WAL at old slot */
        if (nvs_open(src_w, NVS_READWRITE, &h) == ESP_OK) {
            nvs_erase_all(h);
            nvs_commit(h);
            nvs_close(h);
        }
        /* Clear WAL at new slot (was overwritten above) */
        if (nvs_open(dst_w, NVS_READWRITE, &h) == ESP_OK) {
            nvs_erase_all(h);
            nvs_commit(h);
            nvs_close(h);
        }
    }

    /* Erase the now-orphaned last slot */
    char last_m[16], last_w[16];
    ns_main(new_count, last_m, sizeof(last_m));
    ns_wal( new_count, last_w, sizeof(last_w));
    nvs_handle_t h;
    if (nvs_open(last_m, NVS_READWRITE, &h) == ESP_OK) {
        nvs_erase_all(h); nvs_commit(h); nvs_close(h);
    }
    if (nvs_open(last_w, NVS_READWRITE, &h) == ESP_OK) {
        nvs_erase_all(h); nvs_commit(h); nvs_close(h);
    }

    /* Re-sync s_loaded_user if it shifted */
    if (s_loaded_user > removed_idx) {
        s_loaded_user--;
    } else if (s_loaded_user == removed_idx) {
        s_loaded_user = -1;  /* removed user — caller must call streak_set_active_user */
    }

    ESP_LOGI(TAG, "streak_shift_down: removed=%d new_count=%d s_loaded=%d",
             removed_idx, new_count, s_loaded_user);
}

void streak_reset_user(int user_idx) {
    if (user_idx < 0 || user_idx >= MAX_USERS) return;

    if (user_idx == s_loaded_user) {
        memset(&streak_data, 0, sizeof(streak_data));
        save_with_wal_for_user(user_idx);
    } else {
        char ns_m[16], ns_w[16];
        ns_main(user_idx, ns_m, sizeof(ns_m));
        ns_wal(user_idx,  ns_w, sizeof(ns_w));

        nvs_handle_t wal;
        if (nvs_open(ns_w, NVS_READWRITE, &wal) == ESP_OK) {
            nvs_erase_all(wal);
            nvs_commit(wal);
            nvs_close(wal);
        }

        nvs_handle_t h;
        if (nvs_open(ns_m, NVS_READWRITE, &h) == ESP_OK) {
            nvs_set_i32(h, "streak", 0);
            nvs_set_i32(h, "lastY",  0);
            nvs_set_i32(h, "lastM",  0);
            nvs_set_i32(h, "lastD",  0);
            nvs_commit(h);
            nvs_close(h);
        }
    }
    ESP_LOGI(TAG, "User %d streak reset to 0", user_idx);
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
