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

#ifndef FFDAPP_HPP
#define FFDAPP_HPP

#include <QIcon>
#include <QString>
#include <QMainWindow>
#include <boost/shared_ptr.hpp>


class FFDWidget;
class Blender;
class FileManager;

class QUrl;
class QTimer;
class QLabel;
class QAction;
class QWidget;
class QSpinBox;
class QCheckBox;
class QGroupBox;
class QRadioButton;
class QXmlStreamReader;


class FFDApp : public QMainWindow {
	Q_OBJECT
public:
    typedef boost::shared_ptr<FileManager> FileManagerPtr;

    FFDApp();
    ~FFDApp();

    bool loadProject(const QString& uri);
    bool saveProject(const QString& uri);

    bool loadImage(const QString& uri, QWidget* const sender);
    bool saveAnimation(const QString& uri);

    inline Blender* mix() const {
        return _mix;
    }

    inline FFDWidget* src() const {
        return _src;
    }

    inline FFDWidget* dst() const {
        return _dst;
    }

    inline const FileManagerPtr& mgr() const {
        return _file_mgr;
    }


public slots:
    void openImages();
    void openProject();
    void saveProject();
    void saveProjectAs();
    void saveProjectChanges();
    void saveAnimation();
    void saveAnimationAs();
    void toggleAnimation();
    void clear();
    void about();
    void aboutQt();
    void update();
    void fps(int n);
    void len(int n);
    void resolutionChanged(int n);
    void toggleMesh();
    void bidirectionality();
    void clearModifications();

    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void dropEvent(QDropEvent* event, QWidget* sender);


protected:
    void closeEvent(QCloseEvent*);
    bool validAnimation() const;


private:
    void setupTimer();

    void setupToolbar();
    void setupMenus();

    bool load(const QString& uri,
              const QString& mesh,
              FFDWidget* const ffdw);

    void setupDataUI();
    void initDataUI();
    void layoutDataUI();
    void connectDataUI();

    QWidget* setupOptionsUI(QWidget* const parent);
    void initOptionsUI();
    void layoutOptionsUI();

    QString selectedAnimMask() const;

    void handleUrls(QDropEvent* event, QWidget* sender);
    void process(const QUrl& url, QWidget* sender);
    void handleImage(QDropEvent* event, QWidget* sender);

    bool parseProject(QXmlStreamReader& xml, const QString& prj_uri);

    void onLoadResult(const bool success, const QString& uri);
    void onSaveResult(const bool success, const QString& uri);
    void onResult(const bool success,
                  const QString& action,
                  const QString& uri);

    QString genImgURI();


private:
    Blender* _mix;
    FFDWidget* _src;
    FFDWidget* _dst;

    QTimer* _timer;

    QMenu* _file_menu;
    QMenu* _help_menu;

    QAction* _about;
    QAction* _about_qt;

    QAction* _new;
    QAction* _load_img;
    QAction* _load_prj;
    QAction* _save_prj;
    QAction* _save_prj_as;
    QAction* _save_anim;
    QAction* _save_anim_as;
    QAction* _play;
    QAction* _mesh;

    QGroupBox* _opts_ui;
    QLabel* _mesh_lbl;
    QSpinBox* _mesh_sb;
    QLabel* _fps_lbl;
    QSpinBox* _fps_sb;
    QLabel* _len_lbl;
    QSpinBox* _len_sb;
    QCheckBox* _bidirectional;

    QIcon _play_icon;
    QIcon _pause_icon;

    FileManagerPtr _file_mgr;

    QString _prj_uri;
    QString _img_uri;
    QString _anim_uri;
    QString _img_filters;
    unsigned _curr_img_id;
};


#endif // FFDAPP_HPP
