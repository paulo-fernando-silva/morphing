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
#include "glu.hpp"
#include "vec.hpp"

#include <QGLFramebufferObject>
#include <cassert>


QRTT::QRTT(QGLWidget* const widget, const QSize& dim):
	_widget(widget)
{
	assert(_widget != 0);
	_fbo.reset(new QGLFramebufferObject(dim));
	bind();
}


QRTT::~QRTT() {
	unbind();
}


unsigned QRTT::width() const {
	return size().width();
}


unsigned QRTT::height() const {
	return size().height();
}


QSize QRTT::size() const {
	return _fbo->size();
}


void QRTT::bind() {
	if(_widget->context() != QGLContext::currentContext())
		_widget->makeCurrent();

	assert(_fbo->bind());

	cgl::view2D(cgl::uvec2(), cgl::uvec2(width(), height()));
}


void QRTT::unbind() {
	assert(_fbo->release());
	const cgl::uvec2 dim(_widget->width(), _widget->height());
	cgl::view2D(cgl::uvec2(), dim);
}


QImage QRTT::image() const {
	return _fbo->toImage();
}
