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

#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <boost/scoped_ptr.hpp> // TODO: Replace by c++11 unique_ptr
#include <functional>
#include <string>


class Animation {
public:
	typedef unsigned char byte;
	typedef std::function<bool(unsigned frame)> OnFrameAdded;
	enum PixelFormat { RGBA, RGB };

	Animation(OnFrameAdded callback = nullptr, PixelFormat format = RGBA);

	~Animation();

	/**
	 * @brief addFrame add a frame to this animation
	 * @param w the width in pixels
	 * @param h the height in pixels
	 * @param bytes the image data rgba
	 * @param delay the duration of this frame in milliseconds
	 * @return true if the frame was created successfully.
	 */
	bool addFrame(unsigned w,
				  unsigned h,
				  const byte* bytes,
				  unsigned delay);

	unsigned frameCount();

	void clear();

	bool empty() const;

	void reserve(const unsigned size);


	/**
	 * @brief save file to uri
	 * @param uri the filename
	 * @return true if successful
	 */
	bool save(const std::string& uri);


private:
	struct PImpl;
	typedef boost::scoped_ptr<PImpl> PImplPtr;
	PImplPtr _pimpl;
	OnFrameAdded _frameCallback;
};

#endif // ANIMATION_HPP
