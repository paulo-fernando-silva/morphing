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

#include "SaveHelper.hpp"
#include "glBlendWidget.hpp"

#include <algorithm>
#include <cassert>


const unsigned MIN_FRAMES(1);


SaveHelper::SaveHelper(Blender* const blender,
                       const unsigned max_fps):
    _blender(blender),
    _max_fps(max_fps)
{
    assert(blender != 0);
    saveBlenderState();
    setup();
}


SaveHelper::~SaveHelper() {
    loadBlenderState();
}


void SaveHelper::saveBlenderState() {
    _animated = _blender->animated();
    _anim_dir = _blender->animationDirection();
    _fps = _blender->fps();
    _t = _blender->widget()->blendFactor();
}


void SaveHelper::loadBlenderState() {
    _blender->animated(_animated);
    _blender->animationDirection(_anim_dir);
    _blender->fps(_fps);
    _blender->widget()->blendFactor(_t);
}


void SaveHelper::setup() {
    _blender->animated(false);
    _blender->animationDirection(Blender::FRONT);
    _blender->fps(std::min(std::max(_fps, MIN_FRAMES), _max_fps));
    _blender->widget()->blendFactor(0.0f);
}


unsigned SaveHelper::delay() const {
    return _max_fps / _blender->fps();
}




