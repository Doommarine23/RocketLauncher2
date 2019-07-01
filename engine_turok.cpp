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

extern QStringList RocketLauncher2::genturok1cmds(bool displayOnly=true)
{
    QStringList ret;

    //Doommarine23 - Just a message to prevent crashes as I try to figure out how to prevent them.
    //Basically, Rocket Launcher freaks out if there is no string data for the IWAD, even if I've removed all the checks for it.
   // ret << nullptr;
    bool filesadded = false;


    if (pwadloadlist->rowCount() > 0)
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

    //DoomMarine23 NEW FEATURES

    if (ui->check_nointro->isChecked())
        ret << "-skipintromovies";
                // ret << "-uselocalmods";

      /* Basically Unusable right now. Investigate these functions.

       * if (ui->combo_skill->currentText() != "Default")
     {
        qint16 skill = ui->combo_skill->currentIndex();
       ret << "DifficultyMode = " + QString::number(skill);
        ret << "g_difficulty" + QString::number(skill);
        ret << "snd_musicvolume " + QString::number(0.2);
     }
      */

    if (ui->input_argbox->text() != "" && ui->input_argbox->text() != NULL)
        ret.append(splitArgs(ui->input_argbox->text()));

    return ret;
}
