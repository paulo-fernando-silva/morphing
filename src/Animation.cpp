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

#include "Animation.hpp"

#include <Magick++.h>
#include <vector>
#include <iostream>


const char* const FORMATS[] = { "RGBA", "RGB" };


struct Animation::PImpl {
	typedef Magick::Image Image;
	typedef std::vector<Image> Frames;
	Frames _frames;
	Animation::PixelFormat format;
};


Animation::Animation(OnFrameAdded callback, PixelFormat format)
	: _pimpl(new PImpl)
	, _frameCallback(callback)
{
	static bool once(false);

	if(not once) {
		Magick::InitializeMagick(0);
		once = true;
	}

	_pimpl->format = format;
}


Animation::~Animation() {}


bool Animation::addFrame(const unsigned w,
						 const unsigned h,
						 const byte* const bytes,
						 const unsigned delay)
{
	typedef Magick::Image Image;
	try {
		const char* const fmt(FORMATS[_pimpl->format]);
		_pimpl->_frames.push_back(Image(w, h, fmt, Magick::CharPixel, bytes));
		_pimpl->_frames.back().animationDelay(delay);
	} catch(const Magick::Exception& e) {
		std::cerr << "Animation::addFrame: Magick++ exception: "
				  << e.what() << std::endl;
		return false;
	}

	if(_frameCallback != nullptr)
		return _frameCallback(_pimpl->_frames.size());

	return true;
}


bool Animation::empty() const {
	return _pimpl->_frames.empty();
}


unsigned Animation::frameCount() {
	return _pimpl->_frames.size();
}


void Animation::clear() {
	return _pimpl->_frames.clear();
}


void Animation::reserve(const unsigned size) {
	_pimpl->_frames.reserve(size);
}


bool Animation::save(const std::string& uri) {
	if(empty() or uri.empty())
		return false;

	try {
		Magick::writeImages(_pimpl->_frames.begin(),
							_pimpl->_frames.end(),
							uri);
	} catch(const Magick::Exception& e) {
		std::cerr << "Animation::save: Magick++ exception: "
				  << e.what() << std::endl;
		return false;
	}

	return true;
}


