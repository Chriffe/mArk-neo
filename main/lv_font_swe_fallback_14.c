/*******************************************************************************
 * Size: 14 px
 * Bpp: 4
 * Opts: --font /tmp/IBMPlexSans-Regular.ttf -r 0xC4-0xC5,0xD6,0xE4-0xE5,0xF6 --size 14 --format lvgl --bpp 4 --lv-font-name lv_font_swe_fallback_14 -o /dev-server/esp32-project/main/lv_font_swe_fallback_14.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl.h"
#endif

#ifndef LV_FONT_SWE_FALLBACK_14
#define LV_FONT_SWE_FALLBACK_14 1
#endif

#if LV_FONT_SWE_FALLBACK_14

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+00C4 "Ä" */
    0x0, 0x13, 0x3, 0x18, 0x6, 0x4a, 0xa, 0x50,
    0xc, 0xd8, 0x18, 0xc0, 0x1c, 0xde, 0xc0, 0x1e,
    0xd0, 0xd0, 0xf, 0x23, 0xa0, 0x7, 0x28, 0xc0,
    0x28, 0x6, 0xf7, 0x5, 0xf0, 0xc, 0xba, 0x1e,
    0x80, 0x12, 0xa, 0x2, 0x8a, 0x0, 0x34, 0x7f,
    0xe0, 0xd0, 0x3, 0xc6, 0xec, 0xee, 0x3, 0x17,
    0x22, 0xa4, 0x4e, 0xd4, 0x3, 0x9, 0xd8,

    /* U+00C5 "Å" */
    0x0, 0x9b, 0x18, 0x3, 0xd5, 0x94, 0x1, 0xff,
    0xc3, 0xac, 0xa0, 0xf, 0x8c, 0x3, 0xed, 0xd,
    0x0, 0xf2, 0x3a, 0x0, 0x72, 0x8c, 0x2, 0x80,
    0x6f, 0x70, 0x5f, 0x0, 0xcb, 0xa1, 0xe8, 0x1,
    0x20, 0xa0, 0x28, 0xa0, 0x3, 0x47, 0xfe, 0xd,
    0x0, 0x3c, 0x6e, 0xce, 0xe0, 0x31, 0x72, 0x2a,
    0x44, 0xed, 0x40, 0x30, 0x9d, 0x80,

    /* U+00D6 "Ö" */
    0x0, 0x9c, 0x49, 0x80, 0x3d, 0x2a, 0xf6, 0x1,
    0xee, 0x45, 0xd0, 0xe, 0x2b, 0xff, 0x51, 0x0,
    0x45, 0xa5, 0x96, 0x18, 0x20, 0x8, 0x59, 0x34,
    0xa5, 0x90, 0x2, 0xc8, 0x6, 0xe5, 0x2, 0x2,
    0x0, 0xce, 0x40, 0x1f, 0xe1, 0x0, 0xfe, 0x12,
    0x2, 0x0, 0xce, 0x40, 0x5, 0x90, 0xd, 0xca,
    0x0, 0x85, 0x93, 0x35, 0x2c, 0x80, 0xb, 0x4b,
    0x30, 0x18, 0x20,

    /* U+00E4 "ä" */
    0x5, 0xc0, 0xb6, 0x0, 0x9e, 0x83, 0x90, 0x2,
    0x26, 0x5, 0x20, 0x9, 0x37, 0xf5, 0x0, 0x2f,
    0x9e, 0x8a, 0x0, 0xad, 0x5, 0x40, 0xc0, 0xb,
    0x9d, 0xc1, 0x70, 0x2a, 0xfe, 0xe0, 0x80, 0x1c,
    0x8c, 0x3, 0x9c, 0x84, 0x14, 0x40, 0x5, 0x19,
    0xa5, 0x16, 0x20,

    /* U+00E5 "å" */
    0x0, 0x3e, 0x40, 0x7, 0x5e, 0x18, 0x80, 0x6b,
    0xb1, 0x88, 0x6, 0x7b, 0x80, 0xc, 0x9b, 0xfa,
    0x80, 0x17, 0xcf, 0x45, 0x0, 0x56, 0x82, 0xa0,
    0x60, 0x5, 0xce, 0xe0, 0xb8, 0x15, 0x7f, 0x70,
    0x40, 0xe, 0x46, 0x1, 0xce, 0x42, 0xa, 0x20,
    0x2, 0x8c, 0xd2, 0x8b, 0x10,

    /* U+00F6 "ö" */
    0x0, 0x71, 0xae, 0x0, 0x69, 0x27, 0xa0, 0xc,
    0xe2, 0x4c, 0x1, 0xa7, 0xf9, 0xc0, 0x2a, 0x3d,
    0xd5, 0xb8, 0xa, 0xd1, 0x1f, 0x48, 0x29, 0x0,
    0x44, 0x22, 0x0, 0xfc, 0xa4, 0x1, 0x10, 0x8c,
    0xb4, 0x47, 0xd2, 0x0, 0xa3, 0xdd, 0x5b, 0x80
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 144, .box_w = 9, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 55, .adv_w = 144, .box_w = 9, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 109, .adv_w = 159, .box_w = 10, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 168, .adv_w = 120, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 211, .adv_w = 120, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 256, .adv_w = 125, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x1, 0x12, 0x20, 0x21, 0x32
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 196, .range_length = 51, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 6, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 1, 1, 2, 3, 3, 0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 1, 1, 2, 3, 3, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    3, -3, 1, -3, 0, 0, 2, 0,
    0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 3,
    .right_class_cnt     = 3,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 1,
    .bitmap_format = 1,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t lv_font_swe_fallback_14 = {
#else
lv_font_t lv_font_swe_fallback_14 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if LV_FONT_SWE_FALLBACK_14*/

