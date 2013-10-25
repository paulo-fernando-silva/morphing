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

#include <QImage>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>

class glBlendWidget;
class QGLFramebufferObject;


/**
 * @brief The QRTT class creates an fbo for rtt using the dimensions
 * of the biggest attached image.
 * The fbo will be attached from QRTT to ~QRTT.
 * During this period the frames can be drawn to the fbo and retrieved using
 * the pair grabPixels() and pixels().
 * NOTE: This class is specific for (coupled to) the the glBlendWidget.
 * For a more generic approach use QGLFramebufferObject directly.
 * @pre at least one, source or destination, widget must have an image selected.
 */
class QRTT {
public:
    typedef unsigned char byte;
    typedef boost::scoped_array<byte> BytePtr;

    QRTT(glBlendWidget* const blender);

    ~QRTT();

    void grabPixels();

    inline const BytePtr& pixels() const {
        return _pixels;
    }

    unsigned width() const;

    unsigned height() const;


    QImage toImage() const;


private:
    void createFramebuffer();
    void bind();
    void unbind();


private:
    typedef boost::scoped_ptr<QGLFramebufferObject> FBOPtr;
    glBlendWidget* _blender;
    BytePtr _pixels;
    FBOPtr _fbo;
};


#endif // QRTT_HPP
