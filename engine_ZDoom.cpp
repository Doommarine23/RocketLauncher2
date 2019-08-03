/*  This file (engine_ZDoom) is part of Rocket Launcher 2.0 - A cross platform
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

extern QStringList RocketLauncher2::genZDoomcmds(bool displayOnly, bool loadFiles)
{
    QStringList ret;
    QString iwadpath = returnSelectedDndViewItemData(ui->listbox_IWADs);
    bool filesadded = false;
    ret << "-IWAD";

    if (iwadpath == "")
    {
        ret << "fail_IWADSELECT";
        return ret;
    }
    if (displayOnly == true){
       ret << '"'+iwadpath+'"';
    } else {
       ret << iwadpath;
    }

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
        if (enginelist->getEngineType() == Engine_ZDoom)
        {
            ret << "+MAP" << ui->input_map->text();
        }
        else
        {
            QStringList warp = ui->input_map->text().split(" ");
            ret << "-warp";
            ret.append(warp);
        }
    }

    if (ui->combo_skill->currentText() != "Default")
    {
        qint16 skill = ui->combo_skill->currentIndex();
        ret << "-skill" << QString::number(skill);
    }

    if (ui->check_nomonsters->isChecked())
        ret << "-nomonsters";

    if (ui->check_nomusic->isChecked())
        ret << "-nomusic";

    if (ui->check_record->isChecked())
    {
        ret << "-record";
        ret << ui->input_record->text();
    }

    if (ui->input_argbox->text() != "" && ui->input_argbox->text() != NULL)
        ret.append(splitArgs(ui->input_argbox->text()));

    return ret;
}
