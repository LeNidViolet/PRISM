/**
 *  Copyright 2025, Song,Tao.
 *  Created by Song,Tao on 2025/07/08.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H

typedef struct NAMEINDEX_ {
    unsigned int index;
    const char *name;
} NAMEINDEX;

#define CREATESTRMAP(_NAME) {_NAME, #_NAME}
#define CREATECONNECTEDNAME(prefix, name)    prefix##_##name
#define CREATECONNECTEDMAP(prefix, name)     prefix##_##name, #name



#define COLOR_FOREGROUND_ERROR              (QColor::fromRgb(0xFFF44336))
#define COLOR_FOREGROUND_WARN               (QColor::fromRgb(0xFFFFC107))

#define COLOR_BACKGROUND_NEW                (QColor::fromRgb(0xFF66BB6A))
#define COLOR_BACKGROUND_ACTIVE             (QColor::fromRgb(0xFF259FB4))
#define COLOR_BACKGROUND_DELETE             (QColor::fromRgb(0xFFEF5350))

#define G_N_ELEMENTS(arr)                   ((sizeof(arr))/(sizeof((arr)[0])))

#define TREE_WIDGET_ROW_HEIGHT              (20)
#define MAX_LABEL_ECHO_LENGTH               (300)
#define ITEMROLE                            (Qt::UserRole + 0x100)      // return TreeItem
#define SPANROLE                            (ITEMROLE + 1)              // return is need row span

#endif //COMMON_DEFINE_H
