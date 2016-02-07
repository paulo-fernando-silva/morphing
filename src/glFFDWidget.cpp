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

#include "glu.hpp"
#include "QRTT.hpp"
#include "glFFDWidget.hpp"
#include "SignalBlocker.hpp"

#include <QUrl>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>

#include <iostream>
#include <cmath>


typedef glFFDWidget::vec2 vec2;
typedef glFFDWidget::color color;

const unsigned DEFAULT_RESOLUTION(10);
const unsigned MINIMUM_RESOLUTION(2);
const bool DEFAULT_DRAW_MESH_STATE(true);

const float POINT_SIZE(3.0f);
const float PROXIMITY_RADIUS(2 * POINT_SIZE);

const color CLEAR_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
const color LINE_COLOR(0.9f, 0.9f, 0.9f, 1.0f);
const color POINT_COLOR(0.0f, 1.0f, 0.0f, 1.0f);
const color POINT_SELECTION_COLOR(1.0f, 0.0f, 0.0f, 1.0f);


unsigned totalNumberOfIndices(const unsigned width) {
	assert(width != 0);
	const unsigned seg(width - 1);
	// 2 for (horizontal + vertical) dimensions x 2 indices per segment
	const unsigned num_indices(seg * width * 2 * 2);
	return num_indices;
}


glFFDWidget::glFFDWidget(QWidget* const parent,
						 QGLWidget* const shared_widget):
	QGLWidget(parent, shared_widget),
	_tex(0),
	_selection(NO_SELECTION),
	_draw_mesh(DEFAULT_DRAW_MESH_STATE),
	_modified(false),
	_resolution(DEFAULT_RESOLUTION)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setAcceptDrops(true);
	resetMesh();
}


glFFDWidget::~glFFDWidget() {
	clear();
}


void glFFDWidget::clear() {
	warnChanges();
	const SignalBlocker block(this);
	resetMesh();
	clearTex();
}


void glFFDWidget::resolution(const unsigned n) {
	warnChanges();
	{
		const SignalBlocker block(this);
		assert(n >= MINIMUM_RESOLUTION);
		_resolution = n;
		resetMesh();
	}
	emit resolutionChanged(resolution());
}


void glFFDWidget::resetMesh() {
	warnChanges();
	selectAndPropagate(NO_SELECTION);
	initMesh();
	initIndices();
}


void glFFDWidget::initializeGL() {
	glClearColor(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, CLEAR_COLOR.a);
	glPointSize(POINT_SIZE);
	cgl::view2D(uvec2(), uvec2(width(), height()));
}


void glFFDWidget::resizeGL(int width, int height) {
	cgl::view2D(uvec2(), uvec2(width, height));
}


void glFFDWidget::paintGL() {
	selectGLContext();

	glClear(GL_COLOR_BUFFER_BIT);

	if(validTex())
		drawTex();

	if(isDrawingMesh())
		draw();
}


void glFFDWidget::drawTex() const {
	assert(validTex());
	cgl::rect(tex());
}


void glFFDWidget::draw() const {
	drawMesh();
	drawSelection();
}


void glFFDWidget::drawMesh() const {
	if(mesh().empty())
		return;

	const unsigned num_idx(indices().size());
	assert(mesh().size() == (resolution() * resolution()));
	assert(num_idx == totalNumberOfIndices(resolution()));

	glVertexPointer(2, GL_FLOAT, 0, &mesh().front().x);
	glEnableClientState(GL_VERTEX_ARRAY);

	{ // draw horizontal and vertical lines followed by points.
		const cgl::BindColor state(LINE_COLOR);
		glDrawElements(GL_LINES, num_idx, GL_UNSIGNED_INT, &indices().front());
	}
	{
		const cgl::BindColor state(POINT_COLOR);
		glDrawArrays(GL_POINTS, 0, mesh().size());
	}

	glDisableClientState(GL_VERTEX_ARRAY);
}


void glFFDWidget::drawSelection() const {
	if(not hasSelection())
		return;

	const cgl::BindColor state(POINT_SELECTION_COLOR);

	glVertexPointer(2, GL_FLOAT, 0, &mesh()[selection()]);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_POINTS, 0, 1);
	glDisableClientState(GL_VERTEX_ARRAY);
}


bool glFFDWidget::square(const Mesh& msh) {
	const unsigned N(msh.size());
	const unsigned n(std::sqrt(N));
	return (n * n == N);
}


void glFFDWidget::mousePressEvent(QMouseEvent* event) {
	if(event->buttons() & Qt::LeftButton) {
		_mouse = event->pos();
		select(normalize(_mouse.x(), _mouse.y()));
	}
}


void glFFDWidget::mouseMoveEvent(QMouseEvent* event) {
	if(not (event->buttons() & Qt::LeftButton))
		return;

	if(hasSelection())
		moveSelectionTo(event->pos());
	else if((event->pos() - _mouse).manhattanLength() >=
			  QApplication::startDragDistance())
		dragEvent();
}


void glFFDWidget::moveSelectionTo(const QPoint& p) {
	assert(hasSelection());
	_mesh[selection()] = normalize(p.x(), p.y());
	postModified();
}


vec2 glFFDWidget::normalize(const int x, const int y) {
	if(width() == 0 or height() == 0)
		return vec2();

	typedef cgl::ivec2 ivec2;
	const ivec2& p(cgl::clamp(ivec2(x, height() - y),
							  ivec2(), ivec2(width(), height())));

	vec2 n(p.x / float(width()), p.y / float(height()));
	n = cgl::clamp(n, 0.0f, 1.0f);

	return n;
}


void glFFDWidget::select(const vec2& p) {
	if(mesh().empty() or width() == 0 or height() == 0)
		return;

	assert(0.0f <= p.x and p.x <= 1.0f);
	assert(0.0f <= p.y and p.y <= 1.0f);

	const vec2 scr(width(), height());
	float r_min(PROXIMITY_RADIUS / cgl::length(scr));

	int selection(NO_SELECTION);
	const unsigned N(_mesh.size());

	for(unsigned i(0); i != N; ++i) {
		vec2& pprime(_mesh[i]);
		const float r(cgl::length(p - pprime));

		if(r <= r_min) {
			selection = i;
			r_min = r;
		}
	}

	selectAndPropagate(selection);
}


void glFFDWidget::select(int i) {
	if(_selection != i) {
		assert(NO_SELECTION <= i and i < int(mesh().size()));
		_selection = i;
	}
}


void glFFDWidget::selectAndPropagate(const int i) {
	const int old(selection());

	select(i);

	if(old != selection())
		emit selectionChanged(selection());
}


bool glFFDWidget::saveMesh(std::ostream& out) {
	const ConstIterator end(mesh().end());

	for(ConstIterator i(mesh().begin()); i != end; ++i)
		out << *i << ' ';

	clearModification();

	return out.good();
}


bool glFFDWidget::loadMesh(std::istream& in) {
	Mesh msh;
	msh.reserve(DEFAULT_RESOLUTION * DEFAULT_RESOLUTION);
	vec2 v;

	while(in) {
		in >> v;
		if(in)
			msh.push_back(v);
	}

	if(not msh.empty() and square(msh)) {
		mesh(msh);
		return true;
	}

	return false;
}


void glFFDWidget::mesh(const Mesh& msh) {
	warnChanges();

	const unsigned N(msh.size());
	const unsigned n(std::sqrt(N));

	assert(n * n == N);

	selectAndPropagate(NO_SELECTION);
	_resolution = n;
	_mesh = msh;
	initIndices();
	emit resolutionChanged(resolution());
	clearModification();
}


void glFFDWidget::drawMesh(const bool status) {
	_draw_mesh = status;
}


void glFFDWidget::clearTex() {
	tex(0);
}


void glFFDWidget::tex(const GLuint tx) {
	if(_tex != tx) {
		_tex = tx;
		postModified();
	}
}


void glFFDWidget::initMesh() {
	assert(resolution() != 0);

	const unsigned width(resolution());
	const float d(1.0f / (width - 1));
	float y(0.0f);

	_mesh.clear();
	_mesh.reserve(width * width);

	for(unsigned i(0); i != width; ++i) {
		float x(0.0f);

		for(unsigned j(0); j != width; ++j, x += d)
			_mesh.push_back(vec2(x, y));

		y += d;
	}

	assert(_mesh.size() == (width * width));
}


void glFFDWidget::initIndices() {
	const unsigned width(resolution());
	assert(width != 0);
	const unsigned seg(width - 1);
	const unsigned num_indices(totalNumberOfIndices(width));
	_indices.clear();
	_indices.reserve(num_indices);

	// horizontal lines
	for(unsigned i(0), k(0); i != width; ++i, k += width)
		for(unsigned j(0); j != seg; ++j) {
			_indices.push_back(k + j);
			_indices.push_back(k + j + 1);
		}

	// vertical lines
	for(unsigned j(0); j != width; ++j)
		for(unsigned i(0), k(0); i != seg; ++i, k += width) {
			_indices.push_back(k + j);
			_indices.push_back(k + j + width);
		}

	assert(_indices.size() == num_indices);
}


void glFFDWidget::warnChanges() {
	if(modified()) {
		emit destructiveChange();
		clearModification();
	}
}


void glFFDWidget::clearModification() {
	_modified = false;
}


void glFFDWidget::postModified() {
	_modified = true;
}


void glFFDWidget::dragEnterEvent(QDragEnterEvent* event) {
	emit dragEnter(event);
}


void glFFDWidget::dropEvent(QDropEvent* event) {
	emit dropped(event, this);
}


QImage glFFDWidget::frame() {
	if(validTex()) {
		const bool mesh_state(isDrawingMesh());
		drawMesh(false);

		selectGLContext();

		const cgl::uvec2& dim(cgl::dimensions(tex()));
		QRTT rtt(this, QSize(dim.x, dim.y));

		paintGL();

		drawMesh(mesh_state);

		return rtt.toImage();
	}

	return QImage();
}


void glFFDWidget::dragEvent() {
	if(not uri().isEmpty() and validTex()) {
		QList<QUrl> urls;
		urls.append(QUrl(uri()));

		QMimeData* const mime_data(new QMimeData);
		mime_data->setUrls(urls);

		QDrag* const drag(new QDrag(this));
		drag->setMimeData(mime_data);
		drag->setPixmap(QPixmap::fromImage(frame()));
		drag->exec();
	}
}


void glFFDWidget::uri(const QString& new_uri) {
	_uri = new_uri;
}


void glFFDWidget::selectGLContext() {
	if(context() != QGLContext::currentContext())
		makeCurrent();
}


