/*
 * The MIT License (MIT)
 *
 * Copyright (C) 2004-2013 Paulo Silva <paulo.jnkml@gmail.com>
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

#include "QRTT.hpp"
#include "glBlendWidget.hpp"
#include "glFFDWidget.hpp"
#include "cgl/glu.hpp"

#include <QGLFramebufferObject>
#include <cassert>


QRTT::QRTT(QGLWidget* const widget, const QSize& dim):
    _widget(widget)
{
    assert(_widget != 0);

    if(_widget->context() != QGLContext::currentContext())
        _widget->makeCurrent();

    _fbo.reset(new QGLFramebufferObject(dim.width(), dim.height()));
    _pixels.reset(new byte[dim.width() * dim.height() * 4]);

    bind();
}


QRTT::~QRTT() {
    unbind();
}


unsigned QRTT::width() const {
    return _fbo->size().width();
}


unsigned QRTT::height() const {
    return _fbo->size().height();
}


void QRTT::bind() {
    assert(_fbo->bind());
    cgl::view2D(cgl::uvec2(), uvec2(width(), height()));
}


void QRTT::unbind() {
    assert(_fbo->release());
    const uvec2 dim(_widget->width(), _widget->height());
    cgl::view2D(cgl::uvec2(), dim);
}


void QRTT::grabPixels() {
    glReadPixels(0, 0,  width(), height(), GL_RGBA,
                 GL_UNSIGNED_BYTE, _pixels.get());
}


QImage QRTT::toImage() const {
    return _fbo->toImage();
}





