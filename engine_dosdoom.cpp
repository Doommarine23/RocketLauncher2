/*  This file (engine_dosdoom) is part of Rocket Launcher 2.0 - A cross platform
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

extern QStringList RocketLauncher2::genDOSBoxcmd()
{
    QStringList ret;
    QStringList dosTemp;
    QFileInfo doomExeFile(enginelist->DoomExePath);
    QDir root(doomExeFile.absolutePath());

    if (!doomExeFile.exists())
    {
        ret << "fail_DOOMEXE";
        return ret;
    }

    QString mountfold = doomExeFile.absolutePath();
    ret << "-c";
    ret << "MOUNT C " + root.rootPath();
    ret << "-c";
    ret << "C:";
    ret << "-c";
    ret << "cd " + mountfold;
    ret << "-c";
    ret << "aspect = true";
    bool filesadded = false;

    if (pwadloadlist->rowCount() > 0)
    {
        for (int i = 0; i < pwadloadlist->rowCount(); i++ )
        {
            if (pwadloadlist->item(i)->checkState() == Qt::Checked)
            {
                if (!filesadded)
                {
                    dosTemp << "-file";
                    filesadded = true;
                }
                dosTemp << pwadloadlist->item(i)->data(Qt::UserRole).toString();
            }
        }
    }

    if (ui->input_map->text() != "" && ui->input_map->text() != NULL)
    {
        dosTemp << "-warp" << ui->input_map->text();
    }

    if (ui->combo_skill->currentText() != "Default")
    {
        qint16 skill = ui->combo_skill->currentIndex();
        dosTemp << "-skill" << QString::number(skill);
    }

    if (ui->check_nomonsters->isChecked())
        dosTemp << "-nomonsters";

    if (ui->check_nomusic->isChecked())
        dosTemp << "-nomusic";

    if (ui->check_record->isChecked())
        dosTemp << "-record " + ui->input_record->text();

    if (ui->input_argbox->text() != "" && ui->input_argbox->text() != NULL)
        dosTemp.append(ui->input_argbox->text().split(" "));

    ret << "-c";
    ret << doomExeFile.fileName() + " " + dosTemp.join(" ");
    ret << "-c";
    ret << "exit";

    return ret;
}
