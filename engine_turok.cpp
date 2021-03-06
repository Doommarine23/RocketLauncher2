/*  This file (engine_turok) is part of Rocket Launcher 2.0 - A cross platform
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

// TODO: Add bool for launching without pwads
extern QStringList RocketLauncher2::genturok1cmds(bool displayOnly=true, bool loadFiles = false)
{
    QStringList ret;
    bool filesadded = false;


    if (loadFiles && pwadloadlist->rowCount() > 0 )
    {
        for (int i = 0; i < pwadloadlist->rowCount(); i++ )
        {
            if (pwadloadlist->item(i)->checkState() == Qt::Checked)
            {
                if (!filesadded)
                {
                    ret << "-file";
                    filesadded = true;
                }
                if (displayOnly == true){
                    ret << '"'+pwadloadlist->item(i)->data(Qt::UserRole).toString()+'"';
                } else {
                    ret << pwadloadlist->item(i)->data(Qt::UserRole).toString();
                }
            }
        }
    }

    if (ui->input_map->text() != "" && ui->input_map->text() != NULL)
    {

        ret << "-runmap" << "levels/" + ui->input_map->text() + ".map";
    }

    if (ui->check_nointro->isChecked())
        ret << "-skipintromovies";

    if (ui->input_argbox->text() != "" && ui->input_argbox->text() != NULL)
        ret.append(splitArgs(ui->input_argbox->text()));


        return ret;
}
