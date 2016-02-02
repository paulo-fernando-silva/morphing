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

#include <QDir>
#include <QUrl>
#include <QFile>
#include <QLabel>
#include <QTimer>
#include <QAction>
#include <QPixmap>
#include <QMenuBar>
#include <QSpinBox>
#include <QToolBar>
#include <QCheckBox>
#include <QGroupBox>
#include <QFileInfo>
#include <QGLContext>
#include <QStatusBar>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QStringList>
#include <QSpacerItem>
#include <QMessageBox>
#include <QImageReader>
#include <QRadioButton>
#include <QDragEnterEvent>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>


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

const QString SAVE_ACT("save");
const QString LOAD_ACT("load");

const QString DEFAULT_DIR("./");
const QString DEFAULT_IMAGE_NAME("image");
const QString DEFAULT_IMAGE_DOT_EXT(".png");
const QString URL("https://sites.google.com/site/paulofernandosilva");
const QString LINK("<a href='" + URL + "'>my website</a>");
const QString TITLE("FFDApp Morphing Example");
const QString ABOUT_BEGIN(
		"<h4>2D Free Form Deformation morphing </h4>"
		"<p style=\"font:12px;\">"
		"Use the File menu to load images or a project file. "
		"Select one image as source and another as destination,"
		" each using the respective combo box. "
		"Move the points on the control meshes"
		" until you have reached the desired effect. "
		"Click animate for a continous animation. "
		"Set the animation time and frames per second"
		" and save the resulting animation to a file.<br>"
		"You can drag and drop project files and images directly to the app,"
		" and from the app to other applications.<br>"
		"For more random code go to " + LINK + ".<br>Enjoy :)");
const QString ABOUT_END("</p>");


/* *****************************************************************************
 * Utilities forward declaration.
 * ****************************************************************************/
QString path(const QString& uri);

void write(const Tag tag,
		   const FFDWidget* const w,
		   const QDir& prj_dir,
		   QXmlStreamWriter& xml);

void loadProjectAttributes(QXmlStreamReader& xml, FFDApp* app);

bool isImage(const QString& ext);

bool isProject(const QString& ext);

QString imageFilters();


class SelectionHelper {
public:
	inline SelectionHelper(FFDWidget* const widget):
		_widget(widget)
	{
		assert(_widget != 0);
		_selection = _widget->selection();
	}

	inline void reset(const bool state) {
		if(_selection != -1 and state)
			_widget->select(_selection);
	}

private:
	FFDWidget* _widget;
	int _selection;
};




/* *****************************************************************************
 * FFDApp implementation.
 * ****************************************************************************/
FFDApp::FFDApp():
	_mix(0),
	_src(0),
	_dst(0),
	_timer(0),
	_file_menu(0),
	_help_menu(0),
	_about(0),
	_about_qt(0),
	_new(0),
	_load_img(0),
	_load_prj(0),
	_save_prj(0),
	_save_prj_as(0),
	_save_img(0),
	_save_img_as(0),
	_save_anim(0),
	_save_anim_as(0),
	_play(0),
	_mesh(0),
	_opts_ui(0),
	_mesh_lbl(0),
	_mesh_sb(0),
	_fps_lbl(0),
	_fps_sb(0),
	_len_lbl(0),
	_len_sb(0),
	_bidirectional(0),
	_img_filters(imageFilters()),
	_curr_img_id(0)
{
	setupTimer();
	setupToolbar();
	setupMenus();
	setupDataUI();

	setAcceptDrops(true);

	clear();

	fps(DEFAULT_FPS); // start the frame update timer.
}


FFDApp::~FFDApp() {
	clear();
}


void FFDApp::clear() {
	_src->clear();
	_dst->clear();
	_mix->clear();
	_file_mgr->clear();
	clearModifications();
}


void FFDApp::setupDataUI() {
	setCentralWidget(new QWidget);
	initDataUI();
	connectDataUI();
	layoutDataUI();
}


void FFDApp::initDataUI() {
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
}


void FFDApp::connectDataUI() {
	glFFDWidget* const src_wgt(_src->widget());
	glFFDWidget* const dst_wgt(_dst->widget());

	connect(_mix, SIGNAL(animStateChanged()), this, SLOT(syncAnimState()));

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

	connect(src_wgt, SIGNAL(dragEnter(QDragEnterEvent*)),
			this, SLOT(dragEnterEvent(QDragEnterEvent*)));
	connect(dst_wgt, SIGNAL(dragEnter(QDragEnterEvent*)),
			this, SLOT(dragEnterEvent(QDragEnterEvent*)));

	connect(src_wgt, SIGNAL(dropped(QDropEvent*, QWidget*)),
			this, SLOT(dropEvent(QDropEvent*, QWidget*)));
	connect(dst_wgt, SIGNAL(dropped(QDropEvent*, QWidget*)),
			this, SLOT(dropEvent(QDropEvent*, QWidget*)));
}


void FFDApp::layoutDataUI() {
	QGridLayout* const layout(new QGridLayout);
	layout->setColumnMinimumWidth(0, DEFAULT_IMG_SIZE_PX);
	layout->setColumnMinimumWidth(1, DEFAULT_IMG_SIZE_PX);
	layout->setColumnMinimumWidth(2, DEFAULT_IMG_SIZE_PX);
	layout->setRowMinimumHeight(0, DEFAULT_IMG_SIZE_PX);

	unsigned idx(0);
	layout->addWidget(_src, 0, idx++);
	layout->addWidget(_mix, 0, idx++);
	layout->addWidget(_dst, 0, idx++);
	layout->addWidget(setupOptionsUI(centralWidget()), 1, 0, 1, idx);

	centralWidget()->setLayout(layout);
}


void FFDApp::setupToolbar() {
	QToolBar* const bar(addToolBar(tr("Toolbar")));
	bar->setStyleSheet("spacing: 0px");

	_new = new QAction(QIcon(":/new"), tr("New Project"), bar);
	_load_img = new QAction(QIcon(":/open_img"), tr("Open Images"), bar);
	_load_prj = new QAction(QIcon(":/open_prj"), tr("Open Project"), bar);
	_save_prj = new QAction(QIcon(":/save_prj"), tr("Save Project"), bar);
	_save_prj_as = new QAction(QIcon(":/save_prj_as"),
							   tr("Save Project As"), bar);
	_save_img = new QAction(QIcon(":/save_img"), tr("Save Image"), bar);
	_save_img_as = new QAction(QIcon(":/save_img_as"),
								tr("Save Image As"), bar);
	_save_anim = new QAction(QIcon(":/save_anim"), tr("Save Animation"), bar);
	_save_anim_as = new QAction(QIcon(":/save_anim_as"),
								tr("Save Animation As"), bar);
	_play_icon = QIcon(":/play");
	_pause_icon = QIcon(":/pause");
	_play = new QAction(_play_icon, tr("Play"), bar);
	_mesh = new QAction(QIcon(":/mesh"), tr("Mesh On/Off"), bar);

	bar->addAction(_new);
	bar->addAction(_load_prj);
	bar->addAction(_save_prj);
	bar->addAction(_save_prj_as);
	bar->addSeparator();
	bar->addAction(_load_img);
	bar->addAction(_save_img);
	bar->addAction(_save_img_as);
	bar->addAction(_save_anim);
	bar->addAction(_save_anim_as);
	bar->addSeparator();
	bar->addAction(_play);
	bar->addAction(_mesh);

	connect(_new, SIGNAL(triggered()), this, SLOT(clear()));
	connect(_load_img, SIGNAL(triggered()), this, SLOT(openImages()));
	connect(_load_prj, SIGNAL(triggered()), this, SLOT(openProject()));
	connect(_save_prj, SIGNAL(triggered()), this, SLOT(saveProject()));
	connect(_save_prj_as, SIGNAL(triggered()), this, SLOT(saveProjectAs()));
	connect(_save_img, SIGNAL(triggered()), this, SLOT(saveImage()));
	connect(_save_img_as, SIGNAL(triggered()), this, SLOT(saveImageAs()));
	connect(_save_anim, SIGNAL(triggered()), this, SLOT(saveAnimation()));
	connect(_save_anim_as, SIGNAL(triggered()), this, SLOT(saveAnimationAs()));
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
	_file_menu->addAction(_load_prj);
	_file_menu->addAction(_save_prj);
	_file_menu->addAction(_save_prj_as);
	_file_menu->addSeparator();
	_file_menu->addAction(_load_img);
	_file_menu->addAction(_save_img);
	_file_menu->addAction(_save_img_as);
	_file_menu->addAction(_save_anim);
	_file_menu->addAction(_save_anim_as);
	_file_menu->addSeparator();
	_file_menu->addAction(_play);
	_file_menu->addAction(_mesh);

	_help_menu->addAction(_about);
	_help_menu->addAction(_about_qt);

	menuBar()->addMenu(_file_menu);
	menuBar()->addMenu(_help_menu);
}


void FFDApp::setupTimer() {
	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(update()));
}


void FFDApp::fps(int n) {
	if(n < 0)
		return;

	assert(unsigned(n) <= MAX_FPS);

	_mix->fps(n);

	if(n == 0)
		stopTimer();
	else
		startTimer();

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



bool FFDApp::saveProject(const QString& uri) {
	assert(not uri.isEmpty());

	_prj_uri = uri;
	const QDir& dir(QFileInfo(uri).dir());
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
			write(SRC_TAG, _src, dir, xml);
			write(DST_TAG, _dst, dir, xml);
		xml.writeEndElement();
	xml.writeEndDocument();

	return true;
}


bool FFDApp::load(const QString& uri,
				  const QString& mesh,
				  FFDWidget* const ffdw)
{
	std::stringstream in(mesh.toStdString());

	const bool loaded_mesh(ffdw->widget()->loadMesh(in));
	const bool uri_empty(uri.isEmpty());
	const bool loaded_image(uri_empty or loadImage(uri, ffdw->widget()));

	clearModifications();

	return loaded_image and loaded_mesh;
}


bool FFDApp::loadProject(const QString& uri) {
	assert(not uri.isEmpty());

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

	clear();
	_prj_uri = uri;

	if(parseProject(xml, uri))
		return true;

	clear();

	return false;
}


bool FFDApp::parseProject(QXmlStreamReader& xml, const QString& prj_uri) {
	bool error(false);
	const QDir& prj_dir(QFileInfo(prj_uri).dir());

	xml.readNextStartElement(); // PROJECT
	loadProjectAttributes(xml, this);
	while(!xml.atEnd())
		if(xml.readNext()) {
			const QString& tag(xml.name().toString());
			QString uri(xml.attributes().value(TAG[URI_TAG]).toString());

			if(QFileInfo(uri).isRelative())
				uri = prj_dir.filePath(uri);

			if(tag == TAG[SRC_TAG])
				error = error or not load(uri, xml.readElementText(), _src);
			else if(tag == TAG[DST_TAG])
				error = error or not load(uri, xml.readElementText(), _dst);
		}

	return not error;
}


void FFDApp::toggleAnimation() {
	if(_mix->animated())
		pauseAnimation();
	else
		playAnimation();
}


void FFDApp::playAnimation() {
	_mix->animated(true);
	_play->setIcon(_pause_icon);
}


void FFDApp::pauseAnimation() {
	_mix->animated(false);
	_play->setIcon(_play_icon);
}


void FFDApp::update() {
	_src->update();
	_mix->update();
	_dst->update();
}


void FFDApp::openImages() {
	const QStringList& files(QFileDialog::getOpenFileNames(
								 this, "Open:", path(_load_img_uri),
								 "Image Files (" + _img_filters + ")"));

	if(files.isEmpty())
		return;

	unsigned files_loaded(0);
	const QStringList::const_iterator& end(files.end());
	for(QStringList::const_iterator i(files.begin()); i != end; ++i)
		if(not i->isEmpty() and loadImage(*i, 0))
			++files_loaded;
		else
			QMessageBox::warning(this, tr("Load Image"),
								"Unable to open '" + *i +"'",
								QMessageBox::Ok);

	statusBar()->showMessage(QString::number(files_loaded) + " Files Loaded");
}


bool FFDApp::loadImage(const QString& uri, QWidget* const sender) {
	SelectionHelper src_sel(_src);
	SelectionHelper dst_sel(_dst);

	if(mgr()->loadImage(uri)) {
		_load_img_uri = uri;

		src_sel.reset(sender != 0 and sender != _src->widget());
		dst_sel.reset(sender != 0 and sender != _dst->widget());

		return true;
	}

	return false;
}


void FFDApp::openProject() {
	const QString& uri(QFileDialog::getOpenFileName(
		this, "Open:", path(_prj_uri), "Files (*." + PRJ_EXT + ")"));

	if(not uri.isEmpty())
		onLoadResult(loadProject(uri), uri);
}


void FFDApp::saveProjectAs() {
	const QString& uri(QFileDialog::getSaveFileName(
		this, "Save as:", path(_prj_uri), "Files (*." + PRJ_EXT + ")"));

	if(not uri.isEmpty())
		onSaveResult(saveProject(uri), uri);
}


void FFDApp::saveProject() {
	if(_prj_uri.isEmpty())
		saveProjectAs();
	else
		onSaveResult(saveProject(_prj_uri), _prj_uri);
}


void FFDApp::saveImage() {
	if(not validAnimation())
		return;

	if(_save_img_uri.isEmpty())
		saveImageAs();
	else
		onSaveResult(saveImage(_save_img_uri), _save_img_uri);
}


void FFDApp::saveImageAs() {
	if(not validAnimation())
		return;

	const QString& uri(QFileDialog::getSaveFileName(
						   this, "Save as:", path(_save_img_uri),
						   "Image (" + _img_filters + ")"));

	if(not uri.isEmpty())
		onSaveResult(saveImage(uri), uri);
}


void FFDApp::saveAnimation() {
	if(not validAnimation())
		return;

	if(_anim_uri.isEmpty())
		saveAnimationAs();
	else
		onSaveResult(saveAnimation(_anim_uri), _anim_uri);
}


void FFDApp::saveAnimationAs() {
	if(not validAnimation())
		return;

	const QString& uri(QFileDialog::getSaveFileName(
						   this, "Save as:", path(_anim_uri),
						   "Animation (" + animMask() + ")"));

	if(not uri.isEmpty())
		onSaveResult(saveAnimation(uri), uri);
}


bool FFDApp::validAnimation() const {
	return _src->hasSelection() and _dst->hasSelection();
}


void FFDApp::about() {
	QMessageBox::about(this, tr("About"), ABOUT_BEGIN +
					   "<br><br>Supported image types: " + _img_filters +
					   "<br><br>Supported animation types: " + animMask() +
					   ABOUT_END);
}


void FFDApp::aboutQt() {
	QMessageBox::aboutQt(this, tr("About Qt"));
}


bool FFDApp::saveImage(const QString& uri) {
	assert(_mix->widget()->canPaint());

	if(_mix->widget()->frame().save(uri)) {
		_save_img_uri = uri;
		return true;
	}

	return false;
}


bool FFDApp::saveAnimation(const QString& uri) {
	stopTimer();
	const bool saved(_mix->save(uri));
	startTimer();

	if(saved)
		_anim_uri = uri;

	return saved;
}


QString FFDApp::animMask() const {
	const QString& mask("*." + GIF_EXT + " *." + MPG_EXT);
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


QWidget* FFDApp::setupOptionsUI(QWidget* const parent) {
	_opts_ui = new QGroupBox("Options", parent);
	_mesh_lbl = new QLabel("Mesh Resolution", _opts_ui);
	_mesh_sb = new QSpinBox(_opts_ui);
	_fps_lbl = new QLabel("FPS", _opts_ui);
	_fps_sb = new QSpinBox(_opts_ui);
	_len_lbl = new QLabel("Duration (msec)", _opts_ui);
	_len_sb = new QSpinBox(_opts_ui);
	_bidirectional = new QCheckBox("Bidirectional Animation", _opts_ui);

	initOptionsUI();
	layoutOptionsUI();

	return _opts_ui;
}


void FFDApp::initOptionsUI() {
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

	connect(_fps_sb, SIGNAL(valueChanged(int)), this, SLOT(fps(int)));
	connect(_len_sb, SIGNAL(valueChanged(int)), _mix, SLOT(duration(int)));

	connect(_mesh_sb, SIGNAL(valueChanged(int)),
			this, SLOT(resolutionChanged(int)));
	connect(_bidirectional, SIGNAL(stateChanged(int)),
			this, SLOT(bidirectionality()));
}


void FFDApp::layoutOptionsUI() {
	_fps_lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	_len_lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	_mesh_lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	QGridLayout* const layout(new QGridLayout);
	unsigned idx(0);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, idx++);
	layout->addWidget(_mesh_lbl, 0, idx++);
	layout->addWidget(_mesh_sb, 0, idx++);
	layout->addWidget(_fps_lbl, 0, idx++);
	layout->addWidget(_fps_sb, 0, idx++);
	layout->addWidget(_len_lbl, 0, idx++);
	layout->addWidget(_len_sb, 0, idx++);
	layout->addWidget(_bidirectional, 0, idx++);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, idx++);

	_opts_ui->setLayout(layout);
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


void FFDApp::dragEnterEvent(QDragEnterEvent* event) {
	// Not allowing raw data, unless I save it somehow in the project file.
	if(event->mimeData()->hasUrls() or event->mimeData()->hasImage())
		event->acceptProposedAction();
}


void FFDApp::dropEvent(QDropEvent* event) {
	dropEvent(event, 0);
}


void FFDApp::dropEvent(QDropEvent* event, QWidget* sender) {
	if(event->mimeData()->hasUrls())
		handleUrls(event, sender);

	if(event->mimeData()->hasImage())
		handleImage(event, sender);
}


void FFDApp::handleUrls(QDropEvent* event, QWidget* sender) {
	typedef QList<QUrl> Urls;
	typedef Urls::ConstIterator ConstIterator;

	const Urls& urls(event->mimeData()->urls());

	const ConstIterator end(urls.end());
	for(ConstIterator url(urls.begin()); url != end; ++url)
		process(*url, sender);
}


void FFDApp::process(const QUrl& url, QWidget* sender) {
	if(url.isLocalFile()) {
		const QString& uri(url.toLocalFile());
		const QFileInfo file(uri);
		const QString& ext(file.suffix());

		if(isProject(ext))
			onLoadResult(loadProject(uri), uri);
		else if(isImage(ext))
			onLoadResult(loadImage(uri, sender), uri);
		else
			statusBar()->showMessage(uri + " is not supported.");
	}
}


void FFDApp::handleImage(QDropEvent* event, QWidget* sender) {
	assert(event and event->mimeData()->hasImage());

	const QString& uri(genImgURI());

	const QImage& image(qvariant_cast<QImage>(event->mimeData()->imageData()));
	image.save(uri);

	onLoadResult(loadImage(uri, sender), uri);
}


void FFDApp::onResult(const bool success,
					  const QString& action,
					  const QString& uri)
{
	const QString& msg('\'' + uri + "' " + action);

	if(success)
		statusBar()->showMessage(msg + " ok.", MSG_DELAY);
	else
		statusBar()->showMessage(msg + " failed.");
}


void FFDApp::onLoadResult(const bool success, const QString& uri) {
	onResult(success, LOAD_ACT, uri);
}


void FFDApp::onSaveResult(const bool success, const QString& uri) {
	onResult(success, SAVE_ACT, uri);
}


QString FFDApp::genImgURI() {
	const QDir dir(path(_prj_uri));

	QFileInfo info;
	QString uri;

	do {
		uri = dir.filePath(DEFAULT_IMAGE_NAME +
						   QString::number(_curr_img_id++) +
						   DEFAULT_IMAGE_DOT_EXT);
		info.setFile(uri);
	} while(info.exists());

	return uri;
}


void FFDApp::syncAnimState() {
	if(_mix->animated())
		playAnimation();
	else
		pauseAnimation();
}


void FFDApp::startTimer() {
	const unsigned n(_mix->fps());
	assert(n != 0);
	_timer->start(1000.0f / n);
}


void FFDApp::stopTimer() {
	_timer->stop();
}


/* *****************************************************************************
 * Extra aux stuff
 * ****************************************************************************/
QString path(const QString& uri) {
	if(uri.isEmpty())
		return DEFAULT_DIR;

	return QFileInfo(uri).absolutePath();
}


void write(const Tag tag,
		   const FFDWidget* const w,
		   const QDir& prj_dir,
		   QXmlStreamWriter& xml)
{
	assert(w != 0);

	const QString& uri(prj_dir.relativeFilePath(w->selectionURI()));

	xml.writeStartElement(TAG[tag]);
		xml.writeAttribute(TAG[URI_TAG], uri);
		std::stringstream out;
		w->widget()->saveMesh(out);
		xml.writeCharacters(QString(out.str().c_str()));
	xml.writeEndElement();
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


bool isImage(const QString& ext) {
	typedef QList<QByteArray> FileFormats;
	typedef FileFormats::ConstIterator ConstIterator;
	const FileFormats& formats(QImageReader::supportedImageFormats());

	const ConstIterator end(formats.end());
	for(ConstIterator i(formats.begin()); i != end; ++i)
		if(ext == *i)
			return true;

	return false;
}


bool isProject(const QString& ext) {
	const QString lowercase_ext(ext.toLower());
	return lowercase_ext == PRJ_EXT;
}


QString imageFilters() {
	typedef QList<QByteArray> FileFormats;
	typedef FileFormats::ConstIterator ConstIterator;
	const FileFormats& formats(QImageReader::supportedImageFormats());

	QString filters;

	const ConstIterator end(formats.end());
	for(ConstIterator i(formats.begin()); i != end; ++i)
		filters += " *." + *i;

	return filters;
}
