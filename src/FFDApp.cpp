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

#include "FFDApp.hpp"
#include "Blender.hpp"
#include "FFDWidget.hpp"
#include "glFFDWidget.hpp"
#include "FileManager.hpp"
#include "glBlendWidget.hpp"
#include "SignalBlocker.hpp"

#include <QTimer>
#include <QAction>
#include <QFileDialog>
#include <QStringList>
#include <QGridLayout>
#include <QStatusBar>
#include <QMenuBar>
#include <QFileInfo>
#include <QToolBar>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMessageBox>
#include <QSpinBox>
#include <QLabel>
#include <QGLContext>
#include <QSpacerItem>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

#include <boost/bind.hpp>


using namespace std;

enum Tag {
    PROJECT_TAG,
    SRC_TAG,
    DST_TAG,
    URI_TAG,
    FPS_TAG,
    LEN_TAG
};


const QString TAG[] = {
    "project",
    "src",
    "dst",
    "uri",
    "fps",
    "len"
};


const unsigned DEFAULT_FPS(30);
const unsigned FPS_STEP(1);
const unsigned MIN_FPS(0);
const unsigned MAX_FPS(60);

const unsigned DEFAULT_LEN(1000);
const unsigned LEN_STEP(100);
const unsigned MIN_LEN(LEN_STEP);
const unsigned MAX_LEN(10000);

const unsigned DEFAULT_RES(10);
const unsigned RES_STEP(1);
const unsigned MIN_RES(2);
const unsigned MAX_RES(100);

const unsigned MSG_DELAY(10000);

const unsigned DEFAULT_IMG_SIZE_PX(400);

const QString PRJ_EXT("xml");
const QString GIF_EXT("gif");
const QString MPG_EXT("mpg");

const bool DEFAULT_GIF_CHECKED(true);
const bool DEFAULT_MPG_CHECKED(false);

const QString URL("https://sites.google.com/site/paulofernandosilva");
const QString LINK("<a href='" + URL + "'>my website</a>");
const QString TITLE("FFDApp Morphing Example");
const QString ABOUT("<h4>2D Free Form Deformation morphing </h4>"
                    "<p style=\"font:12px;\">"
                    "Use the File menu to load images or a project file. "
                    "Select one image as source and another as destination,"
                    " each using the respective combo box. "
                    "Move the points on the control meshes"
                    " until you have reached the desired effect. "
                    "Click animate for a continous animation. "
                    "Set the animation time and frames per second"
                    " and save the resulting animation to a file.<br>"
                    "For more random code go to " + LINK + ".<br>Enjoy :)"
                    "</p>");

FFDApp::FFDApp():
    _mix(0),
    _src(0),
    _dst(0),
    _frame(0),
    _file_menu(0),
    _help_menu(0),
    _about(0),
    _about_qt(0),
    _new(0),
    _load_img(0),
    _load_prj(0),
    _save_prj(0),
    _save_anim(0),
    _play(0),
    _mesh(0),

    _params_ui(0),
    _mesh_lbl(0),
    _mesh_sb(0),

    _anim_opts(0),
    _fps_lbl(0),
    _fps_sb(0),
    _len_lbl(0),
    _len_sb(0),
    _gif(0),
    _mpg(0),
    _bidirectional(0)
{
    setupTimer();
    setupToolbar();
    setupMenus();
	setupUI();

    clear();

    fps(DEFAULT_FPS); // start the frame update timer.
}


FFDApp::~FFDApp() {
    clear();
}


void FFDApp::setupUI() {
    setCentralWidget(new QWidget);
    initUI();
    layoutUI();
}


void FFDApp::initUI() {
    _mix = new Blender(centralWidget(), "Interpolated Image");
    glBlendWidget* const bw(_mix->widget());

    _file_mgr.reset(new FileManager(bw));

    _src = new FFDWidget(centralWidget(), "Source Image", bw, _file_mgr);
    _dst = new FFDWidget(centralWidget(), "Destination Image", bw, _file_mgr);

    glFFDWidget* const src_wgt(_src->widget());
    glFFDWidget* const dst_wgt(_dst->widget());

    assert(src_wgt->isSharing() and dst_wgt->isSharing());

    _mix->widget()->src(src_wgt);
    _mix->widget()->dst(dst_wgt);

    connect(src_wgt, SIGNAL(selectionChanged(int)), dst_wgt, SLOT(select(int)));
    connect(dst_wgt, SIGNAL(selectionChanged(int)), src_wgt, SLOT(select(int)));

    connect(src_wgt, SIGNAL(destructiveChange()),
            this, SLOT(saveProjectChanges()));
    connect(dst_wgt, SIGNAL(destructiveChange()),
            this, SLOT(saveProjectChanges()));

    connect(src_wgt, SIGNAL(resolutionChanged(int)),
            this, SLOT(resolutionChanged(int)));
    connect(dst_wgt, SIGNAL(resolutionChanged(int)),
            this, SLOT(resolutionChanged(int)));
}


void FFDApp::layoutUI() {
    QGridLayout* const central_layout(new QGridLayout);
    central_layout->setColumnMinimumWidth(0, DEFAULT_IMG_SIZE_PX);
    central_layout->setColumnMinimumWidth(1, DEFAULT_IMG_SIZE_PX);
    central_layout->setColumnMinimumWidth(2, DEFAULT_IMG_SIZE_PX);
    central_layout->setRowMinimumHeight(0, DEFAULT_IMG_SIZE_PX);

    unsigned idx(0);
    central_layout->addWidget(_src, 0, idx++);
    central_layout->addWidget(_mix, 0, idx++);
    central_layout->addWidget(_dst, 0, idx++);
    central_layout->addWidget(setupParamsUI(centralWidget()), 1, 0, 1, idx);
    centralWidget()->setLayout(central_layout);
}


void FFDApp::setupToolbar() {
    QToolBar* const bar(addToolBar(tr("Toolbar")));
    bar->setStyleSheet("spacing: 0px");

    _new = new QAction(QIcon(":/new"), tr("New Project"), bar);
    _load_img = new QAction(QIcon(":/open_img"), tr("Open Images"), bar);
    _load_prj = new QAction(QIcon(":/open_prj"), tr("Open Project"), bar);
    _save_prj = new QAction(QIcon(":/save_prj"), tr("Save Project"), bar);
    _save_anim = new QAction(QIcon(":/save_img"), tr("Save Animation"), bar);
    _play_icon = QIcon(":/play");
    _pause_icon = QIcon(":/pause");
    _play = new QAction(_play_icon, tr("Play"), bar);
    _mesh = new QAction(QIcon(":/mesh"), tr("Mesh On/Off"), bar);

    bar->addAction(_new);
    bar->addAction(_load_img);
    bar->addAction(_load_prj);
    bar->addAction(_save_prj);
    bar->addAction(_save_anim);
    bar->addAction(_play);
    bar->addAction(_mesh);

    connect(_new, SIGNAL(triggered()), this, SLOT(clear()));
    connect(_load_img, SIGNAL(triggered()), this, SLOT(openImages()));
    connect(_load_prj, SIGNAL(triggered()), this, SLOT(openProject()));
    connect(_save_prj, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(_save_anim, SIGNAL(triggered()), this, SLOT(saveAnimation()));
    connect(_play, SIGNAL(triggered()), this, SLOT(toggleAnimation()));
    connect(_mesh, SIGNAL(triggered()), this, SLOT(toggleMesh()));
}


void FFDApp::setupMenus() {
    _file_menu = new QMenu(tr("&File"));
    _help_menu = new QMenu(tr("&Help"));

    _about = new QAction(tr("&About"), this);
    _about_qt = new QAction(tr("About &Qt"), this);

    connect(_about, SIGNAL(triggered()), this, SLOT(about()));
    connect(_about_qt, SIGNAL(triggered()), this, SLOT(aboutQt()));

    _file_menu->addAction(_new);
    _file_menu->addAction(_load_img);
    _file_menu->addAction(_load_prj);
    _file_menu->addAction(_save_prj);
    _file_menu->addAction(_save_anim);
    _file_menu->addSeparator();
    _file_menu->addAction(_play);
    _file_menu->addAction(_mesh);

    _help_menu->addAction(_about);
    _help_menu->addAction(_about_qt);

    menuBar()->addMenu(_file_menu);
    menuBar()->addMenu(_help_menu);
}


void FFDApp::setupTimer() {
    _frame = new QTimer(this);
    connect(_frame, SIGNAL(timeout()), this, SLOT(update()));
}


void FFDApp::fps(int n) {
    if(n < 0)
        return;

    assert(unsigned(n) <= MAX_FPS);

    _mix->fps(n);

    if(n == 0)
        _frame->stop();
    else
        _frame->start(1000.0f / n);

    if(_fps_sb->value() != int(_mix->fps())) {
        const SignalBlocker block(_fps_sb);
        _fps_sb->setValue(_mix->fps());
    }
}


void FFDApp::len(int n) {
    if(n < int(MIN_LEN) or n > int(MAX_LEN))
        return;

    _mix->duration(n);

    if(_len_sb->value() != int(_mix->duration())) {
        const SignalBlocker block(_len_sb);
        _len_sb->setValue(_mix->duration());
    }
}


void write(const Tag tag,
           const FFDWidget* const w,
           QXmlStreamWriter& xml)
{
    assert(w != 0);

    xml.writeStartElement(TAG[tag]);
        xml.writeAttribute(TAG[URI_TAG], w->selectionURI());
        std::stringstream out;
        w->widget()->saveMesh(out);
        xml.writeCharacters(QString(out.str().c_str()));
    xml.writeEndElement();
}


bool FFDApp::saveProject(const QString& uri) const {
    assert(uri.size() != 0);
    QFile file(uri);

    if(!file.open(QIODevice::WriteOnly)) {
        statusBar()->showMessage("Failed to open '" + uri + "\'");
        return false;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
        xml.writeStartElement(TAG[PROJECT_TAG]);
            xml.writeAttribute(TAG[FPS_TAG], QString::number(_mix->fps()));
            xml.writeAttribute(TAG[LEN_TAG], QString::number(_mix->duration()));
            write(SRC_TAG, _src, xml);
            write(DST_TAG, _dst, xml);
        xml.writeEndElement();
    xml.writeEndDocument();

    return true;
}


bool FFDApp::load(const QString& uri,
                  const QString& mesh,
                  FFDWidget* const ffdw)
{
    std::stringstream in(mesh.toStdString());

    const int src_sel(_src->selection());
    const int dst_sel(_dst->selection());

    const bool loaded_mesh(ffdw->widget()->loadMesh(in));
    const bool no_image(uri.isEmpty());
    const bool loaded_image(no_image or mgr()->loadImage(uri));

    if(not no_image and loaded_image) {
        if(src_sel != -1 and ffdw != _src)
            _src->select(src_sel);

        if(dst_sel != -1 and ffdw != _dst)
            _dst->select(dst_sel);
    }

    clearModifications();

    return loaded_image and loaded_mesh;
}


void loadProjectAttributes(QXmlStreamReader& xml, FFDApp* app) {
    assert(app != 0);

    if(xml.atEnd())
        return;

    const QString& fps_str(xml.attributes().value(TAG[FPS_TAG]).toString());
    const QString& len_str(xml.attributes().value(TAG[LEN_TAG]).toString());

    bool fps_ok(false), len_ok(false);
    const int fps(fps_str.toInt(&fps_ok));
    const int len(len_str.toInt(&len_ok));

    if(fps_ok and fps >= 0)
        app->fps(fps);

    if(len_ok and len > 0)
        app->len(len);
}


bool FFDApp::loadProject(const QString& uri) {
    assert(uri.size() != 0);
    QFile file(uri);
    if(!file.open(QIODevice::ReadOnly)) {
        statusBar()->showMessage("Failed to open '" + uri + "\'");
        return false;
    }

    QXmlStreamReader xml(&file);
    if(xml.hasError()) {
        statusBar()->showMessage("Error '" + uri + "\': " + xml.errorString());
        return false;
    }

    bool error(false);
    xml.readNextStartElement(); // PROJECT
    loadProjectAttributes(xml, this);
    while(!xml.atEnd())
        if(xml.readNext()) {
            const QString& tag(xml.name().toString());
            const QString& uri(xml.attributes().value(TAG[URI_TAG]).toString());
            if(tag == TAG[SRC_TAG])
                error = error or not load(uri, xml.readElementText(), _src);
            else if(tag == TAG[DST_TAG])
                error = error or not load(uri, xml.readElementText(), _dst);
        }

    if(error)
        clear();

    return not error;
}


void FFDApp::clear() {
    _src->clear();
    _dst->clear();
    _mix->clear();
    _file_mgr->clear();
    clearModifications();
}


void FFDApp::toggleAnimation() {
    if(_mix->animated()) {
        _mix->animated(false);
        _play->setIcon(_play_icon);
    } else {
        _mix->animated(true);
        _play->setIcon(_pause_icon);
    }
}


void FFDApp::update() {
    _src->update();
    _mix->update();
    _dst->update();
}


void FFDApp::openImages() {
    const QStringList& files(
        QFileDialog::getOpenFileNames(this, "Open:", "./",
                                      "Image Files (*.png *.jpg *.bmp)"));

    unsigned files_loaded(0);
    const QStringList::const_iterator& end(files.end());
    for(QStringList::const_iterator i(files.begin()); i != end; ++i)
        if(i->size() != 0 and mgr()->loadImage(*i))
            ++files_loaded;
        else
            QMessageBox::warning(this, tr("Load Image"),
                                "Unable to open '" + *i +"'",
                                QMessageBox::Ok);

    statusBar()->showMessage(
        QString::number(files_loaded) + QString(" Files Loaded"));
}


void FFDApp::openProject() {
    const QString& uri(
        QFileDialog::getOpenFileName(this, "Open:", "./",
                                     "Files (*." + PRJ_EXT + ")"));

    if(uri.size() != 0) {
        clear();

        if(loadProject(uri))
           statusBar()->showMessage("Loaded project from '" + uri + "'",
                                    MSG_DELAY);
        else
            statusBar()->showMessage("Failed to load project from '" +
                                     uri + "'");
    }
}


void FFDApp::saveProjectAs(const QString& uri) {
    _prj_uri = uri;

    const QString msg("project to '" + _prj_uri + "'");

    if(saveProject(_prj_uri))
        statusBar()->showMessage("Saved " + msg, MSG_DELAY);
    else
        statusBar()->showMessage("Failed to save " + msg);
}


void FFDApp::saveProjectAs() {
    const QString& uri(
                QFileDialog::getSaveFileName(this, "Save as:", "./",
                                             "Files (*." + PRJ_EXT + ")"));

    if(not uri.isEmpty())
        saveProjectAs(uri);
}


void FFDApp::saveProject() {
    if(_prj_uri.isEmpty())
        saveProjectAs();
    else
        saveProjectAs(_prj_uri);
}


void FFDApp::saveAnimation() {
    if(not (_src->hasSelection() and _dst->hasSelection()))
        return;

    const QString& uri(QFileDialog::getSaveFileName(this, "Save as:", "./",
                                                    "Animation ("
                                                    + selectedAnimMask() +
                                                    ")"));

    const QString& msg("animation to '" + uri + "'");

    if(uri.size() != 0) {
        if(saveAnimation(uri))
           statusBar()->showMessage("Saved " + msg, MSG_DELAY);
        else
            statusBar()->showMessage("Failed to save " + msg);
    }
}


void FFDApp::about() {
    QMessageBox::about(this, tr("About"), ABOUT);
}


void FFDApp::aboutQt() {
    QMessageBox::aboutQt(this, tr("About Qt"));
}


bool FFDApp::saveAnimation(const QString& uri) const {
    return _mix->save(uri);
}


QString FFDApp::selectedAnimMask() const {
    if(_gif->isChecked())
        return "*." + GIF_EXT;

    if(_mpg->isChecked())
        return "*." + MPG_EXT;

    const QString mask("*." + GIF_EXT + " *." + MPG_EXT);
    return mask;
}


void FFDApp::resolutionChanged(int n) {
    bool changed(false);

    if(_src->widget()->resolution() != unsigned(n)) {
        _src->widget()->resolution(n);
        changed = true;
    }

    if(_dst->widget()->resolution() != unsigned(n)) {
        _dst->widget()->resolution(n);
        changed = true;
    }

    if(changed)
        _mix->widget()->updateFaces();

    if(_mesh_sb->value() != n) {
        const SignalBlocker mesh_block(_mesh_sb);
        _mesh_sb->setValue(n);
    }
}


QWidget* FFDApp::setupParamsUI(QWidget* const parent) {
    _params_ui = new QWidget(parent);
    _mesh_lbl = new QLabel("Mesh Resolution", _params_ui);
    _mesh_sb = new QSpinBox(_params_ui);

    _anim_opts = new QGroupBox("Animation", _params_ui);
    _fps_lbl = new QLabel("FPS", _params_ui);
    _fps_sb = new QSpinBox(_params_ui);
    _len_lbl = new QLabel("Duration (msec)", _params_ui);
    _len_sb = new QSpinBox(_params_ui);
    _gif = new QRadioButton(GIF_EXT, _anim_opts);
    _mpg = new QRadioButton(MPG_EXT, _anim_opts);
    _bidirectional = new QCheckBox("Bidirectional", _params_ui);

    initParamsUI();
    layoutParamsUI();

    return _params_ui;
}


void FFDApp::initParamsUI() {
    _fps_sb->setRange(MIN_FPS, MAX_FPS);
    _len_sb->setRange(MIN_LEN, MAX_LEN);
    _mesh_sb->setRange(MIN_RES, MAX_RES);

    _fps_sb->setSingleStep(FPS_STEP);
    _len_sb->setSingleStep(LEN_STEP);
    _mesh_sb->setSingleStep(RES_STEP);

    _fps_sb->setValue(_mix->fps());
    _len_sb->setValue(_mix->duration());
    assert(_src->widget()->resolution() == _dst->widget()->resolution());
    _mesh_sb->setValue(_src->widget()->resolution());

    _bidirectional->setChecked(_mix->bidirectional());

    _gif->setChecked(DEFAULT_GIF_CHECKED);
    _mpg->setChecked(DEFAULT_MPG_CHECKED);

    connect(_fps_sb, SIGNAL(valueChanged(int)), this, SLOT(fps(int)));
    connect(_len_sb, SIGNAL(valueChanged(int)), _mix, SLOT(duration(int)));

    connect(_mesh_sb, SIGNAL(valueChanged(int)),
            this, SLOT(resolutionChanged(int)));
    connect(_bidirectional, SIGNAL(stateChanged(int)),
            this, SLOT(bidirectionality()));
}


void FFDApp::layoutParamsUI() {
    _fps_lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _len_lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _mesh_lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QGridLayout* const anim_grp_layout(new QGridLayout);
    unsigned anim_type_idx(0);
    anim_grp_layout->addWidget(_fps_lbl, 0, anim_type_idx++);
    anim_grp_layout->addWidget(_fps_sb, 0, anim_type_idx++);
    anim_grp_layout->addWidget(_len_lbl, 0, anim_type_idx++);
    anim_grp_layout->addWidget(_len_sb, 0, anim_type_idx++);
    anim_grp_layout->addWidget(_gif, 0, anim_type_idx++);
    anim_grp_layout->addWidget(_mpg, 0, anim_type_idx++);
    anim_grp_layout->addWidget(_bidirectional, 0, anim_type_idx++);
    _anim_opts->setLayout(anim_grp_layout);

    QGridLayout* const layout(new QGridLayout);
    unsigned idx(0);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, idx++);
    layout->addWidget(_anim_opts, 0, idx++);
    layout->addWidget(_mesh_lbl, 0, idx++);
    layout->addWidget(_mesh_sb, 0, idx++);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, idx++);
    _params_ui->setLayout(layout);
}


void FFDApp::bidirectionality() {
    _mix->bidirectional(_bidirectional->isChecked());
}


void FFDApp::toggleMesh() {
    _src->widget()->drawMesh(not _src->widget()->isDrawingMesh());
    _dst->widget()->drawMesh(not _dst->widget()->isDrawingMesh());
}


void FFDApp::saveProjectChanges() {
    QMessageBox question(this);
    question.setText("The project has unsaved changes.");
    question.setInformativeText("Do you want to save your changes?");
    question.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
    question.setDefaultButton(QMessageBox::Save);

    if(question.exec() == QMessageBox::Save)
        saveProject();

    clearModifications();
}


void FFDApp::clearModifications() {
    _src->widget()->clearModification();
    _dst->widget()->clearModification();
}


void FFDApp::closeEvent(QCloseEvent*) {
    if(_src->widget()->modified() or _dst->widget()->modified())
        saveProjectChanges();
}

