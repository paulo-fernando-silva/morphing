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

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <QString>
#include <QObject>
#include <vector>
#include <cassert>


class QGLWidget;


class FileManager : public QObject {
    Q_OBJECT
public:
    struct Resource {
        Resource(const QString& uri, const unsigned glid);
        QString uri;
        QString name;
        unsigned glid;
    };

    typedef std::vector<Resource> Resources;
    typedef Resources::iterator Iterator;
    typedef Resources::const_iterator ConstIterator;
    typedef Resources::size_type Size;

    FileManager(QGLWidget* const ctx);
    ~FileManager();

    void clear();

    void add(const Resource& resource);


    inline bool empty() const {
        return _resources.empty();
    }


    inline Size size() const {
        return _resources.size();
    }


    inline const Resource& resource(const unsigned i) const {
        assert(i < _resources.size());
        return _resources[i];
    }


    bool loadImage(const QString& uri);

    inline QGLWidget* ctx() const {
        return _ctx;
    }


signals:
    void filesChanged();


private:
    QGLWidget* _ctx;
    Resources _resources;
};


#endif // FILEMANAGER_HPP
