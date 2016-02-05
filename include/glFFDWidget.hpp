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

#ifndef GLWIDGET_HPP
#define GLWIDGET_HPP


#include "cgl/vec.hpp"

#include <QPoint>
#include <QString>
#include <QGLWidget>

#include <vector>
#include <iosfwd>


class glFFDWidget : public QGLWidget {
	Q_OBJECT
public:
	typedef cgl::vec2 vec2;
	typedef cgl::uvec2 uvec2;
	typedef cgl::vec4 color;

	typedef std::vector<vec2> Mesh;
	typedef Mesh::iterator Iterator;
	typedef Mesh::const_iterator ConstIterator;
	typedef std::vector<unsigned> Indices;

	static const int NO_SELECTION = -1;


	glFFDWidget(QWidget* const parent, QGLWidget* const shared_widget);

	~glFFDWidget();

	void clear();


	static bool square(const Mesh& msh);

	void resetMesh();

	void mesh(const Mesh& msh);

	void resolution(const unsigned n);

	void drawMesh(const bool status);

	inline bool isDrawingMesh() const {
		return _draw_mesh;
	}

	inline const Mesh& mesh() const {
		return _mesh;
	}

	inline unsigned resolution() const {
		return _resolution;
	}


	vec2 normalize(const int x, const int y);


	void selectAndPropagate(const int i);

	void select(const vec2& p);

	inline int selection() const {
		return _selection;
	}

	inline bool hasSelection() const {
		return _selection != NO_SELECTION;
	}


	void clearTex();

	void tex(const GLuint tx);

	inline GLuint tex() const {
		return _tex;
	}

	bool validTex() const {
		return tex() != 0; // 0 is not a valid gl image id
	}


	void uri(const QString& new_uri);
	inline const QString& uri() const {
		return _uri;
	}


	bool saveMesh(std::ostream& out);
	bool loadMesh(std::istream& in);

	void clearModification();

	inline bool modified() const {
		return _modified;
	}


	QImage frame();


signals:
	void selectionChanged(int new_selection);

	void resolutionChanged(int new_resolution);

	void destructiveChange();

	void dragEnter(QDragEnterEvent* event);
	void dropped(QDropEvent* event, QWidget* sender);


public slots:
	void select(int i);


protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);

	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);

	void moveSelectionTo(const QPoint& p);

	void dragEvent();


private:
	void draw() const;
	void drawTex() const;
	void drawMesh() const;
	void drawSelection() const;

	inline const Indices& indices() const {
		return _indices;
	}

	void initMesh();
	void initIndices();

	void warnChanges();
	void postModified();

	void selectGLContext();


private:
	GLuint _tex;
	int _selection;
	bool _draw_mesh;
	bool _modified;
	unsigned _resolution;
	Mesh _mesh;
	Indices _indices;
	QPoint _mouse;
	QString _uri;
};


#endif
