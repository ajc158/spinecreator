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

#include "viewELexptpanelhandler.h"
#include "connectionmodel.h"
#include "glconnectionwidget.h"
#include "rootdata.h"
#include "rootlayout.h"
#include "layoutaliaseditdialog.h"
#include "layouteditpreviewdialog.h"
#include "mainwindow.h"
#include "nineML_classes.h"
#include "experiment.h"
#include "projectobject.h"
//#include "stringify.h"


viewELExptPanelHandler::viewELExptPanelHandler(QObject *parent) :
    QObject(parent)
{
}

viewELExptPanelHandler::viewELExptPanelHandler(viewELstruct * viewEL, rootData * data, QObject *parent) :
    QObject(parent)
{

    this->data = data;
    this->viewEL = viewEL;
    this->exptSetup = new QVBoxLayout;
    this->exptInputs = new QVBoxLayout;
    this->exptOutputs = new QVBoxLayout;
    this->exptChanges = new QVBoxLayout;
    this->cursor = QPointF(0,0);

    // visual experiments test code - all looks good but not right now...
    if (0) {
        GLWidget * gl = new GLWidget;
        connect(gl, SIGNAL(reDraw(QPainter*,float,float,float,int,int,drawStyle)),data,SLOT(reDrawAll(QPainter*,float,float,float,int,int,drawStyle)));
        connect(gl, SIGNAL(mouseMove(float,float)), this, SLOT(mouseMove(float,float)));

        ((QHBoxLayout *) this->viewEL->expt->layout())->addWidget(gl);
    }

    this->exptSetup->setContentsMargins(4,4,4,4);
    this->exptInputs->setContentsMargins(4,4,4,4);
    this->exptOutputs->setContentsMargins(4,4,4,4);
    this->exptChanges->setContentsMargins(4,4,4,4);

    // add panel to expts
    ((QHBoxLayout *) this->viewEL->expt->layout())->addLayout(this->exptSetup);

    // add divider
    QFrame* lineA = new QFrame();
    lineA->setMaximumWidth(1);
    lineA->setFrameShape(QFrame::VLine);
    lineA->setFrameShadow(QFrame::Plain);
    ((QHBoxLayout *) this->viewEL->expt->layout())->addWidget(lineA);

    // add panel to expts
    ((QHBoxLayout *) this->viewEL->expt->layout())->addLayout(this->exptInputs);

    // add divider
    QFrame* lineB = new QFrame();
    lineB->setMaximumWidth(1);
    lineB->setFrameShape(QFrame::VLine);
    lineB->setFrameShadow(QFrame::Plain);
    ((QHBoxLayout *) this->viewEL->expt->layout())->addWidget(lineB);

    // add panel to expts
    ((QHBoxLayout *) this->viewEL->expt->layout())->addLayout(this->exptOutputs);

    // add divider
    QFrame* lineC = new QFrame();
    lineC->setMaximumWidth(1);
    lineC->setFrameShape(QFrame::VLine);
    lineC->setFrameShadow(QFrame::Plain);
    ((QHBoxLayout *) this->viewEL->expt->layout())->addWidget(lineC);

    // add panel to expts
    ((QHBoxLayout *) this->viewEL->expt->layout())->addLayout(this->exptChanges);

    this->viewEL->panel->setStyleSheet("background-color :white");

    redrawPanel();
    redrawExpt();

}

void viewELExptPanelHandler::recursiveDeleteLoop(QLayout * parentLayout) {

    QLayoutItem * item;
    while ((item = parentLayout->takeAt(0))) {
        if (item->widget()) {
            item->widget()->disconnect((QObject *)0);
            item->widget()->hide();
            forDeleting.push_back(item->widget());
        }
        if (item->layout())
            recursiveDeleteLoop(item->layout());
        delete item;
    }
    parentLayout->deleteLater();

}

void viewELExptPanelHandler::recursiveDelete(QLayout * parentLayout) {

    QLayoutItem * item;
    while ((item = parentLayout->takeAt(2))) {
        if (item->widget()) {
            item->widget()->disconnect((QObject *)0);
            item->widget()->hide();
            forDeleting.push_back(item->widget());
        }
        if (item->layout())
            recursiveDeleteLoop(item->layout());
        delete item;
    }

}


void viewELExptPanelHandler::recursiveDeleteExpt(QLayout * parentLayout) {

    QLayoutItem * item;
    while ((item = parentLayout->takeAt(0))) {
        if (item->widget()) {
            item->widget()->disconnect((QObject *)0);
            item->widget()->hide();
            forDeleting.push_back(item->widget());
        }
        if (item->layout())
            recursiveDeleteLoop(item->layout());
        delete item;
    }

}

void viewELExptPanelHandler::redraw() {
    redrawPanel();
    redrawExpt();
}

void viewELExptPanelHandler::redraw(int) {
    redrawPanel();
    redrawExpt();
}

void viewELExptPanelHandler::redraw(double) {
    redrawPanel();
    redrawExpt();
}

void viewELExptPanelHandler::redrawExpt() {

    // clear old stuff
    while(forDeleting.size() > 0) {
        forDeleting[0]->deleteLater();
        forDeleting.erase(forDeleting.begin());
    }

    recursiveDeleteExpt(exptSetup);
    recursiveDeleteExpt(exptInputs);
    recursiveDeleteExpt(exptOutputs);
    recursiveDeleteExpt(exptChanges);

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    if (currentExperiment->editing)
        return;

    QFont titleFont("Helvetica [Cronyx]", 12);
    QFont mainFont("Helvetica [Cronyx]", 10);

    QLabel * title;
    title = new QLabel(tr("Setup Simulator"));
    title->setFont(titleFont);
    exptSetup->addWidget(title);
    title = new QLabel(tr("Add model inputs"));
    title->setFont(titleFont);
    exptInputs->addWidget(title);
    title = new QLabel(tr("Add model outputs"));
    title->setFont(titleFont);
    exptOutputs->addWidget(title);
    title = new QLabel(tr("Add model changes"));
    title->setFont(titleFont);
    exptChanges->addWidget(title);

    // redraw SIMULATOR SETUP

    QFormLayout * formSim = new QFormLayout;
    exptSetup->addLayout(formSim);

    //Simulator
    QComboBox * engine = new QComboBox;

    // load existing simulators:
    QSettings settings;

    settings.beginGroup("simulators");
    QStringList sims = settings.childGroups();
    for (int i = 0; i < sims.size(); ++i) {
        engine->addItem(sims[i]);
    }
    settings.endGroup();

    for (int i = 0; i < engine->count(); ++i) {
        engine->setCurrentIndex(i);
        if (engine->currentText() == currentExperiment->setup.simType)
            break;
    }
    connect(engine, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedEngine(QString)));
    formSim->addRow("Engine:",engine);

    //duration
    QDoubleSpinBox * dur = new QDoubleSpinBox;
    dur->setSuffix(" s");
    dur->setMaximum(100000);
    dur->setMinimum(0.01);
    dur->setValue(currentExperiment->setup.duration);
    connect(dur, SIGNAL(editingFinished()), this, SLOT(changedDuration()));
    formSim->addRow("Duration:",dur);

    //timestep
    QDoubleSpinBox * dt = new QDoubleSpinBox;
    dt->setValue(currentExperiment->setup.dt);
    dt->setSuffix(" ms");
    connect(dt, SIGNAL(editingFinished()), this, SLOT(changedDt()));
    formSim->addRow("Time-step:",dt);

    //solver
    QComboBox * solver = new QComboBox;
    solver->addItem("Forward Euler",(int) ForwardEuler);
    //solver->addItem("Runge Kutta",(int) RungeKutta);
    //solver->addItem("DAMSON");
    for (int i = 0; i < solver->count(); ++i) {
        solver->setCurrentIndex(i);
        if (((solverType) solver->itemData(i).toInt()) == currentExperiment->setup.solver)
            break;
    }
    connect(solver, SIGNAL(currentIndexChanged(int)), this, SLOT(changedSolver(int)));
    formSim->addRow("Solver:",solver);

    //solver order
    if (currentExperiment->setup.solver == RungeKutta) {
        QSpinBox * solverOrd = new QSpinBox;
        solverOrd->setMinimum(2);
        solverOrd->setMaximum(4);
        solverOrd->setValue(currentExperiment->setup.solverOrder);
        connect(solverOrd, SIGNAL(valueChanged(int)), this, SLOT(changedSolverOrder(int)));
        formSim->addRow("Solver order:",solverOrd);
    }

    // redraw MODEL INPUTS

    QVBoxLayout * formIn = new QVBoxLayout;
    exptInputs->addLayout(formIn);

    // existing
    for (uint i = 0; i < currentExperiment->ins.size(); ++i) {
        exptInput * in = currentExperiment->ins[i];
        formIn->addLayout(in->drawInput(this->data, this));
    }

    //add new input
    QPushButton * addIn = new QPushButton("Add Input");
    addIn->setIcon(QIcon(":/icons/toolbar/addShad.png"));
    addIn->setFlat(true);
    addIn->setFocusPolicy(Qt::NoFocus);
    addIn->setToolTip("Add a new experimental input to the current model");
    addIn->setFont(addFont);
    // grey out if editing:
    for (uint i=0; i < currentExperiment->ins.size(); ++i)
        if (currentExperiment->ins[i]->edit)
            addIn->setDisabled(true);

    connect(addIn, SIGNAL(clicked()), this, SLOT(addInput()));

    formIn->addWidget(addIn);



    // redraw MODEL OUTPUTS

    QVBoxLayout * formOut = new QVBoxLayout;
    exptOutputs->addLayout(formOut);

    // existing
    for (uint i = 0; i < currentExperiment->outs.size(); ++i) {
        exptOutput * out = currentExperiment->outs[i];
        formOut->addLayout(out->drawOutput(this->data, this));
    }

    //add new output
    QPushButton * addOut = new QPushButton("Add Output");
    addOut->setIcon(QIcon(":/icons/toolbar/addShad.png"));
    addOut->setFlat(true);
    addOut->setFocusPolicy(Qt::NoFocus);
    addOut->setToolTip("Add a new experimental output to the current model");
    addOut->setFont(addFont);
    // grey out if editing:
    for (uint i=0; i < currentExperiment->outs.size(); ++i)
        if (currentExperiment->outs[i]->edit)
            addOut->setDisabled(true);

    connect(addOut, SIGNAL(clicked()), this, SLOT(addOutput()));

    formOut->addWidget(addOut);

    // redraw CHANGES

    QVBoxLayout * formLesion = new QVBoxLayout;
    exptChanges->addLayout(formLesion);

    formLesion->addWidget(new QLabel("Lesions"));

    // existing
    for (uint i = 0; i < currentExperiment->lesions.size(); ++i) {
        exptLesion * out = currentExperiment->lesions[i];
        formLesion->addLayout(out->drawLesion(this->data, this));
    }

    //add new lesion
    QPushButton * addLesion = new QPushButton("Add Lesion");
    addLesion->setIcon(QIcon(":/icons/toolbar/addShad.png"));
    addLesion->setFlat(true);
    addLesion->setFocusPolicy(Qt::NoFocus);
    addLesion->setToolTip("Add a new lesion to the current model");
    addLesion->setFont(addFont);
    // grey out if editing:
    for (uint i=0; i < currentExperiment->lesions.size(); ++i)
        if (currentExperiment->lesions[i]->edit)
            addLesion->setDisabled(true);

    connect(addLesion, SIGNAL(clicked()), this, SLOT(addLesion()));

    formLesion->addWidget(addLesion);



    QVBoxLayout * formPropChanges = new QVBoxLayout;
    exptChanges->addLayout(formPropChanges);

    formPropChanges->addWidget(new QLabel("Changed properties"));

    // existing
    for (uint i = 0; i < currentExperiment->changes.size(); ++i) {
        exptChangeProp * out = currentExperiment->changes[i];
        formPropChanges->addLayout(out->drawChangeProp(this->data, this));
    }

    //add new par change
    QPushButton * addChange = new QPushButton("Add Property Change");
    addChange->setIcon(QIcon(":/icons/toolbar/addShad.png"));
    addChange->setFlat(true);
    addChange->setFocusPolicy(Qt::NoFocus);
    addChange->setToolTip("Add a changed property to the current model");
    addChange->setFont(addFont);
    // grey out if editing:
    for (uint i=0; i < currentExperiment->changes.size(); ++i)
        if (currentExperiment->changes[i]->edit)
            addChange->setDisabled(true);

    connect(addChange, SIGNAL(clicked()), this, SLOT(addChangedProp()));

    formPropChanges->addWidget(addChange);

    exptSetup->addStretch();
    exptInputs->addStretch();
    exptOutputs->addStretch();
    exptChanges->addStretch();

}

void viewELExptPanelHandler::redrawPanel() {

    QVBoxLayout * panel = ((QVBoxLayout *) viewEL->panel->layout());

    // clear panel except toolbar
    recursiveDelete(panel);

    // add strech to avoid layout spacing out items
    panel->addStretch();

    // add button
    QPushButton * add = new QPushButton("Add experiment");
    add->setIcon(QIcon(":/icons/toolbar/addShad.png"));
    add->setFlat(true);
    add->setFocusPolicy(Qt::NoFocus);
    add->setToolTip("Add a new experiment to the current model");
    add->setFont(addFont);
    // grey out if editing:
    for (uint i=0; i < data->experiments.size(); ++i)
        if (data->experiments[i]->editing)
            add->setDisabled(true);

    connect(add, SIGNAL(clicked()), this, SLOT(addExperiment()));

    panel->insertWidget(2, add);

    // add experiments
    for (int i = data->experiments.size()-1; i >= 0; --i) {

        exptBox * box = data->experiments[i]->getBox(this);

        box->setProperty("index", i);
        connect(box, SIGNAL(clicked()), this, SLOT(changeSelection()));

        panel->insertWidget(2, box);

    }


}

void viewELExptPanelHandler::addExperiment() {

    QVBoxLayout * panel = ((QVBoxLayout *) viewEL->panel->layout());

    // add experiment to list
    data->experiments.push_back(new experiment);

    // select the new experiment
    exptBox * layout = data->experiments.back()->getBox(this);
    panel->insertWidget(3, layout);
    data->experiments.back()->select(&(data->experiments));

    data->experiments.back()->editing = true;

    // redraw to show the new experiment
    redrawPanel();
    redrawExpt();

}

void viewELExptPanelHandler::delExperiment() {

    uint index = sender()->property("index").toUInt();

    data->experiments.erase(data->experiments.begin() + index);

    // redraw to updata the selection
    redrawPanel();
    redrawExpt();

}

void viewELExptPanelHandler::moveExperiment() {

    uint index = sender()->property("index").toUInt();
    uint type = sender()->property("type").toUInt();

    experiment * tempExperiment;

    switch (type)
    {
    case 0: // up

        tempExperiment = data->experiments[index];
        data->experiments.erase(data->experiments.begin() + index);
        data->experiments.insert(data->experiments.begin() + index+1, tempExperiment);
        break;

    case 1: // down

        tempExperiment = data->experiments[index];
        data->experiments.erase(data->experiments.begin() + index);
        data->experiments.insert(data->experiments.begin() + index-1, tempExperiment);
        break;
    }

    // redraw to updata the selection
    redrawPanel();

}

void viewELExptPanelHandler::editExperiment() {

    uint index = sender()->property("index").toUInt();

    data->experiments[index]->editing = true;

    // redraw to update the editBox
    redrawPanel();
    redrawExpt();

}

void viewELExptPanelHandler::doneEditExperiment() {

    uint index = sender()->property("index").toUInt();

    // update the experiment
    QLineEdit * titleEdit = (QLineEdit *) sender()->property("title").value<void *>();
    data->experiments[index]->name = titleEdit->text();

    QTextEdit * descEdit = (QTextEdit *) sender()->property("desc").value<void *>();
    data->experiments[index]->description = descEdit->toPlainText();

    data->experiments[index]->editing = false;

    // redraw to update the editBox
    redrawPanel();
    redrawExpt();

}

void viewELExptPanelHandler::cancelEditExperiment() {

    uint index = sender()->property("index").toUInt();

    data->experiments[index]->editing = false;

    // redraw to update the editBox
    redrawPanel();
    redrawExpt();

}

void viewELExptPanelHandler::changeSelection() {

    uint index = sender()->property("index").toUInt();

    // check if this is the selected box - if it is then do nothing
    if (data->experiments[index]->selected)
        return;

    // otherwise select it
    data->experiments[index]->select(&(data->experiments));

    // redraw to update the selection
    redrawPanel();
    redrawExpt();

}


void viewELExptPanelHandler::changedEngine(QString sim) {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    currentExperiment->setup.simType = sim;

    // redraw to update the selection
    redrawExpt();

}

void viewELExptPanelHandler::changedDt() {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    currentExperiment->setup.dt = ((QDoubleSpinBox *) sender())->value();;

    // redraw to update the selection
    //redrawExpt();

}

void viewELExptPanelHandler::changedDuration() {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    currentExperiment->setup.duration = ((QDoubleSpinBox *) sender())->value();

    // redraw to update the selection
    //redrawExpt();

}

void viewELExptPanelHandler::changedSolver(int index) {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    solverType type = (solverType) ((QComboBox *) sender())->itemData(index).toInt();

    currentExperiment->setup.solver = type;

    // redraw to update the selection
    redrawExpt();

}

void viewELExptPanelHandler::changedSolverOrder(int val) {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    currentExperiment->setup.solverOrder = val;

    // redraw to update the selection
    redrawExpt();

}

void viewELExptPanelHandler::addInput() {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    currentExperiment->ins.push_back(new exptInput);

    // redraw to update the selection
    redrawExpt();

}

void viewELExptPanelHandler::setInputName() {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();

    in->name = ((QLineEdit *) sender())->text();

}

void viewELExptPanelHandler::setInputComponent() {

    // input text
    QString text = ((QLineEdit *) sender())->text();
    NineMLComponentData * src = (NineMLComponentData *)0;

    // find source:
    for (uint i = 0; i < data->system.size(); ++i) {
        if (data->system[i]->neuronType->getXMLName() == text) {
            src = data->system[i]->neuronType;
        }
        for (uint j = 0; j < data->system[i]->projections.size(); ++j) {
            for (uint k = 0; k < data->system[i]->projections[j]->synapses.size(); ++k) {
                if (data->system[i]->projections[j]->synapses[k]->weightUpdateType->getXMLName() == text) {
                    src = data->system[i]->projections[j]->synapses[k]->weightUpdateType;
                }
                if (data->system[i]->projections[j]->synapses[k]->postsynapseType->getXMLName() == text) {
                    src = data->system[i]->projections[j]->synapses[k]->postsynapseType;
                }
            }
        }
    }

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();

    if (src != (NineMLComponentData *)0) {
        // if no change...
        if (in->target == src)
            return;
        // else
        in->target = src;
        // old port is no longer valid
        in->portName = "";
        in->set = true;
        QPalette p = ((QLineEdit *) sender())->palette();
        p.setColor( QPalette::Normal, QPalette::Base, QColor(200, 255, 200) );
        ((QLineEdit *) sender())->setPalette(p);
        // UI changes required so hit up a redraw
        // disconnect sender so it doesn't flip out when it loses focus
        sender()->disconnect((QObject *)0);
        // redraw to update the selection
        redrawExpt();
    } else {
        in->set = false;
        QPalette p = ((QLineEdit *) sender())->palette();
        p.setColor( QPalette::Normal, QPalette::Base, QColor(255, 200, 200) );
        ((QLineEdit *) sender())->setPalette(p);
    }
}

void viewELExptPanelHandler::setInputPort(int index) {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();

    // assign the port stored with the currentIndex onto the input
    Port * port = (Port *) ((QComboBox *) sender())->itemData(index).value<void *>();
    in->portName = port->name;
    in->portIsAnalog = port->isAnalog();

    // disconnect sender so it doesn't flip out when it loses focus
    sender()->disconnect((QObject *)0);

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::setInputType(int index) {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();

    // assign the port stored with the currentIndex onto the input
    in->inType = (exptInType) ((QComboBox *) sender())->itemData(index).toInt();

    in->params.clear();

    // disconnect sender so it doesn't flip out when it loses focus
    sender()->disconnect((QObject *)0);

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::setInputRateDistributionType(int index) {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();

    if (index == 0)
        in->rateDistribution = Regular;
    else if (index == 1)
        in->rateDistribution = Poisson;

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::acceptInput() {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();
    in->edit = false;

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::editInput() {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();
    in->edit = true;

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::delInput() {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();

    for (uint i = 0; i < currentExperiment->ins.size(); ++i) {
        if (currentExperiment->ins[i] == in) {
            delete in;
            currentExperiment->ins.erase(currentExperiment->ins.begin()+i);
        }
    }

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::setInputTypeData(double value) {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();
    if (in->params.size() != 1)
        in->params.resize(1,0);
    in->params[0] = value;

    // redraw to update the selection
    //redrawExpt();
}

void viewELExptPanelHandler::setInputExternalData() {

    QString type = sender()->property("type").toString();
    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();

    if (type == "command")
        in->externalInput.commandline = ((QLineEdit *) sender())->text();
    else if (type == "port")
        in->externalInput.port = ((QSpinBox *) sender())->value();
    else if (type == "size")
        in->externalInput.size = ((QSpinBox *) sender())->value();

    // redraw to update the selection
    //redrawExpt();
}

void viewELExptPanelHandler::setInputParams(int row, int col) {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();
    QTableWidget * table = (QTableWidget *) sender();

    // find the edited value and alter the back end
    double value = table->currentItem()->data(Qt::DisplayRole).toDouble();

    if (in->inType == timevarying) {

        // construct index and set value
        int index = row*2+col;
        if (index > (int) in->params.size()-1)
            in->params.resize(qCeil(index/2)*2, 0);

        in->params[index] = value;

        // reorder:
        vector <float> tempVec;

        while (in->params.size() > 1) {
            int min = 100000000;
            int minIndex = 0;
            for (uint i = 0; i < in->params.size(); i+=2) {
                if (in->params[i] < min) {min = in->params[i]; minIndex = i;}
            }
            for (uint i = 0; i < 2; ++i) {
                tempVec.push_back(in->params[minIndex]);
                in->params.erase(in->params.begin()+minIndex);
            }
        }

        in->params.swap(tempVec);

    }
    else if (in->inType == arrayConstant) {

        in->params[row] = value;

    }
    else if (in->inType == arrayTimevarying) {

        vector < float > curr;

        bool copy = false;
        // move current index to new vector
        for (int i = 0; i < (int) in->params.size(); i+=2) {
            if (in->params[i] == -1 && copy) break;
            if (copy)
            {
                curr.push_back(in->params[i]);
                curr.push_back(in->params[i+1]);
                in->params.erase(in->params.begin()+i, in->params.begin()+i+2);
                i -= 2;
            }
            if (in->params[i] == -1 && in->params[i+1] == in->currentIndex) {
                copy = true;
                in->params.erase(in->params.begin()+i, in->params.begin()+i+2);
                i -= 2;
            }
        }

        // construct index and set value
        int index = row*2+col;
        if (index > (int) curr.size()-1)
            curr.resize(qCeil(index/2)*2, 0);

        curr[index] = value;

        // reorder:
        vector <float> tempVec;

        while (curr.size() > 1) {
            int min = 100000000;
            int minIndex = 0;
            for (uint i = 0; i < curr.size(); i+=2) {
                if (curr[i] < min) {min = curr[i]; minIndex = i;}
            }
            for (uint i = 0; i < 2; ++i) {
                tempVec.push_back(curr[minIndex]);
                curr.erase(curr.begin()+minIndex);
            }
        }

        // put back the index
        in->params.push_back(-1);
        in->params.push_back(in->currentIndex);
        for (uint i = 0; i < tempVec.size(); ++i)
            in->params.push_back(tempVec[i]);

    }


    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::fillInputParams() {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();
    QDoubleSpinBox * fillSpin = (QDoubleSpinBox *) sender()->property("ptrSpin").value<void *>();

    if (in->inType == arrayConstant) {

        for (uint i = 0; i < in->params.size(); ++i)
            in->params[i] = fillSpin->value();

    }

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::changeInputIndex(int index) {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();

    in->currentIndex = index;

    redrawExpt();

}

void viewELExptPanelHandler::setInputAddTVRow() {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();

    if (in->inType == timevarying) {
        in->params.resize(qCeil(in->params.size()/2)*2+2,0);
    }
    if (in->inType == arrayTimevarying) {
        bool found = false;
        int indexIndex = -1;

        // find where to operate on
        for (uint i = 0; i < in->params.size(); i+=2) {
            if (in->params[i] == -1 && found) {indexIndex = i; break;}
            if (in->params[i] == -1 && in->params[i+1] == in->currentIndex) {found = true;}
        }

        // insert the row
        if (indexIndex == -1) {
            in->params.push_back(0);
            in->params.push_back(0);
        } else {
            in->params.insert(in->params.begin()+indexIndex-1,2,0);
        }
    }

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::setInputDelTVRow() {

    exptInput * in = (exptInput *) sender()->property("ptr").value<void *>();

    if (in->inType == timevarying) {
        in->params.resize(qCeil(in->params.size()/2)*2-2,0);
    }
    if (in->inType == arrayTimevarying) {
        bool found = false;
        int indexIndex = -1;

        // find where to operate on
        for (uint i = 0; i < in->params.size(); i+=2) {
            if (in->params[i] == -1 && found) {indexIndex = i; break;}
            if (in->params[i] == -1 && in->params[i+1] == in->currentIndex) {found = true;}
        }

        // remove the row
        if (indexIndex == -1) {
            in->params.pop_back();
            in->params.pop_back();
        } else {
            in->params.erase(in->params.begin()+indexIndex,in->params.begin()+indexIndex+2);
        }
    }

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::addOutput() {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    currentExperiment->outs.push_back(new exptOutput);

    // redraw to update the selection
    redrawExpt();

}

void viewELExptPanelHandler::setOutputName() {

    exptOutput * out = (exptOutput *) sender()->property("ptr").value<void *>();

    out->name = ((QLineEdit *) sender())->text();

}

void viewELExptPanelHandler::setOutputComponent() {

    // input text
    QString text = ((QLineEdit *) sender())->text();
    NineMLComponentData * src = (NineMLComponentData *)0;

    // find source:
    for (uint i = 0; i < data->system.size(); ++i) {
        if (data->system[i]->neuronType->getXMLName() == text) {
            src = data->system[i]->neuronType;
        }
        for (uint j = 0; j < data->system[i]->projections.size(); ++j) {
            for (uint k = 0; k < data->system[i]->projections[j]->synapses.size(); ++k) {
                if (data->system[i]->projections[j]->synapses[k]->weightUpdateType->getXMLName() == text) {
                    src = data->system[i]->projections[j]->synapses[k]->weightUpdateType;
                }
                if (data->system[i]->projections[j]->synapses[k]->postsynapseType->getXMLName() == text) {
                    src = data->system[i]->projections[j]->synapses[k]->postsynapseType;
                }
            }
        }
    }

    exptOutput * out = (exptOutput *) sender()->property("ptr").value<void *>();

    if (src != (NineMLComponentData *)0) {
        // if no change
        if (out->source == src)
            return;
        // else...
        out->source = src;
        // old port is no longer valid
        out->portName = "";
        out->set = true;
        QPalette p = ((QLineEdit *) sender())->palette();
        p.setColor( QPalette::Normal, QPalette::Base, QColor(200, 255, 200) );
        ((QLineEdit *) sender())->setPalette(p);
        // UI change so hit up a redraw
        // disconnect sender so it doesn't flip out when it loses focus
        sender()->disconnect((QObject *)0);
        // redraw to update the selection
        redrawExpt();
    } else {
        qDebug() << "moo2";
        out->set = false;
        QPalette p = ((QLineEdit *) sender())->palette();
        p.setColor( QPalette::Normal, QPalette::Base, QColor(255, 200, 200) );
        ((QLineEdit *) sender())->setPalette(p);
    }
}

void viewELExptPanelHandler::setOutputPort(int index) {

    exptOutput * out = (exptOutput *) sender()->property("ptr").value<void *>();

    // assign the port stored with the currentIndex onto the input
    Port * port = (Port *) ((QComboBox *) sender())->itemData(index).value<void *>();
    out->portName = port->name;
    out->portIsAnalog = port->isAnalog();

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::setOutputType() {
    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::setOutputIndices() {

    exptOutput * out = (exptOutput *) sender()->property("ptr").value<void *>();
    // sanity check
    QString text = ((QLineEdit *) sender())->text();

    // blank is invalid
    if (text.isEmpty()) {
        ((QLineEdit *) sender())->setText(out->indices);
        return;
    }
    if (text != "all") {
        QString validCharacters("1234567890,");
        for (int i = 0; i < text.size(); ++i) {
            if (!validCharacters.contains(text[i])) {
                // invalid character used
                ((QLineEdit *) sender())->setText(out->indices);
                return;
            }
        }
        QStringList numbers = text.split(',');
        for (int i = 0; i < numbers.size(); ++i) {
            if (numbers[i].isEmpty()) {
                // invalid arrangement used
                ((QLineEdit *) sender())->setText(out->indices);
                return;
            }
        }
    }
    out->indices = text;

}

void viewELExptPanelHandler::acceptOutput() {

    exptOutput * out = (exptOutput *) sender()->property("ptr").value<void *>();
    out->edit = false;

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::editOutput() {

    exptOutput * out = (exptOutput *) sender()->property("ptr").value<void *>();
    out->edit = true;

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::delOutput() {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    exptOutput * out = (exptOutput *) sender()->property("ptr").value<void *>();

    for (uint i = 0; i < currentExperiment->outs.size(); ++i) {
        if (currentExperiment->outs[i] == out) {
            delete out;
            currentExperiment->outs.erase(currentExperiment->outs.begin()+i);
        }
    }

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::toggleExternalOutput(bool check) {

    exptOutput * out = (exptOutput *) sender()->property("ptr").value<void *>();
    out->isExternal = check;
}

void viewELExptPanelHandler::setOutputExternalData() {

    QString type = sender()->property("type").toString();
    exptOutput * out = (exptOutput *) sender()->property("ptr").value<void *>();

    if (type == "command")
        out->externalOutput.commandline = ((QLineEdit *) sender())->text();
    else if (type == "port")
        out->externalOutput.port = ((QSpinBox *) sender())->value();
    else if (type == "size")
        out->externalOutput.size = ((QSpinBox *) sender())->value();

    // redraw to update the selection
    //redrawExpt();
}

void viewELExptPanelHandler::addLesion() {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    currentExperiment->lesions.push_back(new exptLesion);

    // redraw to update the selection
    redrawExpt();

}

void viewELExptPanelHandler::setLesionProjection() {

    // input text
    QString text = ((QLineEdit *) sender())->text();
    projection * lesionedProj = (projection *)0;

    // find source:
    for (uint i = 0; i < data->system.size(); ++i) {
        for (uint j = 0; j < data->system[i]->projections.size(); ++j) {
            if (data->system[i]->projections[j]->getName() == text)
                lesionedProj = data->system[i]->projections[j];
        }
    }

    exptLesion * lesion = (exptLesion *) sender()->property("ptr").value<void *>();

    if (lesionedProj != (projection *)0) {
        lesion->proj = lesionedProj;
        lesion->set = true;
        QPalette p = ((QLineEdit *) sender())->palette();
        p.setColor( QPalette::Normal, QPalette::Base, QColor(200, 255, 200) );
        ((QLineEdit *) sender())->setPalette(p);
    } else {
        lesion->set = false;
        QPalette p = ((QLineEdit *) sender())->palette();
        p.setColor( QPalette::Normal, QPalette::Base, QColor(255, 200, 200) );
        ((QLineEdit *) sender())->setPalette(p);
    }

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::acceptLesion() {

    exptLesion * lesion = (exptLesion *) sender()->property("ptr").value<void *>();
    lesion->edit = false;

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::editLesion() {

    exptLesion * lesion = (exptLesion *) sender()->property("ptr").value<void *>();
    lesion->edit = true;

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::delLesion() {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    exptLesion * lesion = (exptLesion *) sender()->property("ptr").value<void *>();

    for (uint i = 0; i < currentExperiment->lesions.size(); ++i) {
        if (currentExperiment->lesions[i] == lesion) {
            delete lesion;
            currentExperiment->lesions.erase(currentExperiment->lesions.begin()+i);
        }
    }

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::setChangeParComponent() {

    // input text
    QString text = ((QLineEdit *) sender())->text();
    NineMLComponentData * src = (NineMLComponentData *)0;

    // find source:
    for (uint i = 0; i < data->system.size(); ++i) {
        if (data->system[i]->neuronType->getXMLName() == text) {
            src = data->system[i]->neuronType;
        }
        for (uint j = 0; j < data->system[i]->projections.size(); ++j) {
            for (uint k = 0; k < data->system[i]->projections[j]->synapses.size(); ++k) {
                if (data->system[i]->projections[j]->synapses[k]->weightUpdateType->getXMLName() == text) {
                    src = data->system[i]->projections[j]->synapses[k]->weightUpdateType;
                }
                if (data->system[i]->projections[j]->synapses[k]->postsynapseType->getXMLName() == text) {
                    src = data->system[i]->projections[j]->synapses[k]->postsynapseType;
                }
            }
        }
    }

    exptChangeProp * changeProp = (exptChangeProp *) sender()->property("ptr").value<void *>();

    if (src != (NineMLComponentData *)0) {
        changeProp->component = src;
        if (src->ParameterList.size() > 0) {
            changeProp->par = new ParameterData(src->ParameterList.front());
        } else if (src->StateVariableList.size() > 0) {
            changeProp->par = new StateVariableData(src->StateVariableList.front());
        } else {
            // no pars or statevars, return
            return;
        }
        changeProp->set = true;
        QPalette p = ((QLineEdit *) sender())->palette();
        p.setColor( QPalette::Normal, QPalette::Base, QColor(200, 255, 200) );
        ((QLineEdit *) sender())->setPalette(p);
    } else {
        changeProp->set = false;
        QPalette p = ((QLineEdit *) sender())->palette();
        p.setColor( QPalette::Normal, QPalette::Base, QColor(255, 200, 200) );
        ((QLineEdit *) sender())->setPalette(p);
    }

    // disconnect sender so it doesn't flip out when it loses focus
    sender()->disconnect((QObject *)0);

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::setChangeProp(QString name) {

    exptChangeProp * changeProp = (exptChangeProp *) sender()->property("ptr").value<void *>();

    for (uint i = 0; i < changeProp->component->ParameterList.size(); ++i) {
        if (name == changeProp->component->ParameterList[i]->name) {
            delete changeProp->par;
            changeProp->par = new ParameterData(changeProp->component->ParameterList[i]);
        }
    }
    for (uint i = 0; i < changeProp->component->StateVariableList.size(); ++i) {
        if (name == changeProp->component->StateVariableList[i]->name) {
            delete changeProp->par;
            changeProp->par = new StateVariableData(changeProp->component->StateVariableList[i]);
        }
    }
    redrawExpt();

}

void viewELExptPanelHandler::addChangedProp() {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    currentExperiment->changes.push_back(new exptChangeProp);

    // redraw to update the selection
    redrawExpt();

}

void viewELExptPanelHandler::acceptChangedProp() {

    exptChangeProp * prop = (exptChangeProp *) sender()->property("ptr").value<void *>();
    prop->edit = false;

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::editChangedProp() {

    exptChangeProp * prop = (exptChangeProp *) sender()->property("ptr").value<void *>();
    prop->edit = true;

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::delChangedProp() {

    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) return;

    exptChangeProp * prop = (exptChangeProp *) sender()->property("ptr").value<void *>();

    for (uint i = 0; i < currentExperiment->changes.size(); ++i) {
        if (currentExperiment->changes[i] == prop) {
            delete prop;
            currentExperiment->changes.erase(currentExperiment->changes.begin()+i);
        }
    }

    // redraw to update the selection
    redrawExpt();
}

void viewELExptPanelHandler::run() {

    QSettings settings;

    runButton = (QPushButton *) sender();
    runButton->setEnabled(false);

    simulatorStdOutText = "";
    simulatorStdErrText = "";

    // fetch current experiment sim engine
    experiment * currentExperiment = NULL;

    // find currentExperiment
    for (uint i = 0; i < data->experiments.size(); ++i) {
        if (data->experiments[i]->selected) {currentExperiment = data->experiments[i]; break;}
    }

    if (currentExperiment == NULL) {
        runButton->setEnabled(true);
        return;
    }

    QString simName = currentExperiment->setup.simType;

    // load path
    settings.beginGroup("simulators/" + simName);
    QString path = settings.value("path").toString();
    settings.endGroup();

    QStringList pathComponents = path.split(QDir::toNativeSeparators("/"));

    QString wk_dir_string = "";
    QString file_to_run = "./";
    file_to_run += pathComponents.back();


    for (int i = 0; i < pathComponents.count()-1; ++i) {
        wk_dir_string += "/" + pathComponents[i];
    }

    wk_dir_string = QDir::toNativeSeparators(wk_dir_string);

    QDir wk_dir(wk_dir_string);

    // set up the environment for the spawned processes
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    settings.beginGroup("simulators/" + simName + "/envVar");

    // load values
    QStringList keysTemp = settings.childKeys();

    for (int i = 0; i < keysTemp.size(); ++i) {
        env.insert(keysTemp[i], settings.value(keysTemp[i]).toString());
    }
    settings.endGroup();

    settings.setValue("simulator_export_path",QDir::toNativeSeparators(wk_dir_string + "/model/"));
    settings.setValue("export_binary",settings.value("simulators/" + simName + "/binary").toBool());

    // clear directory
    QDir model_dir(QDir::toNativeSeparators(wk_dir_string + "/model/"));
    QStringList files = model_dir.entryList(QDir::Files);
    for (int i = 0; i < files.size(); ++i) {
        model_dir.remove(files[i]);
    }

    // write out model
    if (!this->data->currProject->export_for_simulator(QDir::toNativeSeparators(wk_dir_string + "/model/"), data)) {
        settings.remove("simulator_export_path");
        settings.remove("export_binary");
        runButton->setEnabled(true);
        return;
    }

    settings.remove("simulator_export_path");
    settings.remove("export_binary");

    QProcess * simulator = new QProcess;
    simulator->setWorkingDirectory(wk_dir.absolutePath());
    simulator->setProcessEnvironment(env);

    simulator->setProperty("logpath", wk_dir_string + QDir::separator() + "temp");

    simulator->start(file_to_run);

    connect(simulator, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(simulatorFinished(int, QProcess::ExitStatus)));
    connect(simulator, SIGNAL(readyReadStandardOutput()), this, SLOT(simulatorStandardOutput()));
    connect(simulator, SIGNAL(readyReadStandardError()), this, SLOT(simulatorStandardError()));
}

void viewELExptPanelHandler::simulatorFinished(int, QProcess::ExitStatus status) {

    // update run button
    runButton->setEnabled(true);

    // collect logs
    QDir logs(sender()->property("logpath").toString());

    QStringList filter;
    filter << "*.xml";
    logs.setNameFilters(filter);

    // add logs to graphs
    data->main->viewGV.properties->loadDataFiles(logs.entryList(), &logs);

    // and insert logs into visualiser
    data->main->viewVZ.OpenGLWidget->addLogs(&data->main->viewGV.properties->logs);

    // get status
    if (status == QProcess::CrashExit) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Simulator Error");
        msgBox.setText(simulatorStdErrText);
        msgBox.exec();
        return;
    }
    if (status == QProcess::NormalExit) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Simulator Complete");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Simulator has finished. See below for more details, or any errors.");
        msgBox.setDetailedText(simulatorStdOutText);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

}

void viewELExptPanelHandler::simulatorStandardOutput() {

    QByteArray data = ((QProcess *) sender())->readAllStandardOutput();
    simulatorStdOutText = simulatorStdOutText + QString().fromUtf8(data);
   /* while (simulatorStdOutText.size()>1000) {
        simulatorStdOutText.remove(0,1);
    }*/

}

void viewELExptPanelHandler::simulatorStandardError() {

    QByteArray data = ((QProcess *) sender())->readAllStandardError();
    simulatorStdOutText = simulatorStdOutText + QString().fromUtf8(data);
    /*while (simulatorStdOutText.size()>1000) {
        simulatorStdOutText.remove(0,1);
    }*/

}
void viewELExptPanelHandler::mouseMove(float xGL, float yGL) {
    // move viewpoint
    GLWidget * source = (GLWidget *) sender();
    source->move(xGL+source->viewX-cursor.x(),yGL-source->viewY-cursor.y());
}
