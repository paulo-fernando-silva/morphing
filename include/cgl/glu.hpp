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

#ifndef GLU_HPP
#define GLU_HPP

#include "cgl/vec.hpp"
#include <GL/gl.h>
#include <string>


namespace cgl {


/**
 * @brief view2D a convinient function to setup a 2D view.
 * @param pos the lower left position of the viewport (in px)
 * @param dim the dimensions of the viewport (in px)
 * @param v0 the lower left point of the projection.
 * @param v1 the top right point of the projection.
 * usage:
 * 1-to-1px: view2D(uvec2(), uvec2(w, h), vec2(), vec2(w, h));
 * canonical: view2D(uvec2(), uvec2(w, h), vec2(), uvec2(1.0f));
 * where (w, h) are the desired width and height.
 */
void view2D(const uvec2& pos,
            const uvec2& dim,
            const vec2& v0 = vec2(),
            const vec2& v1 = vec2(1.0f));


/**
 * @brief dimensions get the dimensions of the 'texture'
 * @param texture
 * @return the a uvec2 containing (width, height) of the 'texture'
 */
uvec2 dimensions(const GLint texture);


/**
 * @brief Prints the most recent GL error to std::cerr.
 * @param msg some message to be printed with the error.
 */
void printGlError(const std::string& msg = "");


/**
 * @brief Draws a filled rectangle.
 * @param color the color of the rectangle.
 * @param v0 the position of the lower left corner.
 * @param v1 the position of the top right corner.
 * @param wireframe if true render just the outline.
 */
void rect(const vec4& color = vec4(1.0f),
          const vec2& v0 = vec2(0.0f),
          const vec2& v1 = vec2(1.0f),
          const bool wireframe = false);


/**
 * @brief Render a textured rectangle covered with 'texture'.
 * @param v0 the position of the lower left corner.
 * @param v1 the position of the top right corner.
 * @param vt0 the texture coordinates of the lower left corner.
 * @param vt1 the texture coordinates of the top right corner.
 */
void rect(const unsigned texture,
          const vec2& v0 = vec2(0.0f),
          const vec2& v1 = vec2(1.0f),
          const vec2& vt0 = vec2(0.0f),
          const vec2& vt1 = vec2(1.0f));


class BindColor {
public:
    /**
     * @brief Bind the 'color' to the current GL state
     * @param color the color to bind
     */
    BindColor(const vec4& color);
    /** Return GL to its old state. */
    ~BindColor();
private:
    vec4 _old_color;
};


class BindTexture2D {
public:
    /**
     * @brief Bind texture with GL 'name' to 'unit'
     * @param name the gl name of the texture to bind
     * @param unit the desired texture unit
     */
    BindTexture2D(const GLuint name, const GLuint unit = 0);
    /** @brief Return GL to its old state. */
    ~BindTexture2D();
private:
    GLenum _unit;
    bool _was_enabled;
    GLint _bound_tex;
    GLenum _old_unit;
};


} // namespace cgl


#endif //GLU_HPP

