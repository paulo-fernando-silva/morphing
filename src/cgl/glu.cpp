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

#include "cgl/glu.hpp"

#include <iostream>
#include <iomanip>
#include <cassert>


namespace cgl {


void printGlError(const std::string& msg) {
	GLenum err = glGetError();
	const std::string total_msg((msg.empty()? "" : "msg:'" + msg + "\' "));

	if(err != GL_NO_ERROR)
		std::cerr   << "glError: " << total_msg
					<< " no:'" << std::hex << err << "'\n";
}


void rect(const vec4& color,
		  const vec2& v0,
		  const vec2& v1,
		  const bool wireframe)
{
	const BindColor state(color);
	GLenum mode(GL_TRIANGLE_STRIP);

	vec2 vp[4];
	vp[0] = v0;
	vp[1] = vec2(v1.x, v0.y);

	if(wireframe) {
		vp[2] = v1;
		vp[3] = vec2(v0.x, v1.y);
		mode = GL_LINE_LOOP;
	} else {
		vp[2] = vec2(v0.x, v1.y);
		vp[3] = v1;
	}

	glVertexPointer(2, GL_FLOAT, 0, vp);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(mode, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
}



void rect(const unsigned texture,
		  const vec2& v0,
		  const vec2& v1,
		  const vec2& vt0,
		  const vec2& vt1)
{
	if(!glIsTexture(texture))
		return;

	const BindTexture2D bind(texture);
	GLint old_state(0);
	glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &old_state);

	if(old_state != GL_MODULATE)
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	vec2 vp[4] = {v0, vec2(v1.x, v0.y), vec2(v0.x, v1.y), v1};
	vec2 vtp[4] = {vt0, vec2(vt1.x, vt0.y), vec2(vt0.x, vt1.y), vt1};

	glTexCoordPointer(2, GL_FLOAT, 0, vtp);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vp);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(old_state != GL_MODULATE)
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, old_state);
}


void view2D(const uvec2& pos,
			const uvec2& dim,
			const vec2& v0,
			const vec2& v1)
{
	glViewport(pos.x, pos.y, dim.x, dim.y);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(v0.x, v1.x, v0.y, v1.y, -1.0f, 1.0f);
}


uvec2 dimensions(const GLint texture) {
	if(texture == 0)
		return uvec2();

	const BindTexture2D binder(texture);
	ivec2 idim;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &idim.x);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &idim.y);
	const uvec2 dim(max(idim, 0));
	return dim;
}


BindColor::BindColor(const vec4& color) {
	glGetFloatv(GL_CURRENT_COLOR, &_old_color.x);
	glColor4f(color.r, color.g, color.b, color.a);
}


BindColor::~BindColor() {
	glColor4f(_old_color.r, _old_color.g, _old_color.b, _old_color.a);
}




BindTexture2D::BindTexture2D(const GLuint name, const GLuint unit):
	_unit(unit),
	_was_enabled(false),
	_bound_tex(0),
	_old_unit(GL_TEXTURE0)
{
	assert(name != 0);

	glGetIntegerv(GL_ACTIVE_TEXTURE, reinterpret_cast<GLint*>(&_old_unit));
	if(unit != (_old_unit - GL_TEXTURE0))
		glActiveTexture(GL_TEXTURE0 + unit);

	_was_enabled = glIsEnabled(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &_bound_tex);
	glBindTexture(GL_TEXTURE_2D, name);
}


BindTexture2D::~BindTexture2D() {
	glBindTexture(GL_TEXTURE_2D, _bound_tex);

	if(not _was_enabled)
		glDisable(GL_TEXTURE_2D);

	if(_unit != (_old_unit - GL_TEXTURE0))
		glActiveTexture(_old_unit);
}


} // namespace cgl

