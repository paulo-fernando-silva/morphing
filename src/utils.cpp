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

#include "utils.hpp"
#include "cgl/glu.hpp"
#include <iostream>
#include <cmath>


void interpolate(const Mesh& a,
                 const Mesh& b,
                 const float& t,
                 Mesh& msh)
{
    assert(0.0f <= t and t <= 1.0f);
    assert(not a.empty());
    const unsigned a_size(a.size());
    const unsigned b_size(b.size());
    assert(a_size == b_size);

    msh.resize(a_size);

    for(unsigned i(0); i != a_size; ++i)
        msh[i] = a[i] * (1.0f - t) + b[i] * t;
}


bool glBlendAlphaEquals(const float& a) {
    float bc[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
    glGetFloatv(GL_BLEND_COLOR, bc);
    const bool equal(bc[3] == a);

    if(not equal) {
        cgl::printGlError();
        return false;
    }

    return true;
}


void draw(const int tex,
          const Mesh& mesh,
          const Mesh& tc,
          const Faces& faces)
{
    assert(tex != 0);
    assert(not mesh.empty());
    assert(mesh.size() == tc.size());
    assert(not faces.empty());

    const cgl::BindTexture2D bind(tex);

    glTexCoordPointer(2, GL_FLOAT, 0, &tc.front().x);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, &mesh.front().x);
    glEnableClientState(GL_VERTEX_ARRAY);

    glDrawElements(GL_TRIANGLES, faces.size() * 3,
                   GL_UNSIGNED_INT, &faces.front().a);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void generateTriangles(const unsigned xdiv,
                       const unsigned ydiv,
                       Faces& faces)
{
    const unsigned xpts(xdiv + 1); // number of points in x
    const unsigned NF(2 * xdiv * ydiv); // total number of faces

    faces.reserve(NF);

    for(unsigned y(0); y != ydiv; ++y)
        for(unsigned x(0); x != xdiv; ++x) {
            const unsigned a(y * xpts + x);
            const unsigned b(a + 1);
            const unsigned c(b + xdiv);
            const unsigned d(c + 1);

            faces.push_back(Trig(a, b, d));
            faces.push_back(Trig(a, d, c));
        }

    assert(faces.size() == NF);
}


void drawBlended(const Mesh& src_mesh,
                 const Mesh& dst_mesh,
                 const Faces& faces,
                 const int src_tex,
                 const int dst_tex,
                 const float t)
{
    Mesh mesh;
    interpolate(src_mesh, dst_mesh, t, mesh);

    glDisable(GL_BLEND);
    draw(src_tex, mesh, src_mesh, faces);

    glEnable(GL_BLEND);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

    if(not glBlendAlphaEquals(t)) {
        glBlendColor(0.0f, 0.0f, 0.0f, t);
        assert(glBlendAlphaEquals(t));
    }

    draw(dst_tex, mesh, dst_mesh, faces);
    glDisable(GL_BLEND);
}


