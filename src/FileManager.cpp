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

#include "FileManager.hpp"

#include <QPixmap>
#include <QGLWidget>
#include <QFileInfo>


FileManager::FileManager(QGLWidget* const ctx):
    _ctx(ctx)
{
    assert(_ctx != 0);
}


FileManager::Resource::Resource(const QString& uri, const unsigned glid):
    uri(uri),
    name(QFileInfo(uri).fileName()),
    glid(glid)
{}


FileManager::~FileManager() {
    clear();
}


void FileManager::clear() {
    const Iterator end(_resources.end());

    for(Iterator i(_resources.begin()); i != end; ++i)
        ctx()->deleteTexture(i->glid);

    _resources.clear();
}


void FileManager::add(const Resource& resource) {
    _resources.push_back(resource);
    emit filesChanged();
}


bool FileManager::add(const QPixmap& img, const QString& uri) {
    if(exists(uri))
        return true;

    if(ctx()->context() != QGLContext::currentContext())
        ctx()->makeCurrent();

    if(img.isNull())
        return false;

    add(Resource(uri, ctx()->bindTexture(img)));

    return true;
}


bool FileManager::loadImage(const QString& uri) {
    if(uri.isEmpty())
        return false;

    if(exists(uri))
        return true;

    return add(QPixmap(uri), uri);
}


bool FileManager::exists(const QString& uri) const {
    const ConstIterator end(_resources.end());

    for(ConstIterator i(_resources.begin()); i != end; ++i)
        if(uri == i->uri)
            return true;

    return false;
}


