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
#include "QRTT.hpp"

#include <QSize>
#include <QPoint>
#include <QImage>
#include <QGLWidget>
#include <memory>


class glFFDWidget;


class glBlendWidget : public QGLWidget {
	Q_OBJECT
public:
	glBlendWidget(QWidget* const parent);

	~glBlendWidget();


	void src(glFFDWidget* const w);


	glFFDWidget* src() const;


	void dst(glFFDWidget* const w);


	glFFDWidget* dst() const;


	const float& blendFactor() const;


	void updateFaces();


	bool canPaint() const;

	QSize maxImgDim();

	QSize minImgDim();

	QSize fboDim();

	const QRTT::BytePtr& pixels() const;

	void beginAnimation(const QSize& size);

	void paint();

	QImage frame();

	void endAnimation();


signals:
	void blendFactorChanged(float t);


public slots:
	void blendFactor(float t);


protected:
	void paintGL();

	void initializeGL();
	void resizeGL(int width, int height);

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);

	cgl::uvec2 dimensions(GLint tex);

	const Faces& faces() const;

	bool invariant() const;

	void dragEvent();


private:
	typedef cgl::uvec2 (*Extreme)(const cgl::uvec2&, const cgl::uvec2&);

	QSize imgDim(const Extreme ext);


private:
	typedef std::unique_ptr<QRTT> QRTTPtr;

	float _t;
	glFFDWidget* _src;
	glFFDWidget* _dst;
	Faces _faces;
	QPoint _mouse_press_pos;
	QRTTPtr _rtt;

};


#endif // GLBLENDWIDGET_HPP
