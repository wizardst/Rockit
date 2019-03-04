/*
 * Copyright 2019 Rockchip Electronics Co. LTD
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
 * Author: shika.zhou@rock-chips.com
 *   Date: 2019/03/01
 *
 */

#include "ALSAAapterUtils.h" // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ALSAAapterUtils"

int parse_control_id(const char *str, snd_ctl_elem_id_t *id) {
    int c, size, numid;
    char *ptr;

    while (*str == ' ' || *str == '\t')
        str++;
    if (!(*str))
        return -EINVAL;
    snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
    while (*str) {
        if (!strncasecmp(str, "numid=", 6)) {
            str += 6;
            numid = atoi(str);
            if (numid <= 0) {
                fprintf(stderr, "amixer: Invalid numid %d\n", numid);
                return -EINVAL;
            }
            snd_ctl_elem_id_set_numid(id, atoi(str));
            while (isdigit(*str))
            str++;
        } else if (!strncasecmp(str, "iface=", 6)) {
            str += 6;
            if (!strncasecmp(str, "card", 4)) {
                snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_CARD);
                str += 4;
            } else if (!strncasecmp(str, "mixer", 5)) {
                snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
                str += 5;
            } else if (!strncasecmp(str, "pcm", 3)) {
                snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_PCM);
                str += 3;
            } else if (!strncasecmp(str, "rawmidi", 7)) {
                snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_RAWMIDI);
                str += 7;
            } else if (!strncasecmp(str, "timer", 5)) {
                snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_TIMER);
                str += 5;
            } else if (!strncasecmp(str, "sequencer", 9)) {
                snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_SEQUENCER);
                str += 9;
            } else {
                return -EINVAL;
            }
        } else if (!strncasecmp(str, "name=", 5)) {
            char buf[64];
            str += 5;
            ptr = buf;
            size = 0;
            if (*str == '\'' || *str == '\"') {
                c = *str++;
                while (*str && *str != c) {
                    if (size < static_cast<int>(sizeof(buf))) {
                        *ptr++ = *str;
                        size++;
                    }
                    str++;
                }
                if (*str == c)
                    str++;
            } else {
                while (*str && *str != ',') {
                    if (size < static_cast<int>(sizeof(buf))) {
                        *ptr++ = *str;
                        size++;
                    }
                    str++;
                }
                *ptr = '\0';
            }
            snd_ctl_elem_id_set_name(id, buf);
        } else if (!strncasecmp(str, "index=", 6)) {
            str += 6;
            snd_ctl_elem_id_set_index(id, atoi(str));
            while (isdigit(*str))
                str++;
        } else if (!strncasecmp(str, "device=", 7)) {
            str += 7;
            snd_ctl_elem_id_set_device(id, atoi(str));
            while (isdigit(*str))
                str++;
        } else if (!strncasecmp(str, "subdevice=", 10)) {
            str += 10;
            snd_ctl_elem_id_set_subdevice(id, atoi(str));
            while (isdigit(*str))
                str++;
        }
        if (*str == ',') {
            str++;
        } else {
            if (*str)
                return -EINVAL;
        }
    }

    return 0;
}

const char *control_iface(snd_ctl_elem_id_t *id) {
    return snd_ctl_elem_iface_name(snd_ctl_elem_id_get_interface(id));
}

void show_control_id(snd_ctl_elem_id_t *id) {
    unsigned int index, device, subdevice;
    RT_LOGD("numid=%u,iface=%s,name='%s'",
            snd_ctl_elem_id_get_numid(id),
            control_iface(id),
            snd_ctl_elem_id_get_name(id));
    index = snd_ctl_elem_id_get_index(id);
    device = snd_ctl_elem_id_get_device(id);
    subdevice = snd_ctl_elem_id_get_subdevice(id);
    if (index)
        RT_LOGD(",index=%i", index);
    if (device)
        RT_LOGD(",device=%i", device);
    if (subdevice)
        RT_LOGD(",subdevice=%i", subdevice);
}

INT32 get_integer(char **ptr, INT32 min, INT32 max) {
    INT32 val = min;
    char *p = *ptr, *s;

    if (*p == ':')
        p++;
    if (*p == '\0' || (!isdigit(*p) && *p != '-'))
        goto out;

    s = p;
    val = strtol(s, &p, 10);
    if (*p == '.') {
        p++;
        strtol(p, &p, 10);
    }
    if (*p == '%') {
        val = (INT32)convert_prange1(strtod(s, NULL), min, max);
        p++;
    }
    val = check_range(val, min, max);
    if (*p == ',')
        p++;
 out:
    *ptr = p;
    return val;
}

