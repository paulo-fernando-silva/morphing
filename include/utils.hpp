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

#ifndef UTILS_HPP
#define UTILS_HPP

#include "vec.hpp"
#include <vector>
#include <cassert>


struct Trig {
	Trig(const unsigned a,
		 const unsigned b,
		 const unsigned c):
		a(a),
		b(b),
		c(c)
	{
		assert(a != b);
		assert(b != c);
		assert(c != a);
	}

	unsigned a, b, c;
};


typedef cgl::vec2 vec2;
typedef cgl::uvec2 uvec2;
typedef cgl::vec4 color;
typedef std::vector<vec2> Mesh;
typedef std::vector<Trig> Faces;


void interpolate(const Mesh& a,
				 const Mesh& b,
				 const float& t,
				 Mesh& msh);


bool glBlendAlphaEquals(const float& a);


void draw(const int tex,
		  const Mesh& mesh,
		  const Mesh& tc,
		  const Faces& faces);


void generateTriangles(const unsigned xdiv,
					   const unsigned ydiv,
					   Faces& faces);


void drawBlended(const Mesh& src_mesh,
				 const Mesh& dst_mesh,
				 const Faces& faces,
				 const int src_tex,
				 const int dst_tex,
				 const float t);


#endif // UTILS_HPP
