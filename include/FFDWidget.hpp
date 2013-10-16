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

#ifndef FFDWIDGET_HPP
#define FFDWIDGET_HPP

#include <QWidget>
#include <QString>
#include <boost/shared_ptr.hpp>


class glFFDWidget;
class FileManager;

class QLabel;
class QComboBox;
class QGLWidget;


class FFDWidget : public QWidget {
    Q_OBJECT
public:
    typedef boost::shared_ptr<FileManager> FileManagerPtr;

    FFDWidget(QWidget* const parent,
              const QString& title,
              QGLWidget* const shared_widget,
              const FileManagerPtr& file_mgr);


    void update();

    void clear();


    inline glFFDWidget* widget() const {
        return _widget;
    }


    inline const FileManagerPtr& mgr() const {
        return _file_mgr;
    }


    QString selectionURI() const;

    int selection() const;

    void select(const int i);

    bool hasSelection() const;


public slots:
    void tex(int);

    void filesChanged();


private:
    void setupUI(const QString& title,
                 QGLWidget* const shared_widget);

    inline QComboBox* files() const {
        return _files;
    }


private:
    glFFDWidget* _widget;
    QLabel* _title;
    QComboBox* _files;
    FileManagerPtr _file_mgr;
};

#endif // FFDWIDGET_HPP
