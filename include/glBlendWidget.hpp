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

#ifndef GLBLENDWIDGET_HPP
#define GLBLENDWIDGET_HPP

#include "utils.hpp"

#include <QSize>
#include <QPoint>
#include <QImage>
#include <QGLWidget>


class glFFDWidget;


class glBlendWidget : public QGLWidget {
    Q_OBJECT
public:
    glBlendWidget(QWidget* const parent);

    ~glBlendWidget();


    void src(glFFDWidget* const w);


    inline glFFDWidget* src() const {
        return _src;
    }


    void dst(glFFDWidget* const w);


    inline glFFDWidget* dst() const {
        return _dst;
    }


    inline const float& blendFactor() const {
        return _t;
    }


    void updateFaces();

    // made public to help with rtt
    void paintGL();

    /**
     * @brief frame a SLOW but simple way of capturing the current frame
     * @return a QImage representing the widget's contents.
     * @pre #canPaint
     */
    QImage frame();

    bool canPaint() const;

    QSize maxImgDim();


signals:
    void blendFactorChanged(float t);


public slots:
    void blendFactor(float t);


protected:
    void initializeGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    inline const Faces& faces() {
        return _faces;
    }

    bool invariant() const;

    void dragEvent();


private:
    float _t;
    glFFDWidget* _src;
    glFFDWidget* _dst;
    Faces _faces;
    QPoint _mouse_press_pos;
};


#endif // GLBLENDWIDGET_HPP
