/***************************************************************************
**                                                                        **
**  This file is part of SpineCreator, an easy to use, GUI for            **
**  describing spiking neural network models.                             **
**  Copyright (C) 2013 Alex Cope, Paul Richmond                           **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Alex Cope                                            **
**  Website/Contact: http://bimpa.group.shef.ac.uk/                       **
****************************************************************************/

#ifndef NINEMLSORTINGDIALOG_H
#define NINEMLSORTINGDIALOG_H

#include "globalHeader.h"

#include "nineML_classes.h"
#include "rootdata.h"

namespace Ui {
class NineMLSortingDialog;
}

class NineMLSortingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NineMLSortingDialog(QWidget *parent = 0);
    explicit NineMLSortingDialog(rootData * data, QWidget *parent = 0);

    ~NineMLSortingDialog();
    
private:
    Ui::NineMLSortingDialog *ui;
    rootData * dPtr;

public slots:
    void accept();
    void reject();
    void del();
    void reset();
};

#endif // NINEMLSORTINGDIALOG_H
