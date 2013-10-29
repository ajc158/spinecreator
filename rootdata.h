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

#ifndef ROOTDATA_H
#define ROOTDATA_H

#include "globalHeader.h"

#include "nineml_layout_classes.h"
#include "connection.h"
#include "population.h"
#include "ninemlsortingdialog.h"
#include "rootlayout.h"
#include "glwidget.h"
#include "connectionmodel.h"
#include "glconnectionwidget.h"
#include "systemobject.h"
#include "valuelistdialog.h"

struct selStruct {
    int type;
    int index;
    int colInd;
};

struct cursorType {
    GLfloat x;
    GLfloat y;
};

struct loadedComponent {
    NineMLComponent * component;
    QString url;
};

class rootData : public QObject
{
    Q_OBJECT
public:

    // VERSIONING
    //versionNumber version;

    explicit rootData(QObject *parent = 0);

    vector < projectObject * > projects;
    projectObject * currProject;

    vector < population *> system;
    vector < NineMLComponent *> catalogUnsorted;
    vector < NineMLComponent *> catalogNrn;
    vector < NineMLComponent *> catalogWU;
    vector < NineMLComponent *> catalogPS;

    vector < NineMLLayout *> catalogLayout;
    vector < connection *> catalogConn;
    vector < experiment *> experiments;

    vector < loadedComponent > loadedComponents;

    //selStruct selected;

    // structure to hold selected items
    vector <systemObject *> selList;

    cursorType cursor;
    int largestIndex;
    void get_model_xml(QXmlStreamWriter &);
    void get_model_meta_xml(QDomDocument &meta);
    QColor getColor(QColor);
    QImage popImage;
    int getIndex();
    void setCurrConnectionModel(csv_connectionModel *);
    bool selectionMoved;
    void reDrawPanel();
    systemObject * getObjectFromName(QString name);
    void reDrawAll();
    void callRedrawGLview();
    void updateStatusBar(QString, int);
    void setTitle();
    void replaceComponent(NineMLComponent *, NineMLComponent *);
    NineMLRootObject * import_component_xml_single(QString fileName);
    bool isComponentInUse(NineMLComponent * oldComp);
    bool removeComponent(NineMLComponent * oldComp);
    bool isValidPointer(systemObject * ptr);
    bool isValidPointer(NineMLComponentData * ptr);
    bool isValidPointer(NineMLComponent * ptr);

    NineMLComponentData * clipboardCData;
    versionControl * version;

    MainWindow * main;

    QActionGroup * projectActions;

    void redrawViews();


signals:
    void undoRenameBox();
    void finishDrawingSynapse();
    void statusBarUpdate(QString, int);
    void updatePanel(rootData *);
    void updatePanelView2(QString);
    void redrawGLview();
    void setCaption(QString);
    void setWindowTitle();
    void itemsSelected(QString);

public slots:
    void saveImage(QString);
    void reDrawAll(QPainter *, float, float, float, int, int, drawStyle style);
    void selectByGL(float xGL, float yGL, float GLscale);
    void rightClickByGL(float xGL, float yGL, float GLscale);
    void mouseMoveGL(float, float);
    void import_csv(QString);
/*    bool import_project_xml(QString);
    void import_component_xml(QStringList fileNames);
    void import_layout_xml(QStringList fileNames);
    void export_project_xml(QString);
    bool export_model_for_simulator(QString);
    void export_component_xml(QString, NineMLComponent *component);
    void export_layout_xml(QString, NineMLLayout *component);*/
    void updatePortMap(QString);
    void updateType(int index);
    void updatePar();
    void updatePar(int);
    void updateLayoutPar();
    void setSize();
    void setLoc3();
    void addPopulation();
    void addSpikeSource();
    void renamePopulation();
    void startAddBezier(float, float);
    void addBezierOrProjection(float, float);
    void deleteCurrentSelection();
    void changeSynapse();
    void selectColour();
    void setCurrConnectionModelSig(csv_connectionModel *);
    void launchComponentSorter();
    void getNeuronLocationsSrc(vector < vector <loc> >*, vector <QColor> *, QString name);
    void selectByGLMouseUp(float xGL, float yGL, float GLscale);
    void setSelectionbyName(QString);
    void returnPointerToSelf(rootData * * data);
    void addgenericInput();
    void delgenericInput();
    void editConnections();
    void dragSelect(float xGL, float yGL);
    void endDragSelection();
    void setCaptionOut(QString);
    void setModelTitle(QString);
    void undoOrRedoPerformed(int);
    void abortProjection();
    void updatePanelView2Accessor();
    void copyParsToClipboard();
    void pasteParsFromClipboard();
    void selectProject(QAction *);

private:
    QString getUniquePopName(QString newName);
    bool selChange;
    QPointF dragListStart;
    QRectF dragSelection;
    QDomDocument doc;
    QDomDocument tempDoc;

};

#endif // ROOTDATA_H
