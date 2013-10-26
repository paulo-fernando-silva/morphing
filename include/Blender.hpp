/*
 * The MIT License (MIT)
 *
 * Copyright (C) 2013 Paulo Silva <paulo.jnkml@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef BLENDER_HPP
#define BLENDER_HPP

#include <QWidget>
#include <QString>


class glBlendWidget;

class QLabel;
class QSlider;


class Blender : public QWidget {
    Q_OBJECT
public:
    enum AnimDir {FRONT = 1, BACK = -1};

    Blender(QWidget* const parent, const QString& title);


    void update();

    void clear();


    inline glBlendWidget* widget() const {
        return _widget;
    }

    // animation duration in milliseconds
    inline unsigned duration() const {
        return _duration;
    }

    inline unsigned fps() const {
        return _fps;
    }


    void animationDirection(const AnimDir dir);

    inline AnimDir animationDirection() const {
        return _anim_dir;
    }


    void bidirectional(const bool state);

    inline bool bidirectional() const {
        return _bidirectional;
    }


    void animated(const bool state);

    inline bool animated() const {
        return _animated;
    }


    void frameNumber(const float n);
    float frameNumber() const;

    inline unsigned numberOfFrames() const {
        return fps() * duration() / 1000;
    }


    bool save(const QString& uri);


signals:
    void animStateChanged();


public slots:
    void percent(int p);
    void blendFactorChanged(float t);
    void duration(int n);
    void fps(int f);


private:
    void setupUI(const QString& title);

    void stepAnimation();
    float t(const unsigned frame_number) const;

    inline QSlider* slider() const {
        return _slider;
    }


private:
    AnimDir _anim_dir;

    glBlendWidget* _widget;

    QLabel* _title;
    QSlider* _slider;

    bool _animated;
    bool _bidirectional;
    unsigned _duration;
    unsigned _fps;
};

#endif // BLENDER_HPP
