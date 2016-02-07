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
#include <functional>


class glBlendWidget;

class QLabel;
class QSlider;
class Animation;


class Blender : public QWidget {
	Q_OBJECT
public:
	enum AnimDir {FRONT = 1, BACK = -1};

	Blender(QWidget* const parent, const QString& title);


	void update();

	void clear();


	glBlendWidget* widget() const;

	// animation duration in milliseconds
	unsigned duration() const;

	unsigned fps() const;


	void animationDirection(const AnimDir dir);

	AnimDir animationDirection() const;


	void bidirectional(const bool state);

	bool bidirectional() const;


	void animated(const bool state);

	bool animated() const;


	void frameNumber(const float n);
	float frameNumber() const;

	unsigned unidirectionalNumberOfFrames() const;
	unsigned bidirectionalNumberOfFrames() const;
	unsigned totalNumberOfFrames() const;

	void generate(Animation& animation);


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

	QSlider* slider() const;


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
