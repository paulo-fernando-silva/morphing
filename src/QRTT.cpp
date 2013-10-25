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


QRTT::QRTT(glBlendWidget* const blender):
    _blender(blender)
{
    assert(blender != 0);
    createFramebuffer();
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


void QRTT::createFramebuffer() {
    const GLint src(_blender->src()->tex());
    const GLint dst(_blender->dst()->tex());
    assert(src != 0 or dst != 0);

    _blender->makeCurrent();
    cgl::uvec2 src_dim, dst_dim;

    if(src != 0)
        src_dim = cgl::dimensions(src);

    if(dst != 0)
        dst_dim = cgl::dimensions(dst);

    const cgl::uvec2& dim(cgl::max(src_dim, dst_dim));
    assert(cgl::area(dim) != 0);

    _fbo.reset(new QGLFramebufferObject(dim.x, dim.y));
    _pixels.reset(new byte[width() * height() * 4]);
}


void QRTT::bind() {
    assert(_fbo->bind());
    cgl::view2D(cgl::uvec2(), uvec2(width(), height()));
}


void QRTT::unbind() {
    assert(_fbo->release());
    const uvec2 dim(_blender->width(), _blender->height());
    cgl::view2D(cgl::uvec2(), dim);
}


void QRTT::grabPixels() {
    glReadPixels(0, 0,  width(), height(), GL_RGBA,
                 GL_UNSIGNED_BYTE, _pixels.get());
}


QImage QRTT::toImage() const {
    return _fbo->toImage();
}





