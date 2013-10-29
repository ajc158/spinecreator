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

#ifndef VIEWVISEXPTPANELHANDLER_H
#define VIEWVISEXPTPANELHANDLER_H

#include "globalHeader.h"

struct viewELstruct;


class viewELExptPanelHandler : public QObject
{
    Q_OBJECT
public:
    explicit viewELExptPanelHandler(QObject *parent = 0);
    explicit viewELExptPanelHandler(viewELstruct * viewEL, rootData * data, QObject *parent = 0);
    rootData * data;
    //void redraw();

private:
    viewELstruct * viewEL;
    void redrawExpt();
    void redrawPanel();

    QVBoxLayout * exptSetup;
    QVBoxLayout * exptInputs;
    QVBoxLayout * exptOutputs;
    QVBoxLayout * exptChanges;

    QPushButton * runButton;
    QString simulatorStdOutText;
    QString simulatorStdErrText;

    vector <QWidget * > forDeleting;

    void recursiveDeleteLoop(QLayout * parentLayout);
    void recursiveDelete(QLayout * parentLayout);
    void recursiveDeleteExpt(QLayout * parentLayout);

    QPointF cursor;

signals:
    
public slots:
    void addExperiment();
    void delExperiment();
    void moveExperiment();
    void editExperiment();
    void doneEditExperiment();
    void cancelEditExperiment();
    void changeSelection();
    void changedEngine(QString);
    void changedDt();
    void changedDuration();
    void changedSolver(int);
    void changedSolverOrder(int);
    void addInput();
    void setInputName();
    void setInputComponent();
    void setInputPort(int);
    void setInputType(int);
    void setInputRateDistributionType(int);
    void acceptInput();
    void editInput();
    void setInputTypeData(double);
    void setInputExternalData();
    void setInputParams(int, int);
    void fillInputParams();
    void changeInputIndex(int);
    void setInputAddTVRow();
    void setInputDelTVRow();
    void delInput();
    void addOutput();
    void setOutputName();
    void setOutputComponent();
    void setOutputPort(int);
    void setOutputType();
    void setOutputIndices();
    void acceptOutput();
    void editOutput();
    void delOutput();
    void toggleExternalOutput(bool);
    void setOutputExternalData();
    void addLesion();
    void setLesionProjection();
    void acceptLesion();
    void editLesion();
    void delLesion();
    void setChangeParComponent();
    void setChangeProp(QString name);
    void addChangedProp();
    void acceptChangedProp();
    void editChangedProp();
    void delChangedProp();
    void redraw();
    void redraw(int);
    void redraw(double);

    void run();
    void simulatorFinished(int, QProcess::ExitStatus);
    void simulatorStandardOutput();
    void simulatorStandardError();

    void mouseMove(float xGL, float yGL);

};

#endif // VIEWVISEXPTPANELHANDLER_H
