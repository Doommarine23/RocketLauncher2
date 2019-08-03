/*  This file (rocketlauncher2.cpp) is part of Rocket Launcher 2.0 - A cross platform
 *  front end for all DOOM engine source ports.
 *
 *  Copyright (C) Hypnotoad
 *
 *  Rocket Launcher is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Rocket Launcher is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Rocket Launcher.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QSettings>
#include <QDir>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include "abstractmodels.h"
#include <QProcess>
#include <QException>
#include <QStandardItemModel>
#include <QDebug>
#include <QCommandLineParser>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>
#include <QShortcut>

#include "dndfilesystemlistview.h"
#include "rocketlauncher2.h"
#include "ui_rocketlauncher2.h"
#include "hyp_commonfunc.h"
#include "commandlinedialog.h"

//==========Initialization==========

RocketLauncher2::RocketLauncher2(QWidget *parent, int argc, char *argv[]) :
    QMainWindow(parent),
    ui(new Ui::RocketLauncher2),
    settings(QSettings::IniFormat,QSettings::UserScope,"RocketLauncher2","settings"),
    ConfigSettings(QSettings::IniFormat,QSettings::UserScope,"RocketLauncher2","SavedConfigs"),
    AutoLoad(QSettings::IniFormat,QSettings::UserScope,"RocketLauncher2","AutoLoad")
{
    //settings = QSettings(QSettings::IniFormat,QSettings::UserScope,"RocketLauncher2","settings");
    qDebug() << settings.fileName();
    //qDebug().quote();
    m_mainAppPath = QApplication::applicationDirPath();
    m_settingsfile = m_mainAppPath.filePath("settings.ini");
    m_wadextfilters << "*.wad" << "*.pk3" << ".pk7";
    ui->setupUi(this);
    setupAdditionalUi();

    initListViews();

    connect(ui->button_enginename, SIGNAL(pressed()), this, SLOT(on_input_selEngName_returnPressed()));
    connect(ui->listbox_configFavs, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_button_loadFavConfig_clicked()));
    connect(ui->listbox_pwadload, SIGNAL(fileSystemPathDropped(QString)), this, SLOT(addpwad(QString)));
    connect(enginelist, SIGNAL(updateCombo(const QString)), this, SLOT(setEngineSelection(const QString)));
    connect(enginelist, SIGNAL(updateComboIndex(int)), this, SLOT(setEngineSelectionIndex(int)));
    connect(ui->listbox_fav, SIGNAL(fileSystemPathDropped(QString)), this, SLOT(addToFavorites(const QString)));
    connect(ui->listbox_fav, SIGNAL(internalItemDropped(QDropEvent*)), this, SLOT(copyItemToFav(QDropEvent*)));
    connect(ui->listbox_pwadload, SIGNAL(internalItemDropped(QDropEvent*)), this, SLOT(copyItemToPwads(QDropEvent*)));
    connect(ui->listbox_IWADs, SIGNAL(internalItemDropped(QDropEvent*)), this, SLOT(copyItemToIwads(QDropEvent*)));
    connect(ui->listbox_IWADs, SIGNAL(fileSystemPathDropped(QString)), this, SLOT(addToIWADs(const QString)));
    initPixmaps();
    initConfigs();
    loadsettings();
    enginelist->LoadEngineData();
    autoLoadConfig();

    if (enginelist->DoomExePath != "" && enginelist->DoomExePath != NULL)
        ui->input_idExePath->setText(enginelist->DoomExePath);

    parseCmdLine(argc,argv);
    //ui->listbox_pwadload->setDragDropOverwriteMode(false);

    pwadFilter = tr("Any files (*)"
            ";;WAD/PK3/ZIP/PK7/PKZ/P7Z/KPF "
            "(*.wad *.pk3 *.zip *.pk7 *.pkz *.WAD *.PK3 *.ZIP *.PK7 *.PKZ *.P7Z *.KPF)"
            ";;WAD Files (*.wad *.WAD)"
            ";;PK3 Files (*.pk3 *.PK3)"
            ";;Patch Files (*.bex *.deh *.BEX *.DEH)"
            ";;PK7 Files (*.pk7 *.PK7)"
            ";;PKZ Files (*.pkz *.PKZ)"
            ";;P7Z Files (*.p7z *.P7Z)"
            ";;KPF Files (*.kpf)"
            ";;zip Files (*.zip *.ZIP)");

    //TODO add more map formats to filter as game list expands.
    mapFilter = tr("Any files (*)"
            ";;MAP "
            "(*.map)"
            ";;MAP Files (*.map *.MAP)");

}

void RocketLauncher2::closeEvent(QCloseEvent* event){
    //Rocket AutoRocket
    RocketLauncher2::saveToAutoLoad();
    settings.setValue("size", size());
    QMainWindow::closeEvent(event);
}


RocketLauncher2::~RocketLauncher2()
{
    delete ui;



}

void RocketLauncher2::initPixmaps()
{
    enginepics = new QList<QPixmap>;
    enginepics->append(QPixmap(":/engine/img/dlogo.png").scaled(105,105, Qt::KeepAspectRatio)); //0 Doom
    enginepics->append(QPixmap(":/engine/img/zandro.png").scaled(105,105,Qt::KeepAspectRatio)); //1 Zandronum
    enginepics->append((QPixmap(":/engine/img/ZDoomlogo.png").scaled(105,105,Qt::KeepAspectRatio))); //2 Zdoom
    enginepics->append((QPixmap(":/engine/img/GZDLogo.png").scaled(105,105,Qt::KeepAspectRatio))); //3 GZDoom
    enginepics->append((QPixmap(":/engine/img/Choclogo.png").scaled(105,105,Qt::KeepAspectRatio))); //4 Chocolate Doom
    enginepics->append((QPixmap(":/engine/img/prboom2.gif").scaled(105,105,Qt::KeepAspectRatio))); //5 PRBoom/PRBoom+
    enginepics->append((QPixmap(":/engine/img/Zdaemonlogo.png").scaled(105,105,Qt::KeepAspectRatio))); //6 ZDaemon
    enginepics->append((QPixmap(":/engine/img/oda.png").scaled(105,105,Qt::KeepAspectRatio))); //7 Odamex
    enginepics->append((QPixmap(":/engine/img/eelogo.gif").scaled(105,105,Qt::KeepAspectRatio))); //8 Eternity Engine
    enginepics->append((QPixmap(":/engine/img/dllogo.gif").scaled(105,105,Qt::KeepAspectRatio))); //9 Legacy
    enginepics->append((QPixmap(":/engine/img/edgelogo.png").scaled(105,105,Qt::KeepAspectRatio))); //10 Edge
    enginepics->append((QPixmap(":/engine/img/retrologo.png").scaled(105,105,Qt::KeepAspectRatio))); //11 RetroDoom
    enginepics->append((QPixmap(":/engine/img/vavoom2.png").scaled(105,105,Qt::KeepAspectRatio))); //12 Vavoom
    enginepics->append((QPixmap(":/engine/img/ddlogo.png").scaled(105,105,Qt::KeepAspectRatio))); //13 DoomsDay
    enginepics->append((QPixmap(":/engine/img/turoklogo.png").scaled(105,105,Qt::KeepAspectRatio))); //14 Turok
    ui->img_engine->setPixmap(enginepics->at(0));
}

void RocketLauncher2::setupAdditionalUi(){
    RLMenu = new QMenu(this);
    rlmCmdLne = new QAction("Show Command Line",this);
    rlmLoadRocket = new QAction("Load .rocket (Ctrl+O)",this);
    rlmSaveRocket = new QAction("Save .rocket (Ctrl+S)",this);
    rlmCmdLne->setObjectName("Show Command Line");
    rlmLoadRocket->setObjectName("Load .rocket");
    rlmSaveRocket->setObjectName("Save .rocket");
    RLMenu->addAction(rlmCmdLne);
    RLMenu->addAction(rlmLoadRocket);
    RLMenu->addAction(rlmSaveRocket);
    ui->button_rocketmenu->setMenu(RLMenu);
    connect(rlmCmdLne,SIGNAL(triggered()),this,SLOT(showCommandLine()));
    connect(rlmLoadRocket,SIGNAL(triggered()),this,SLOT(on_button_loadConfigExt_clicked()));
    connect(rlmSaveRocket,SIGNAL(triggered()),this,SLOT(on_button_saveConfigExt_clicked()));
    adjustSize();

    RLOpenShortcut = new QShortcut(QKeySequence(tr("Ctrl+O", "File|Open")), this);
    connect(RLOpenShortcut,SIGNAL(activated()),this,SLOT(on_button_loadConfigExt_clicked()));
    RLSaveShortcut = new QShortcut(QKeySequence(tr("Ctrl+S", "File|Save")), this);
    connect(RLSaveShortcut,SIGNAL(activated()),this,SLOT(on_button_saveConfigExt_clicked()));
}

void RocketLauncher2::initListViews()
{
    enginelist = new EngineListModel();
    ui->combo_Engines->setModel(enginelist);
    ui->listbox_engines->setModel(enginelist);

    pwadloadlist = new QStandardItemModel;
    ui->listbox_pwadload->setModel(pwadloadlist);
    ui->listbox_pwadload->setDragEnabled(true);
    ui->listbox_pwadload->setAcceptDrops(true);
    ui->listbox_pwadload->setDropIndicatorShown(true);
    ui->listbox_pwadload->setDragDropMode(QAbstractItemView::InternalMove);
    ui->listbox_pwadload->setDefaultDropAction(Qt::MoveAction);

    favlist = new QStandardItemModel;
    ui->listbox_fav->setModel(favlist);
    ui->listbox_fav->setDragEnabled(true);
    ui->listbox_fav->setAcceptDrops(true);
    ui->listbox_fav->setDropIndicatorShown(true);
    ui->listbox_fav->setDragDropMode(QAbstractItemView::InternalMove);
    ui->listbox_fav->setDefaultDropAction(Qt::MoveAction);

    iwadlist = new QStandardItemModel;
    ui->listbox_IWADs->setModel(iwadlist);
    ui->listbox_IWADs->setAcceptDrops(true);
    ui->listbox_IWADs->setDropIndicatorShown(true);
    ui->listbox_IWADs->setDragDropMode(QAbstractItemView::InternalMove);
    ui->listbox_IWADs->setDefaultDropAction(Qt::MoveAction);
}

//==========LOAD==========


void RocketLauncher2::loadsettings()
{
    //QSettings settings("RetroTools");
    int fsize = settings.beginReadArray("pwad_favs");
    qDebug() << fsize;
    //qDebug().quote();

    if (fsize > 0)
    {
        for (int i = 0; i < fsize; i++)
        {
            settings.setArrayIndex(i);
            updateFavs(settings.value("fav_path").toString(), false);
        }
    }

    settings.endArray();
    fsize = settings.beginReadArray("iwads");

    if (fsize > 0)
    {
        for (int i = 0; i < fsize; i++)
        {
            settings.setArrayIndex(i);
            updateIWADs(settings.value("iwad_path").toString(), false);
        }
    }

    settings.endArray();

    if (settings.contains("lastIwadIndex"))
    {
        QModelIndex index = iwadlist->index( settings.value("lastIwadIndex").toInt(), 0);
        ui->listbox_IWADs->setCurrentIndex(index);
    }

    if (settings.contains("size"))
        {
            QSize size = settings.value("size", QSize(400, 400)).toSize();
            this->resize(size);
        }

}

void RocketLauncher2::parseCmdLine(int argc, char *argv[])
{
    if (argc < 2)
        return;

    QCommandLineParser parser;
    QStringList args;

    for (int i = 0; i < argc; i++)
    {
        args << argv[i];
    }

    parser.process(args);
    const QStringList posargs = parser.positionalArguments();
    //QMessageBox::information(this,"Test",posargs.at(0));

    if (posargs.size() > 0)
    {
        QString rocketCheck = posargs.at(0);
        if (rocketCheck.right(6) == "rocket")
        {
            loadExtConfig(rocketCheck);
        }
        else
        {
            for (QString path : posargs)
            {
                RocketLauncher2::addpwad(path);
            }
        }
    }
}

void RocketLauncher2::showCommandLine(){
    QString enginefile;
    if (!enginelist->EngineSet)
    {
        QMessageBox::information(this,"Error" ,"Please select or add an engine (source port).");
        return;
    }

    enginefile = enginelist->getCurrentEngine()->path;
    //NOTE: Always show files in the RL command line, even if user was to click "don't load files"
    QStringList cmd = genCommandline(false,true);

if (enginelist->getCurrentEngine()->type != Engine_Turok1) // NOTE Required! Turok doesn't need these warnings (expand to other games later)
   {
        if (cmd[1] == "fail_IWADSELECT")
        {
            QMessageBox::information(this,"Error" ,"Please select your IWAD"); return;
        }
        else if (cmd[0] == "fail_DOOMEXE")
        {
            QMessageBox::information(this,"Error" , "Could not find original Doom Executable for DOSBox"); return;
        }
    }
        QString showargs;
        showargs = enginefile+" "+cmd.join(' ');
        CommandLineDialog *cmdDialog = new CommandLineDialog();
        cmdDialog->setWindowTitle("Command Line");
        cmdDialog->setTextBox(showargs);
        cmdDialog->show();
}

//==========LAUNCH ENGINE==========


QStringList RocketLauncher2::genCommandline(bool displayOnly=false, bool loadFiles=false)
{   // TODO: Turn into a Switch statement later.
    if (enginelist->getCurrentEngine()->type == Engine_DosBox)
    {
        return genDOSBoxcmd();
    }
    if (enginelist->getCurrentEngine()->type == Engine_Turok1)
    {
        if (loadFiles) return genturok1cmds(false,true);
        else return genturok1cmds(false,false);
    }
    if (enginelist->getCurrentEngine()->type == Engine_Default)
    {
        if (loadFiles) return genZDoomcmds(false,true);
        else return genZDoomcmds(false,false);
    }
    if (enginelist->getCurrentEngine()->type == Engine_ZDoom)
    {
        if (loadFiles) return genZDoomcmds(false,true);
        else return genZDoomcmds(false,false);

    }
}

void RocketLauncher2::on_pushButton_3_clicked() //RUN WITH ARCHIVES
{
    QString enginefile;

    if (!enginelist->EngineSet)
    {
        QMessageBox::information(this,"Error" ,"Please select or add an engine (source port).");
        return;
    }
    enginefile = enginelist->getCurrentEngine()->path;
    QStringList cmd = genCommandline(true,true);

    if (enginelist->getCurrentEngine()->type != Engine_Turok1) // NOTE Required! Turok doesn't need these warnings (expand to other games later)
    {
    if (cmd[1] == "fail_IWADSELECT")
    {
        QMessageBox::information(this,"Error" ,"Please select your IWAD");
        return;
    }
    else if (cmd[0] == "fail_DOOMEXE")
    {
        QMessageBox::information(this,"Error" , "Could not find original Doom Executable for DOSBox");
        return;
    }
}

    QFileInfo engineDir(enginefile);

#if defined(Q_OS_LINUX)
        QDir::setCurrent("LD_LIBRARY_PATH=~/.steam/bin32/ ~/.steam/bin32/steam-runtime/run.sh ./" + enginefile);   // DoomMarine23 fallback for Linux, investigate later.
#elif defined (Q_OS_WIN)
        QDir::setCurrent(engineDir.absolutePath());
#endif

    process = new QProcess();
    process->setProcessChannelMode(QProcess::ForwardedChannels);
    try
    {

        qDebug() << cmd;
        process->start(enginefile,cmd);

    }
    catch(QException &e)
    {
        QMessageBox::warning(this,"Error" ,"Engine failed to start.");
    }
}



void RocketLauncher2::on_pushButton_4_clicked() //RUN WITHOUT ARCHIVES
{
    QString enginefile;

    if (!enginelist->EngineSet)
    {
        QMessageBox::information(this,"Error" ,"Please select or add an engine (source port).");
        return;
    }

    enginefile = enginelist->getCurrentEngine()->path;
    QStringList cmd = genCommandline(true,false);

    if (enginelist->getCurrentEngine()->type != Engine_Turok1) // NOTE Required! Turok doesn't need these warnings (expand to other games later)
    {
    if (cmd[1] == "fail_IWADSELECT")
    {
        QMessageBox::information(this,"Error" ,"Please select your IWAD");
        return;
    }
    else if (cmd[0] == "fail_DOOMEXE")
    {
        QMessageBox::information(this,"Error" , "Could not find original Doom Executable for DOSBox");
        return;
    }
}

    QFileInfo engineDir(enginefile);

#if defined(Q_OS_LINUX)
        QDir::setCurrent("LD_LIBRARY_PATH=~/.steam/bin32/ ~/.steam/bin32/steam-runtime/run.sh ./" + enginefile);   // DoomMarine23 fallback for Linux, investigate later.
#elif defined (Q_OS_WIN)
        QDir::setCurrent(engineDir.absolutePath());
#endif

    process = new QProcess();
    process->setProcessChannelMode(QProcess::ForwardedChannels);
    try
    {

        qDebug() << cmd;
        process->start(enginefile,cmd);

    }
    catch(QException &e)
    {
        QMessageBox::warning(this,"Error" ,"Engine failed to start.");
    }
}

//==========ENGINE SELECTION HANDLING=========

void RocketLauncher2::on_pushButton_2_clicked() //Select Engine
{
    QString engine = QFileDialog::getOpenFileName(this,tr("Locate engine executable"));
    if (engine != NULL)
    {
        QFileInfo file = QFileInfo(engine);
        QString result = enginelist->addEngine(file);
            if (result == "Error")
                QMessageBox::information(this,"Error","Something went wrong, no engine added.");
    }
}

void RocketLauncher2::on_combo_Engines_currentIndexChanged(int index)
{
    enginelist->setCurrentEngine(index);
    SetEnginePic(enginelist->getCurrentEngine()->EngineImage);
    on_engine_check();
}

void RocketLauncher2::on_engine_check()
{

    // Hide/Enable features specific to your chosen engine.
    // TODO Possibly externalize these functions?
    switch(enginelist->getCurrentEngine()->type)
    {

    default:
        ui->pushButton_3->setText("Play Doom!");

        //Enable IWAD and Patch Wad Boxes
        ui->IWAD_label->setHidden(false);
        ui->listbox_IWADs->setHidden(false);

        //Enable Skill, IWAD, Patch Wad, Monsters, and Demo Recording Buttons
        ui->combo_skill->setHidden(false);
        ui->button_addiwad->setHidden(false);
        ui->button_deliwad->setHidden(false);
        ui->check_nomonsters->setHidden(false);
        ui->check_nomusic->setHidden(false);
        ui->check_record->setHidden(false);
        ui->input_record->setHidden(false);
        ui->combo_skill->setHidden(false);
        ui->label_skill->setHidden(false);
        break;

    case Engine_Turok1:
        ui->pushButton_3->setText("Play Turok!");

        //Disable IWAD and Patch Wad Boxes
        ui->IWAD_label->setHidden(true);
        ui->listbox_IWADs->setHidden(true);

        //Disable Skill, IWAD, Patch Wad, Monsters, and Demo Recording Buttons
        ui->combo_skill->currentText() = "Default";
        ui->combo_skill->setEnabled(false);

        ui->button_addiwad->setHidden(true);
        ui->button_deliwad->setHidden(true);

        ui->check_nomonsters->setHidden(true);
        ui->check_nomonsters->setChecked(false);

        ui->check_nomusic->setHidden(true);
        ui->check_nomusic->setChecked(false);

        ui->check_record->setHidden(true);
        ui->check_record->setChecked(false);

        ui->input_record->setHidden(true);

        ui->combo_skill->setHidden(true);
        ui->label_skill->setHidden(true);
        break;

    }


}

void RocketLauncher2::SetEnginePic(EnginePic pic)
{
    switch(pic)
    {
        default:
            ui->img_engine->setPixmap(enginepics->at(0)); break;
        case Pic_Zandronum:
            ui->img_engine->setPixmap(enginepics->at(1)); break;
        case Pic_Zdoom:
            ui->img_engine->setPixmap(enginepics->at(2)); break;
        case Pic_GZdoom:
            ui->img_engine->setPixmap(enginepics->at(3)); break;
        case Pic_Chocolate:
            ui->img_engine->setPixmap(enginepics->at(4)); break;
        case Pic_PrBoom:
            ui->img_engine->setPixmap(enginepics->at(5)); break;
        case Pic_ZDaemon:
            ui->img_engine->setPixmap(enginepics->at(6)); break;
        case Pic_Odamex:
            ui->img_engine->setPixmap(enginepics->at(7)); break;
        case Pic_Eternity:
            ui->img_engine->setPixmap(enginepics->at(8)); break;
        case Pic_Legacy:
            ui->img_engine->setPixmap(enginepics->at(9)); break;
        case Pic_Edge:
            ui->img_engine->setPixmap(enginepics->at(10)); break;
        case Pic_Retro:
            ui->img_engine->setPixmap(enginepics->at(11)); break;
        case Pic_Vavoom:
            ui->img_engine->setPixmap(enginepics->at(12)); break;
        case Pic_Doomsday:
            ui->img_engine->setPixmap(enginepics->at(13)); break;
        case Pic_Turok1:
            ui->img_engine->setPixmap(enginepics->at(14)); break;
    }
}



void RocketLauncher2::setEngineSelection(const QString text)
{
    ui->combo_Engines->setCurrentText(text);

}

void RocketLauncher2::setEngineSelectionIndex(int index)
{
    qDebug() << "updating index";
    //qDebug().quote();
    ui->combo_Engines->setCurrentIndex(index);

}

//==========WAD LISTVIEW HANDLING==========

bool RocketLauncher2::addpwad(QString filepath)
{
    return updateDndListView(filepath, pwadloadlist, true);
}

void RocketLauncher2::addToFavorites(const QString filepath)
{
    updateFavs(filepath, true);
}

void RocketLauncher2::updateFavs(QString filepath, bool save)
{
    bool noMatch = updateDndListView(filepath, favlist, false);

    if (save)
    {
        if (noMatch)
            saveListviewPath("pwad_favs", "fav_path", filepath, settings);
    }
}

void RocketLauncher2::on_button_add_clicked()
{
    /*
    const QString filter = tr("WAD/PK3/ZIP/PK7/PKZ/P7Z/KPF "
            "(*.wad *.pk3 *.zip *.pk7 *.pkz *.kpf"
            ";;WAD Files (*.wad)"
            ";;PK3 Files (*.pk3)"
            ";;Patch Files (*.bex *.deh)"
            ";;PK7 Files (*.pk7)"
            ";;PKZ Files (*.pkz)"
            ";;P7Z Files (*.p7z)"
            ";;zip Files (*.zip)"
            ";;kpf Files (*.kpf)"
            ";;Any files (*)");
            */
    addpwad(QFileDialog::getOpenFileName(this,tr("Add a pwad"),"",pwadFilter));

}

void RocketLauncher2::on_button_pwadsclear_clicked()
{
    pwadloadlist->clear();
}

void RocketLauncher2::on_button_remove_clicked()
{
    this->setUpdatesEnabled(false);
    removeSelectedFromDnDListView(ui->listbox_pwadload, pwadloadlist);
    this->setUpdatesEnabled(true);
}

void RocketLauncher2::on_button_favadd_clicked()
{
    updateFavs(QFileDialog::getOpenFileName(this,tr("Add a file to your favorites!"),"",pwadFilter), true);
}

void RocketLauncher2::on_button_favremove_clicked()
{
    this->setUpdatesEnabled(false);
    removeSelectedFromDnDListViewSave(ui->listbox_fav, favlist, "pwad_favs", "fav_path", settings);
    this->setUpdatesEnabled(true);

}

void RocketLauncher2::copyItemToPwads(QDropEvent* pEvent)
{
    this->setUpdatesEnabled(false);
    copyItemToDndListView(qobject_cast<DndFileSystemListView *>(pEvent->source()), pwadloadlist, true);
    this->setUpdatesEnabled(true);
}

void RocketLauncher2::copyItemToFav(QDropEvent* pEvent)
{
    this->setUpdatesEnabled(false);
    copyItemToDndListViewSave(qobject_cast<DndFileSystemListView *>(pEvent->source()), favlist,
                              false, "pwad_favs", "fav_path", settings);
    this->setUpdatesEnabled(true);
}

void RocketLauncher2::on_button_addiwad_clicked()
{
    QString title = tr("Locate an IWAD to add to the list.");
    QString filter = tr("WAD/pk3/zip (*.WAD *.pk3 *.zip)"
            ";;WAD Files (*.WAD)"
            ";;pk3 Files (*.pk3)"
            ";;zip Files (*.zip)"
            ";;All files (*)");
    updateIWADs(QFileDialog::getOpenFileName(this,title,"",filter), true);
}

void RocketLauncher2::updateIWADs(QString filepath, bool save)
{
    bool noMatch = updateDndListView(filepath, iwadlist, false);

    if (save)
    {
        if (noMatch)
            saveListviewPath("iwads", "iwad_path", filepath, settings);
    }
}

void RocketLauncher2::on_button_deliwad_clicked()
{
    removeSelectedFromDnDListViewSave(ui->listbox_IWADs, iwadlist, "iwads", "iwad_path", settings);
}

void RocketLauncher2::copyItemToIwads(QDropEvent* pEvent)
{
    this->setUpdatesEnabled(false);
    copyItemToDndListViewSave(qobject_cast<DndFileSystemListView *>(pEvent->source()), iwadlist,
                              false, "iwads", "iwad_path", settings);
    this->setUpdatesEnabled(true);
}

void RocketLauncher2::addToIWADs(const QString filepath)
{
    updateIWADs(filepath, true);
}

void RocketLauncher2::on_listbox_IWADs_clicked(const QModelIndex &index)
{
    settings.setValue("lastIwadIndex",index.row());
}


//==========MISC==========

// TODO Add a proper help function that documents the program.

bool RocketLauncher2::savesettings(QString key, QString value)
{
    if (key == "" || key == NULL || value == "" || value == NULL)
        return false;

    //QSettings settings("RetroTools");
    settings.setValue(key, value);
    return true;
}
// TODO Change this to open from a help file, making life easier for writing.
void RocketLauncher2::on_button_helpmap_clicked()
{
        switch(enginelist->getCurrentEngine()->type)
        {

        default:
            QMessageBox::information(this, "Map/Warp", "If the engine is a modern ZDoom based engine, use the maplump name, e.g. 'MAP01', otherwise if it's a more oldschool engine, use the map number, e.g. '01'");
            break;
        case Engine_Turok1:
            QMessageBox::information(this, "Map/Warp", "NOTE: Disables achivements, and is NOT case sensitive. Only type in the level's name: level06 If level is inside a folder, include it: doommarine23/lavacave");
            break;

        }

}


void RocketLauncher2::on_button_mapfilename_clicked()
{
    //TODO Add NULL/ERROR sanity check? Seems un-needed right now.
    QFileInfo file = QFileDialog::getOpenFileName(this,tr("Select Map File"),"",mapFilter);
    QString mapfilename = file.baseName();
        ui->input_map->clear();
        ui->input_map->insert(mapfilename);
}
