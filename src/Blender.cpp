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

#include "Blender.hpp"
#include "glBlendWidget.hpp"
#include "Animation.hpp"
#include "SaveHelper.hpp"
#include "QRTT.hpp"
#include "SignalBlocker.hpp"

#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <cassert>


const float DEFAULT_BLEND_FACTOR(0.5f);
const unsigned DEFAULT_LENGTH(1000);
const unsigned MAX_FPS(10); // Magick++ max fps (?)
const bool DEFAULT_ANIMATION_STATE(false);
const Blender::AnimDir DEFAULT_ANIMATION_DIRECTION(Blender::FRONT);
const bool DEFAULT_DIRECTIONAL_STATE(true);


Blender::Blender(QWidget* const parent, const QString& title):
    QWidget(parent),
    _anim_dir(DEFAULT_ANIMATION_DIRECTION),
    _widget(0),
    _title(0),
    _slider(0),
    _animated(DEFAULT_ANIMATION_STATE),
    _bidirectional(DEFAULT_DIRECTIONAL_STATE),
    _duration(DEFAULT_LENGTH),
    _fps(MAX_FPS)
{
    setupUI(title);
    clear();
}


void Blender::clear() {
    animationDirection(DEFAULT_ANIMATION_DIRECTION);
    animated(DEFAULT_ANIMATION_STATE);
    bidirectional(DEFAULT_DIRECTIONAL_STATE);
    duration(DEFAULT_LENGTH);
    fps(MAX_FPS);

    widget()->blendFactor(DEFAULT_BLEND_FACTOR);
}


void Blender::setupUI(const QString& title) {
    _widget = new glBlendWidget(this);

    _title = new QLabel(title);
    _title->setAlignment(Qt::AlignCenter);

    _slider = new QSlider(Qt::Horizontal);
    slider()->setRange(0, 100);
    slider()->setSingleStep(1);
    slider()->setPageStep(100);
    slider()->setTickInterval(5);
    slider()->setTickPosition(QSlider::TicksRight);
    connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(percent(int)));

    connect(widget(), SIGNAL(blendFactorChanged(float)),
            this, SLOT(blendFactorChanged(float)));

    QGridLayout* const layout(new QGridLayout);

    layout->addWidget(_title, 0, 0);
    layout->addWidget(widget(), 1, 0);
    layout->addWidget(slider(), 2, 0);

    setLayout(layout);
}


void Blender::update() {
    if(animated())
        stepAnimation();

    widget()->updateGL();
}


void Blender::stepAnimation() {
    const float num_frames(numberOfFrames());
    float frame_number(frameNumber());

    assert(0.0f <= frame_number and frame_number < num_frames);

    frame_number += animationDirection();

    if(frame_number >= num_frames) {
        if(bidirectional()) {
            frame_number = std::max(0.0f, num_frames - 2.0f);
            _anim_dir = BACK;
        } else
            frame_number = 0.0f;
    } else if(frame_number < 0.0f) {
        frame_number = 1.0f;
        _anim_dir = FRONT;
    }

    frameNumber(frame_number);
}


void Blender::animationDirection(const AnimDir dir) {
    _anim_dir = dir;
}


void Blender::bidirectional(const bool state) {
    _bidirectional = state;
}


void Blender::animated(const bool state) {
    _animated = state;
}


void Blender::duration(int n) {
    if(0 < n)
        _duration = n;
}


void Blender::fps(int f) {
    if(f >= 0)
        _fps = f;
}


void Blender::frameNumber(const float n) {
    const unsigned number_of_frames(numberOfFrames());
    assert(n < number_of_frames);
    const float t(n / (number_of_frames - 1));
    widget()->blendFactor(t);
}


float Blender::frameNumber() const {
    const unsigned number_of_frames(numberOfFrames());
    assert(number_of_frames != 0);
    const float n(widget()->blendFactor() * (number_of_frames - 1));
    return n;
}


void Blender::percent(int p) {
    const float t(p / 100.0f);

    assert(0.0f <= t and t <= 1.0f);

    if(widget()->blendFactor() != t)
        widget()->blendFactor(t);
}


void Blender::blendFactorChanged(float t) {
    const SignalBlocker block(slider());
    assert(0.0f <= t and t <= 1.0f);
    const int d(slider()->maximum() - slider()->minimum());
    const int percent(slider()->minimum() + d * t);
    slider()->setValue(percent);
}


bool Blender::save(const QString& uri) {
    assert(uri.size() != 0);

    const SaveHelper sh(this, MAX_FPS);
    unsigned number_of_frames(numberOfFrames());
    assert(number_of_frames != 0);

    if(bidirectional())
        number_of_frames = number_of_frames * 2 - 1;

    const unsigned delay(sh.delay());
    QRTT rtt(widget());
    const unsigned w(rtt.width()), h(rtt.height());
    Animation animation;
    animation.reserve(number_of_frames);

    for(unsigned i(0); i != number_of_frames; ++i) {
        widget()->paintGL();
        rtt.grabFrame();

        if(not animation.addFrame(w, h, rtt.pixels().get(), delay))
            return false;

        stepAnimation();
    }

    return animation.save(uri.toStdString());
}




