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

#include "FFDWidget.hpp"
#include "glFFDWidget.hpp"
#include "FileManager.hpp"
#include "SignalBlocker.hpp"

#include <QLabel>
#include <QComboBox>
#include <QGridLayout>


const int NO_SELECTION(-1);


FFDWidget::FFDWidget(QWidget* const parent,
                     const QString& title,
                     QGLWidget* const shared_widget,
                     const FileManagerPtr& file_mgr):
    QWidget(parent),
    _widget(0),
    _title(0),
    _files(0),
    _file_mgr(file_mgr)
{
    setupUI(title, shared_widget);
    connect(mgr().get(), SIGNAL(filesChanged()), this, SLOT(filesChanged()));
}


void FFDWidget::setupUI(const QString& title, QGLWidget *const shared_widget) {
    _widget = new glFFDWidget(this, shared_widget);

    _title = new QLabel(title);
    _title->setAlignment(Qt::AlignCenter);

    _files = new QComboBox();
    connect(_files, SIGNAL(currentIndexChanged(int)), this, SLOT(tex(int)));

    QGridLayout* const layout(new QGridLayout);

    layout->addWidget(_title, 0, 0);
    layout->addWidget(widget(), 1, 0);
    layout->addWidget(_files, 2, 0);

    setLayout(layout);
}


void FFDWidget::tex(int idx) {
    typedef FileManager::Resource Resource;

    if(0 <= idx and unsigned(idx) < mgr()->size()) {
        const Resource& resource(mgr()->resource(idx));
        files()->setToolTip(resource.uri);
        widget()->tex(resource.glid);
    }
}


void FFDWidget::filesChanged() {
    typedef FileManager::Size Size;

    const SignalBlocker blocker(files());
    files()->clear();

    const Size N(mgr()->size());

    for(Size i(0); i != N; ++i)
        files()->addItem(mgr()->resource(i).name);

    if(N != 0) {
        files()->setCurrentIndex(N - 1);
        tex(files()->currentIndex());
    } else
        clear();
}


void FFDWidget::update() {
    widget()->updateGL();
}


void FFDWidget::clear() {
    widget()->clear();
    files()->clear();
    files()->setToolTip("");
}


int FFDWidget::selection() const {
    return files()->currentIndex();
}


void FFDWidget::select(const int i) {
    assert(NO_SELECTION <= i and i < files()->count());

    files()->setCurrentIndex(i);
}


QString FFDWidget::selectionURI() const {
    const int sel(selection());

    if(sel == NO_SELECTION)
        return QString();

    return mgr()->resource(sel).uri;
}


bool FFDWidget::hasSelection() const {
    return selection() != NO_SELECTION;
}

