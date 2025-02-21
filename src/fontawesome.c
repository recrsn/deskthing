/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --no-compress --use-color-info --font fa-solid-900.ttf --range 62036,61675,61459,61441,57455,61555 --format lvgl -o fontawesome.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef FONTAWESOME
#define FONTAWESOME 1
#endif

#if FONTAWESOME

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+E06F "" */
    0xf, 0x80, 0x7c, 0x0, 0xc0, 0x1f, 0x3, 0xff,
    0xbf, 0xf9, 0xdd, 0xdc, 0x47, 0xea, 0xbf, 0x95,
    0xf9, 0xaf, 0xdd, 0x76, 0x23, 0x3f, 0xf8, 0xff,
    0x81, 0xf0,

    /* U+F001 "" */
    0x0, 0x7, 0x0, 0x3f, 0x3, 0xff, 0xf, 0xff,
    0xf, 0xff, 0xf, 0xe3, 0xf, 0x3, 0xc, 0x3,
    0xc, 0x3, 0xc, 0x1f, 0xc, 0x3f, 0x7c, 0x3f,
    0xfc, 0x3f, 0xfc, 0x1e, 0xfc, 0x0, 0x78, 0x0,

    /* U+F013 "" */
    0x3, 0xc0, 0x3, 0xc0, 0x27, 0xe4, 0x7f, 0xfe,
    0x7f, 0xfe, 0xfe, 0x7f, 0x7c, 0x3e, 0x3c, 0x3c,
    0x3c, 0x3c, 0x7c, 0x3e, 0xfe, 0x7f, 0x7f, 0xfe,
    0x7f, 0xfe, 0x27, 0xe4, 0x3, 0xc0, 0x3, 0xc0,

    /* U+F073 "" */
    0x18, 0x60, 0x61, 0x8f, 0xff, 0xff, 0xff, 0xff,
    0xfc, 0x0, 0xf, 0xff, 0xff, 0xff, 0xcc, 0xcf,
    0x33, 0x3f, 0xff, 0xff, 0xff, 0xcc, 0xcf, 0x33,
    0x3f, 0xff, 0xff, 0xff,

    /* U+F0EB "" */
    0x1f, 0x7, 0xf1, 0xcf, 0x77, 0xfd, 0xff, 0xbf,
    0xff, 0xff, 0xff, 0x7f, 0xc7, 0xf0, 0xfe, 0xf,
    0x80, 0x0, 0x3e, 0x7, 0xc0, 0x70,

    /* U+F254 "" */
    0xff, 0xff, 0xff, 0x60, 0x66, 0x6, 0x70, 0xc3,
    0x9c, 0x1f, 0x80, 0xf0, 0xf, 0x1, 0xf8, 0x39,
    0xc7, 0xc, 0x60, 0x66, 0x6, 0xff, 0xff, 0xff
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 224, .box_w = 13, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 26, .adv_w = 256, .box_w = 16, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 58, .adv_w = 256, .box_w = 16, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 90, .adv_w = 224, .box_w = 14, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 118, .adv_w = 192, .box_w = 11, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 140, .adv_w = 192, .box_w = 12, .box_h = 16, .ofs_x = 0, .ofs_y = -2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0xf92, 0xfa4, 0x1004, 0x107c, 0x11e5
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 57455, .range_length = 4582, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 6, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
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
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t fontawesome = {
#else
lv_font_t fontawesome = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
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



#endif /*#if FONTAWESOME*/

