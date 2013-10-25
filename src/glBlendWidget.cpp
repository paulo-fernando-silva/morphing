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

#include "cgl/glu.hpp"
#include "glBlendWidget.hpp"
#include "glFFDWidget.hpp"
#include "QRTT.hpp"

#include <QDrag>
#include <QPixmap>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>

#include <algorithm>


const color CLEAR_COLOR(1.0f, 1.0f, 1.0f, 1.0f);


glBlendWidget::glBlendWidget(QWidget* parent):
    QGLWidget(parent),
    _t(0.0f),
    _src(0),
    _dst(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


glBlendWidget::~glBlendWidget() {}


void glBlendWidget::src(glFFDWidget* const w) {
    _src = w;
    assert(invariant());
    updateFaces();
}


void glBlendWidget::dst(glFFDWidget* const w) {
    _dst = w;
    assert(invariant());
    updateFaces();
}


bool glBlendWidget::invariant() const {
    return  (src() == 0 or dst() == 0) or
            (src()->mesh().size() == dst()->mesh().size());
}


void glBlendWidget::blendFactor(float t) {
    _t = std::min(std::max(t, 0.0f), 1.0f);
    emit blendFactorChanged(_t);
}


bool glBlendWidget::canPaint() const {
    return src() != 0 and dst() != 0 and
            (src()->tex() != 0 or dst()->tex() != 0);
}

void glBlendWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    if(canPaint())
        drawBlended(src()->mesh(), dst()->mesh(), faces(),
                    src()->tex(), dst()->tex(), blendFactor());
}


void glBlendWidget::initializeGL() {
    glClearColor(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, CLEAR_COLOR.a);
    cgl::view2D(uvec2(), uvec2(width(), height()));
}


void glBlendWidget::resizeGL(int width, int height) {
    cgl::view2D(uvec2(), uvec2(width, height));
}


void glBlendWidget::updateFaces() {
    _faces.clear();

    if(src() == 0 or dst() == 0)
        return;

    assert(invariant());

    assert(src()->resolution() != 0);
    unsigned w(src()->resolution() - 1);

    generateTriangles(w, w, _faces);
}


QImage glBlendWidget::frame() {
    QRTT rtt(this);
    paintGL();
    return rtt.toImage();
}


void glBlendWidget::mousePressEvent(QMouseEvent* event) {
    if(event->button() == Qt::LeftButton)
        _mouse_press_pos = event->pos();
}


void glBlendWidget::mouseMoveEvent(QMouseEvent* event) {
    if(not (event->buttons() & Qt::LeftButton))
        return;

    if((event->pos() - _mouse_press_pos).manhattanLength() >=
        QApplication::startDragDistance())
        dragEvent();
}


void glBlendWidget::dragEvent() {
    if(canPaint()) {
        // must be able to paint the frame
        const QImage& img(frame());

        QMimeData* const mime_data(new QMimeData);
        mime_data->setImageData(QVariant(img));

        QDrag* const drag(new QDrag(this));
        drag->setMimeData(mime_data);
        drag->setPixmap(QPixmap::fromImage(img));
        drag->exec();
    }
}
