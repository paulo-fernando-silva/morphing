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

#ifndef QRTT_HPP
#define QRTT_HPP

#include <QSize>
#include <QImage>
#include <memory>


class QGLWidget;
class QGLFramebufferObject;


class QRTT {
public:
	typedef unsigned char byte;
	typedef std::unique_ptr<byte> BytePtr;

	QRTT(QGLWidget* const widget, const QSize& dim);

	~QRTT();

	void grabPixels();

	const BytePtr& pixels() const;

	unsigned width() const;
	unsigned height() const;

	QImage toImage() const;

	void bind();
	void unbind();


private:
	QRTT(QRTT&) = delete;
	QRTT& operator=(QRTT&) = delete;

	typedef std::unique_ptr<QGLFramebufferObject> FBOPtr;
	QGLWidget* _widget;
	BytePtr _pixels;
	FBOPtr _fbo;
};


#endif // QRTT_HPP
