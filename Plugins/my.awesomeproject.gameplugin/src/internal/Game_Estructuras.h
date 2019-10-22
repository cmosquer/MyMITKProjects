/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef Game_Estructuras_h
#define Game_Estructuras_h

#include <QmitkAbstractView.h>
#include <mitkSurface.h>
#include "vtkRANSACPlane.h"
#include <vector>
#include <vtkSmartPointer.h>
#include <mitkLookupTable.h>
#include <mitkPointSet.h>
#include <mitkLabelSetImage.h>
#include <qtablewidget.h>
#include <QTableWidget>
//#include <QVTKWidget.h>
#include <QVTKOpenGLWidget.h>
#include "itkImage.h"
#include "itkImageFileWriter.h"
// There's an item "FerulaControls.ui" in the UI_FILES list in
// files.cmake. The Qt UI Compiler will parse this file and generate a
// header file prefixed with "ui_", which is located in the build directory.
// Use Qt Creator to view and edit .ui files. The generated header file
// provides a class that contains all of the UI widgets.
#include <ui_GameControls.h>
#include <ui_GamePlay.h>
#include "GamePlay.h"

#include<bits/stdc++.h>
using namespace std;

// All views in MITK derive from QmitkAbstractView. You have to override
// at least the two methods CreateQtPartControl() and SetFocus().
class Game_Estructuras : public QmitkAbstractView
{
	// As QmitkAbstractView derives from QObject and we want to use the Qt
	// signal and slot mechanism, we must not forget the Q_OBJECT macro.
	// This header file also has to be listed in MOC_H_FILES in files.cmake,
	// in order that the Qt Meta-Object Compiler can find and process this
	// class declaration.
	Q_OBJECT



public:
	// This is a tricky one and will give you some headache later on in
	// your debug sessions if it has been forgotten. Also, don't forget
	// to initialize it in the implementation file.
	static const std::string VIEW_ID;
  Game_Estructuras();
  virtual ~Game_Estructuras() override;


  // In this method we initialize the GUI components and connect the
  // associated signals and slots.
  void CreateQtPartControl(QWidget* parent) override;

  //Widget de Play:
  QWidget *playWidget;
  vector<QVTKOpenGLWidget*> vtkWindows;
  QPushButton *pbStart;
   /*
  QPushButton *pbExport;
  QTableWidget *tableWidget;
  QLabel *titulo;
  QWidget *ppppp;
  */

private slots:

  //void createInteractor();
  void onCrearJuego();
  void onAgregarItem();
  void onConfirmarPreg(int mode);
  void onSimpleConfirmarPreg();
  void onAgregarOpcion();
  void onEliminarItem();
  void onCancelarJuego();
  void onConfirmarItem();
  void onCancelarItem();
  void onJugarDemo();
  void onStart();
  void onMostrarItem(int i);
  void onCargarJuego();
  void onAtras();
  void onConfirmar();
  void onToggle();
  void onGenerico();

private:
  // Typically a one-liner. Set the focus to the default widget.
  void SetFocus() override;
  //void widgetToNodesView();
  //void widgetToItemsView();
  void simpleAgregarOpcion();
  void agregarOpcion(int mode);
  void changeScreen(int);
  void prepararResumen();

  // This method is conveniently called whenever the selection of Data Manager
  // items changes.
//  void OnSelectionChanged(
 //   berry::IWorkbenchPart::Pointer source,
  //  const QList<mitk::DataNode::Pointer>& dataNodes) override;

  Ui::GameControls                    m_Controls;
  GamePlay *m_Play;
  //HighlightStructureInteractor::Pointer             m_CurrentInteractor;
  unsigned long N_items;
  string game_name;
  vector <unsigned long> N_options;
  //mitk::DataNode::Pointer m_currentQuestion;
  //vector <mitk::DataNode::Pointer> m_currentAnswer;
  bool m_flag_hay_correcta;
  bool m_standard_options_flag;
  int current_screen;
  string m_option_summary;
  string m_question_summary;
  vector <std::string> items_summaries;
  vector <std::string> standard_options_names;
  QLineEdit *nodesView;
  QHBoxLayout *itemsView;
  int MAX_OPCS;
};

#endif
