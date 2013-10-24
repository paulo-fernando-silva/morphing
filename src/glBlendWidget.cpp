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


void glBlendWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    if( src() == 0 or dst() == 0 or
        src()->tex() == 0 or dst()->tex() == 0 or
        glBlendColor == 0)
        return;

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

