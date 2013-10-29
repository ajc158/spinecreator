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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "globalHeader.h"

#include "nineML_classes.h"
#include "connectionmodel.h"
#include "glconnectionwidget.h"
#include "rootdata.h"
#include "rootlayout.h"
#include "layoutaliaseditdialog.h"
#include "layouteditpreviewdialog.h"
#include "viewELexptpanelhandler.h"
#include "viewVZlayoutedithandler.h"
#include "viewGVpropertieslayout.h"
#include "brahms_dialog.h"
#include "nineml_alview.h"
#include "versioncontrol.h"

namespace Ui {
    class MainWindow;
}

struct viewGVstruct {
    QMainWindow * subWin;
    QMdiArea * mdiarea;
    QDockWidget * dock;
    QToolBar * toolbar;
    MainWindow * mainwindow;
    viewGVpropertieslayout * properties;
};

struct viewELstruct {
    QFrame * view;
    QWidget * selectionPanel;
    QWidget * panel;
    QWidget * expt;
};

struct viewNLstruct {
    rootLayout * layout;
};

struct viewVZstruct {
    QSplitter * view;
    csv_connectionModel * connMod;
    QTableView * connTable;
    QWidget * panel;
    glConnectionWidget * OpenGLWidget;
    QTreeView * treeView;
    QLabel * mathOut;
    NineMLLayout * layout;
    systemObject * currObject;
    NineMLLayout * editLayout;
    QLabel * errors;
    systemmodel * sysModel;
    QFrame * toolbar;
};

struct viewCLstruct {
     RootComponentItem * root;
     ALView * display;
     QToolBar * toolBar;
     QDockWidget * dock;
     QFrame * frame;
     QHBoxLayout * layout;
     QWidget * propertiesContents;
     QListWidget * fileList;
     QVBoxLayout * fileListLayout;
     QPushButton * toggleFileBox;
     MainWindow * mainWindow;
     QMainWindow * subWin;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    rootData data;
    QUndoGroup * undoStacks;
    viewGVstruct viewGV;
    viewELstruct viewEL;
    viewNLstruct viewNL;
    viewVZstruct viewVZ;
    viewCLstruct viewCL;
    viewELExptPanelHandler * viewELhandler;
    viewVZLayoutEditHandler * viewVZhandler;
    QShortcut * deleteShortcut;
    void addComponentsToFileList();
    QString toolbarStyleSheet;
    void setProjectMenu();

private:
    Ui::MainWindow *ui;
    GLWidget *glWidget;
    void createActions();
    bool unsaved_changes;
#ifdef Q_OS_MAC
    QTimer fix;
#endif

    QAction *undoAction;
    QAction *redoAction;
    QDomDocument tempDoc;
    void initViewEL();
    void connectViewEL();
    void initViewGV();
    void connectViewGV();
    void initViewCL();
    void connectViewCL();
    void initViewVZ();
    void connectViewVZ();
    void configureVCSMenu();
    bool isChanged();
    bool promptToSave();
    void clearComponents();

public slots:
    void import_project();
    void export_project();
    void export_project_as();
    void close_project();
    void import_network();
    void export_network();
    void import_component();
    void export_component();
    void duplicate_component();
    void delete_component();
    void import_layout();
    void export_layout();
    void import_csv();
    void viewGVshow();
    void viewELshow();
    void viewNLshow();
    void viewVZshow();
    void viewCLshow();
    void setCaption(QString);
    void runInBRAHMS();
    void new_project();
    void saveImageAction();
    void launchSimulatorEditor();
    void initialiseModel(NineMLComponent *);
    void updateNetworkButtons(rootData *);
    void undoOrRedoPerformed(int);

    // AL editor slots
    void actionAs_Image_triggered();
    void actionAs_Dotty_Graph_triggered();
    void actionNew_triggered();

    void actionSelectMode_triggered();
    void actionShowHideParams_triggered(bool checked);
    void actionShowHidePorts_triggered(bool checked);
    void actionMove_Up_triggered();
    void actionMove_Down_triggered();
    void actionDeleteItems_triggered();
    void actionZoomIn_triggered();
    void actionZoomOut_triggered();

    void actionAddParamater_triggered();
    void actionAddRegime_triggered();
    void actionAddOnCondition_triggered();
    void actionAddTimeDerivative_triggered();
    void actionAddAnalogePort_triggered();
    void actionAddEventPort_triggered();
    void actionAddImpulsePort_triggered();
    void actionAddStateVariable_triggered();
    void actionAddAlias_triggered();
    void actionAddStateAssignment_triggered();
    void actionAddEventOut_triggered();
    void actionAddImpulseOut_triggered();
    void actionAddOnEvent_triggered();
    void actionAddOnImpulse_triggered();
    void about();

    // versioning
    void actionCommitModel_triggered();
    void actionUpdateModel_triggered();
    void actionRevertModel_triggered();
    void actionRepStatus_triggered();
    void actionRepLog_triggered();
    void actionRescanVCS_triggered();

    //void storeNineMLComponent();

    void updateTitle(bool);
    void updateTitle();

    void fileListToggle();
    void fileListItemChanged(QListWidgetItem *current, QListWidgetItem *);
#ifdef Q_OS_MAC
    // init gl for osx
    void osxHack();
#endif

signals:
    void import_component_xml(QStringList fileNames);
    void import_layout_xml(QStringList fileNames);
    void import_model_xml(QString fileName);
    void export_component_xml(QString fileName, NineMLComponent * component);
    void export_layout_xml(QString fileName, NineMLLayout * component);
    void export_model_xml(QString fileName);
    void import_csv_signal(QString fileName);
    void launchComponentSorter();
    void mathText(QString);
    void statusBarUpdate(QString, int);
    void updatePanel(rootData *);
    void saveImage();



protected:
    void closeEvent(QCloseEvent *event);


};

#endif // MAINWINDOW_H
