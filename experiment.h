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

#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "globalHeader.h"
#include "nineML_classes.h"

class exptBox : public QFrame
{
    Q_OBJECT
public:
    exptBox( QWidget *parent = 0);

signals:
    void clicked();

protected:
    void mouseReleaseEvent ( QMouseEvent *  ) ;

};

enum exptInType {

    constant,
    arrayConstant,
    timevarying,
    arrayTimevarying,
    external,
    spikeList

};

class exptInLookup {
public:
    static QString toString(exptInType val) {
        if (val == constant) return "Constant input";
        if (val == arrayConstant) return "Array of constant inputs";
        if (val == timevarying) return "Time varying input";
        if (val == arrayTimevarying) return "Array of time varying inputs";
        if (val == external) return "External input";
        if (val == spikeList) return "Explicit spike list input";
        return "error - unknown experiment type";
    }

};

enum procedure {

    SingleRun,
    RepeatRuns,
    Tuning

};

enum rateDistributionTypes {

    Regular,
    Poisson

};


enum solverType {

    ForwardEuler,
    RungeKutta

};

struct simulatorSetup {

    QString simType;
    float dt;
    float duration;
    solverType solver;
    int solverOrder;
    procedure exptProcedure;

};

struct externalObject {
    int port;
    QString commandline;
    int size;
};

class exptInput : QObject {
    Q_OBJECT
public:

    exptInput() {edit = true; set=false; eventport.name="spike"; target = NULL; portIsAnalog = false; inType = constant; params.push_back(0); rate = false;currentIndex = 0; name = "New Input"; rateDistribution = Poisson; externalInput.size=1; externalInput.port = 50091;}

    bool rate;
    int currentIndex;

    exptInType inType;
    vector < float > params;
    externalObject externalInput;
    NineMLComponentData * target;
    QString portName;
    bool portIsAnalog;
    bool edit;
    bool set;
    QString name;
    rateDistributionTypes rateDistribution;
    EventPort eventport;

    QVBoxLayout * drawInput(rootData *data, viewELExptPanelHandler * handler);
    void writeXML(QXmlStreamWriter *, projectObject * data);
    void readXML(QXmlStreamReader * , projectObject *);

};

class exptOutput : QObject {
    Q_OBJECT
public:

    exptOutput() {edit = true; set=false; isExternal = false; name = "New Output"; source = NULL; portIsAnalog = true; indices="all"; externalOutput.size=1; externalOutput.port = 50191;}

    //exptOutput outType;
    //vector < float > params;
    NineMLComponentData * source;
    QString portName;
    bool portIsAnalog;
    bool edit;
    bool set;
    bool isExternal;
    QString name;
    externalObject externalOutput;
    QString indices;

    QVBoxLayout * drawOutput(rootData *data, viewELExptPanelHandler * handler);
    void writeXML(QXmlStreamWriter *, projectObject * data);
    void readXML(QXmlStreamReader * , projectObject *);

};

class exptLesion : QObject {
    Q_OBJECT
public:

    exptLesion() {edit = true; set=false; proj = NULL;}

    //exptOutput outType;
    //vector < float > params;
    projection * proj;
    Port * port;
    bool edit;
    bool set;

    QVBoxLayout * drawLesion(rootData *data, viewELExptPanelHandler * handler);
    void writeXML(QXmlStreamWriter *, projectObject * data);
    void readXML(QXmlStreamReader * , projectObject *);

};

class exptChangeProp : QObject {
    Q_OBJECT
public:

    exptChangeProp() {edit = true; set=false; component = NULL; par = NULL;}

    ParameterData * par;
    NineMLComponentData * component;
    bool edit;
    bool set;

    QVBoxLayout * drawChangeProp(rootData *data, viewELExptPanelHandler * handler);
    void writeXML(QXmlStreamWriter *, projectObject * data);
    void readXML(QXmlStreamReader *);

};


class experiment : QObject
{
    Q_OBJECT

public:

    // VERSIONING
    versionNumber version;

    experiment();
    experiment(experiment *);
    ~experiment();

    simulatorSetup setup;
    vector < exptInput * > ins;
    vector < exptOutput * > outs;
    vector < exptChangeProp * > changes;
    vector < exptLesion * > lesions;

    QString name;
    QString description;

    exptBox * getBox(viewELExptPanelHandler *);
    void writeXML(QXmlStreamWriter *, projectObject *data);
    void readXML(QXmlStreamReader * , projectObject *);

    void purgeBadPointer(systemObject *ptr);
    void purgeBadPointer(NineMLComponentData *ptr);
    void purgeBadPointer(NineMLComponent *ptr, NineMLComponent *newPtr);
    void updateChanges(NineMLComponentData * ptr);

    bool selected;
    void select(vector < experiment * > *);
    void deselect();
    bool editing;

private:
    exptBox * currBoxPtr;

};


class NTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    NTableDelegate(QObject* parent);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const;
};

class TVGraph : public QWidget
{
    Q_OBJECT

public:
    TVGraph();
    TVGraph(vector <float>);
    ~TVGraph();
    vector <float> vals;

protected:
    void paintEvent(QPaintEvent *);

};

#endif // EXPERIMENT_H
