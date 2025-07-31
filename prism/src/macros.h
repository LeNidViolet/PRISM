/**
 *  Copyright 2022, LeNidViolet.
 *  Created by LeNidViolet on 2022/8/25.
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
#ifndef PRISM_MACROS_H
#define PRISM_MACROS_H

typedef struct NAMEINDEX_ {
    unsigned int index;
    const char *name;
} NAMEINDEX;

#define CREATESTRMAP(_NAME) {_NAME, #_NAME}
#define CREATECONNECTEDNAME(prefix, name)    prefix##_##name
#define CREATECONNECTEDMAP(prefix, name)     prefix##_##name, #name

#define COLOR_ACCEPTABLE        (QColor::fromRgb(0xFF57C038))
#define COLOR_UNACCEPTABLE      (QColor::fromRgb(0xFFD66871))

#define COLOR_ERROR             COLOR_UNACCEPTABLE
#define COLOR_WARN              (QColor::fromRgb(0xFFA0AB4C))

#define G_N_ELEMENTS(arr)       ((sizeof(arr))/(sizeof((arr)[0])))

#endif //PRISM_MACROS_H
