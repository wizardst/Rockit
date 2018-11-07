/*
 * Copyright 2018 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * author: martin.cheng@rock-chips.com
 *   date: 20180802
 */

#ifndef __RT_NODE_MACRO_H__
#define __RT_NODE_MACRO_H__

/**
 * \file
 * This file implements node (module) macros used to define a rt_node.
 */

enum rt_node_properties
{
    RT_NODE_CREATE,
    RT_CONFIG_CREATE,

    /* DO NOT EVER REMOVE, INSERT OR REPLACE ANY ITEM! It would break the ABI!
     * Append new items at the end ONLY. */
    RT_NODE_CPU_REQUIREMENT=0x100,
    RT_NODE_SHORTCUT,
    RT_NODE_CAPABILITY,
    RT_NODE_SCORE,
    RT_NODE_CB_OPEN,
    RT_NODE_CB_CLOSE,
    RT_NODE_NO_UNLOAD,
    RT_NODE_NAME,
    RT_NODE_SHORTNAME,
    RT_NODE_DESCRIPTION,
    RT_NODE_HELP,
    RT_NODE_TEXTDOMAIN,
    /* Insert new RT_NODE_* here */

    /* DO NOT EVER REMOVE, INSERT OR REPLACE ANY ITEM! It would break the ABI!
     * Append new items at the end ONLY. */
    RT_CONFIG_NAME=0x1000,
    /* command line name (args=const char *) */

    RT_CONFIG_VALUE,
    /* actual value (args=int64_t/double/const char *) */

    RT_CONFIG_RANGE,
    /* minimum value (args=int64_t/double/const char * twice) */

    RT_CONFIG_ADVANCED_RESERVED,
    /* reserved - do not use */

    RT_CONFIG_VOLATILE,
    /* don't write variable to storage (args=none) */

    RT_CONFIG_PERSISTENT_OBSOLETE,
    /* unused (ignored) */

    RT_CONFIG_PRIVATE,
    /* hide from user (args=none) */

    RT_CONFIG_REMOVED,
    /* tag as no longer supported (args=none) */

    RT_CONFIG_CAPABILITY,
    /* capability for a module or list thereof (args=const char*) */

    RT_CONFIG_SHORTCUT,
    /* one-character (short) command line option name (args=char) */

    RT_CONFIG_OLDNAME_OBSOLETE,
    /* unused (ignored) */

    RT_CONFIG_SAFE,
    /* tag as modifiable by untrusted input item "sources" (args=none) */

    RT_CONFIG_DESC,
    /* description (args=const char *, const char *, const char *) */

    RT_CONFIG_LIST_OBSOLETE,
    /* unused (ignored) */

    RT_CONFIG_ADD_ACTION_OBSOLETE,
    /* unused (ignored) */

    RT_CONFIG_LIST,
    /* list of suggested values
     * (args=size_t, const <type> *, const char *const *) */

    RT_CONFIG_LIST_CB,
    /* callback for suggested values
     * (args=const char *, size_t (*)(vlc_object_t *, <type> **, char ***)) */

    /* Insert new RT_CONFIG_* here */
};

/* Configuration hint types */
#define CONFIG_HINT_CATEGORY                0x02  /* Start of new category */
#define CONFIG_HINT_USAGE                   0x05  /* Usage information */

#define CONFIG_CATEGORY                     0x06 /* Set category */
#define CONFIG_SUBCATEGORY                  0x07 /* Set subcategory */
#define CONFIG_SECTION                      0x08 /* Start of new section */

/* Configuration item types */
#define CONFIG_ITEM_FLOAT                   0x20  /* Float option */
#define CONFIG_ITEM_INTEGER                 0x40  /* Integer option */
#define CONFIG_ITEM_RGB                     0x41  /* RGB color option */
#define CONFIG_ITEM_BOOL                    0x60  /* Bool option */
#define CONFIG_ITEM_STRING                  0x80  /* String option */
#define CONFIG_ITEM_PASSWORD                0x81  /* Password option (*) */
#define CONFIG_ITEM_KEY                     0x82  /* Hot key option */
#define CONFIG_ITEM_MODULE                  0x84  /* Module option */
#define CONFIG_ITEM_MODULE_CAT              0x85  /* Module option */
#define CONFIG_ITEM_MODULE_LIST             0x86  /* Module option */
#define CONFIG_ITEM_MODULE_LIST_CAT         0x87  /* Module option */
#define CONFIG_ITEM_LOADFILE                0x8C  /* Read file option */
#define CONFIG_ITEM_SAVEFILE                0x8D  /* Written file option */
#define CONFIG_ITEM_DIRECTORY               0x8E  /* Directory option */
#define CONFIG_ITEM_FONT                    0x8F  /* Font option */

#define CONFIG_ITEM(x) (((x) & ~0xF) != 0)

/* Categories and subcategories */
#define CAT_INTERFACE 1
#define SUBCAT_INTERFACE_GENERAL 101
#define SUBCAT_INTERFACE_MAIN 102
#define SUBCAT_INTERFACE_CONTROL 103
#define SUBCAT_INTERFACE_HOTKEYS 104

#define CAT_AUDIO 2
#define SUBCAT_AUDIO_GENERAL 201
#define SUBCAT_AUDIO_AOUT 202
#define SUBCAT_AUDIO_AFILTER 203
#define SUBCAT_AUDIO_VISUAL 204
#define SUBCAT_AUDIO_MISC 205
#define SUBCAT_AUDIO_RESAMPLER 206

#define CAT_VIDEO 3
#define SUBCAT_VIDEO_GENERAL 301
#define SUBCAT_VIDEO_VOUT 302
#define SUBCAT_VIDEO_VFILTER 303
#define SUBCAT_VIDEO_SUBPIC 305
#define SUBCAT_VIDEO_SPLITTER 306

#define CAT_INPUT 4
#define SUBCAT_INPUT_GENERAL 401
#define SUBCAT_INPUT_ACCESS 402
#define SUBCAT_INPUT_DEMUX 403
#define SUBCAT_INPUT_VCODEC 404
#define SUBCAT_INPUT_ACODEC 405
#define SUBCAT_INPUT_SCODEC 406
#define SUBCAT_INPUT_STREAM_FILTER 407

#define CAT_SOUT 5
#define SUBCAT_SOUT_GENERAL 501
#define SUBCAT_SOUT_STREAM 502
#define SUBCAT_SOUT_MUX 503
#define SUBCAT_SOUT_ACO 504
#define SUBCAT_SOUT_PACKETIZER 505
#define SUBCAT_SOUT_VOD 507
#define SUBCAT_SOUT_RENDERER 508

#define CAT_ADVANCED 6
#define SUBCAT_ADVANCED_MISC 602
#define SUBCAT_ADVANCED_NETWORK 603

#define CAT_PLAYLIST 7
#define SUBCAT_PLAYLIST_GENERAL 701
#define SUBCAT_PLAYLIST_SD 702
#define SUBCAT_PLAYLIST_EXPORT 703

/**
 * Current plugin ABI version
 */
# define MODULE_SYMBOL 4_0_3
# define MODULE_SUFFIX "__4_0_3"

#endif // __RT_NODE_MACRO_H__
