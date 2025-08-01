/**
 *  Copyright 2025, Song,Tao.
 *  Created by Song,Tao on 2025/07/13.
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
#ifndef COMMON_UI_MAIN_FRAME_H
#define COMMON_UI_MAIN_FRAME_H

#include <QMainWindow>

class UIMainFrame final : public QMainWindow {

    Q_OBJECT

public:
    explicit UIMainFrame(QWidget *CentralWidget, QString &Title, int Width = 800, int Height = 600, QWidget *Parent = nullptr);

private:
    // Set dark and light themes
    static void setDarkTheme();
    static void setLightTheme();

    static void selectFount();
};


#endif //COMMON_UI_MAIN_FRAME_H
