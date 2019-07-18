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

#ifndef AwesomeView_h
#define AwesomeView_h

#include <berryISelectionListener.h>
#include <QmitkAbstractView.h>
#include <mitkSurface.h>
#include "vtkRANSACPlane.h"
#include <vector>
#include <vtkSmartPointer.h>
#include <mitkLookupTable.h>
#include <mitkPointSet.h>

#include <qtablewidget.h>
#include <QTableWidget>

// There's an item "AwesomeViewControls.ui" in the UI_FILES list in
// files.cmake. The Qt UI Compiler will parse this file and generate a
// header file prefixed with "ui_", which is located in the build directory.
// Use Qt Creator to view and edit .ui files. The generated header file
// provides a class that contains all of the UI widgets.
#include <ui_AwesomeViewControls.h>

// All views in MITK derive from QmitkAbstractView. You have to override
// at least the two methods CreateQtPartControl() and SetFocus().
class AwesomeView : public QmitkAbstractView
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
	std::vector<vtkSmartPointer<vtkRANSACPlane> > planoIO;
	std::vector<vtkSmartPointer<vtkRANSACPlane> > planoPQ;
	std::vector<vtkSmartPointer<vtkPlaneSource> > planoOBJ;
	int N_casos;
	std::vector<int*> semaforo_desempeno;
	//int N=10; //Cantidad de casos i.e. cant de planos objetivos
	//int ann_starts[N]={0};
	//int ann_ends[N]={0};
	mitk::LookupTable::Pointer mitkLut;
	int flag_SeInicio;

	//Widget de resultados: 
	QWidget *resultsWidget;
	QPushButton *pbExport;
	QTableWidget *tableWidget;
	QLabel *titulo;
	QWidget *ppppp;
	double *distancias;
	double *angulos;
  AwesomeView();

  // In this method we initialize the GUI components and connect the
  // associated signals and slots.
  void CreateQtPartControl(QWidget* parent) override;


private slots:


  //void CalcularInliersPQ_conPlanoObjetivo();
  vtkSmartPointer<vtkPolyData>  CalcularInliersPQ_sinPointSet();
  void AjustarPlanoPQ();
  void AjustarPlanoIO();
  //double Hausdorff(vtkPolyData *pq, vtkPolyData *hueso);
  void onUL(double);
  void onLL(double);
  void onBox_Indicadores();
  double box_Planos(int);
  void Colorimetria();
  double Iniciar();
  void onCalcularInliers();
  void onPlaneNumber(int);
  void onDefinirTotal();
  void onModo();
  void onEstimar();
  void onRestringirAlHueso();
  void onExportarDatos();
private:
  // Typically a one-liner. Set the focus to the default widget.
  void SetFocus() override;

  // This method is conveniently called whenever the selection of Data Manager
  // items changes.
  void OnSelectionChanged(
    berry::IWorkbenchPart::Pointer source,
    const QList<mitk::DataNode::Pointer>& dataNodes) override;
  int RestringirAlHueso(int PQ_o_IO);
  vtkRANSACPlane* AjustarPlano(vtkPolyData* inliers);
  int CalcularInliersPQ(int case_number);
  double CalcularIndicadorISO(vtkPolyData *plano_ejecutado, vtkPlane *plano_objetivo);
  double CalcularIndicadorPpk(vtkPolyData *plano_ejecutado, vtkPlane *plano_objetivo);
  double CalcularIndicadorTolerancia(vtkPolyData *plano_ejecutado, vtkPlane *plano_objetivo);
 // mitk::DataNode* encontrarNodo(QRegExp regexp);
  void OpenAnnotations();
  void GetPlanosObjetivo();
  int DefinirTotalIO(QTableWidget *tableWidget);
  int DefinirTotalPQ(QTableWidget *tableWidget);
  int DefinirComparacion(QTableWidget *tableWidget);
  void dividirPuntosIntraoperatorios(vtkPoints *total_puntos);
  void ExportarDatos_DosMetodos(QTableWidget *tableWidget, QString file_name);
  void ExportarDatos_UnMetodo(QTableWidget *tableWidget, QString file_name);
  double* CalcularIndicadorTumor(vtkPolyData *tumor, vtkPolyData *bounded_plane, int tipo);
  vtkSmartPointer<vtkPolyData> RestringirPlanoAlHueso(vtkRANSACPlane *plano_ejecutado, vtkPlaneSource *plano_objetivo);
  void box_Indicadores(int item, double* results);
  vtkSmartPointer<vtkDoubleArray> CalcularDistancias(vtkPolyData *boundedPlane, vtkPlane *plano_objetivo);
  int GetColorIndicador(int type, double value);
  //double CalcularIndicadorPpk(vtkRANSACPlane *plano_ejecutado, vtkPlaneSource *plano_objetivo);
  void CalcularIndicadoresPlanos(vtkPolyData *boundedPlane_PQ, vtkPolyData *boundedPlane_IO, int case_number);
  //double CalcularIndicadorISO(vtkPolyData* plano_ejecutado, vtkPolyData* plano_objetivo);
  // Generated from the associated UI file, it encapsulates all the widgets
  // of our view.
  Ui::AwesomeViewControls m_Controls;
};

#endif
