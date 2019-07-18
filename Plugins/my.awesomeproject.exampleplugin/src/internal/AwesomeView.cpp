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
//Que la tabla de reusltados se expanda a toda la ventana 



#include <iostream>
#include <string>


#include "AwesomeView.h"
#include "vtkRANSACPlane.h"

#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

#include <usModuleRegistry.h>

#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkPlane.h>
#include <vtkTriangle.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkThresholdPoints.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkDoubleArray.h>
#include <vtkSortDataArray.h>
#include <vtkDelaunay2D.h>
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkLookupTable.h>
#include <vtkCenterOfMass.h>
#include <vtkDistancePolyDataFilter.h>
#include <vtkCleanPolyData.h>
#include<vtkCellLocator.h>
#include <vtkOBBTree.h>
#include <vtkIdList.h>
#include <vtkPointLocator.h>
#include <vtkKdTree.h>

#include <itkMapContainer.h>

#include <mitkMapper.h>
#include <vtkPlaneSource.h>
#include <mitkDataNode.h>
#include <mitkSurface.h>
#include <mitkPointSet.h>
#include <mitkPlaneFit.h>
#include <mitkLookupTable.h>
#include <mitkVtkScalarModeProperty.h>
#include <mitkLookupTableProperty.h>
#include <mitkNodePredicateDataType.h>
#include <mitkPointLocator.h>


#include <QMessageBox>
#include <QString>
#include <QSettings>
#include <QFileDialog>
#include <QTableWidget>
#include <QInputDialog>

#include <AwesomeImageFilter.h>
#include <AwesomeImageInteractor.h>

# define M_PI				3.14159265358979323846  /* pi */
#define PERCENTILE_99865	99865   //dividir por 10000 para obtener el percentil
#define PERCENTILE_0135		135

#define MIN_DIST_SCALE		5
#define MAX_DIST_SCALE		5

#define UNCERTAINTY			0.6
#define ANGULAR_UNCERTAINTY 5

#define MARG_ONC_MINIMO		1
#define MARG_ONC_IDEAL		4
#define	MARG_ONC_DEMASIADO	10

#define INIT_VAL	1000

//Colores RGB para los objetos

//-----PIEZA QUIRURGICA-----//
#define R_PQ 242
#define G_PQ 177
#define B_PQ 91
//----PLANO OBJETIVO 0-----//
#define R_PO0 255
#define G_PO0 0
#define B_PO0 0

//----PLANO OBJETIVO 2-----//
#define R_PO1 255
#define G_PO1 255
#define B_PO1 0

//----PLANO OBJETIVO 3-----//
#define R_PO2 0
#define G_PO2 0
#define B_PO2 255

//----PLANO OBJETIVO 4-----//
#define R_PO3 51
#define G_PO3 255
#define B_PO3 255

//----------HUESO----------//
#define R_H 255
#define G_H 255
#define B_H 255
//-PUNTOS INTRAOPERATORIOS-//
#define R_PI 255
#define G_PI 102
#define B_PI 255
//----TUMOR-------//
#define R_T 0
#define G_T 255
#define B_T 0
//-------INLIERS PQ--------//
#define R_INL 255
#define G_INL 229
#define B_INL 204
//-PLANO PIEZA QUIRURGICA--//
#define R_PLANO_PQ 255
#define G_PLANO_PQ 128
#define B_PLANO_PQ 0
//---PLANO PTOS INTRAOP---//
#define R_PLANO_IO 255
#define G_PLANO_IO 0
#define B_PLANO_IO 255



using namespace std;

AwesomeView::AwesomeView()
{
	vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();

	lut->SetTableRange(-1.0*MIN_DIST_SCALE, 1.0*MAX_DIST_SCALE);
	lut->Build();

	mitkLut = mitk::LookupTable::New();
	mitkLut->SetVtkLookupTable(lut);
	flag_SeInicio = 0;
	
	resultsWidget = new QWidget;
	titulo = new QLabel(resultsWidget);
	tableWidget = new QTableWidget(resultsWidget);
	pbExport = new QPushButton(resultsWidget);
	ppppp = new QWidget;

	resultsWidget->setSizeIncrement(4, 5);
	resultsWidget->hide();
	resultsWidget->setMinimumSize(1000, 600);

	tableWidget->verticalHeader()->hide();
	tableWidget->horizontalHeader()->hide();
	tableWidget->setEnabled(false);

	titulo->setGeometry(300, 0, 700, 90);
	pbExport->setText("Exportar resultados");
	pbExport->setGeometry(800, 600, 200, 40);
	pbExport->setStyleSheet("QPushButton {background-color: black; color: white;}");
	connect(pbExport, SIGNAL(clicked()), this, SLOT(onExportarDatos()));
}

const std::string AwesomeView::VIEW_ID = "my.awesomeproject.views.awesomeview";

void AwesomeView::CreateQtPartControl(QWidget* parent)
{
  // Setting up the UI is a true pleasure when using .ui files, isn't it?
  m_Controls.setupUi(parent);

  //m_Controls.listResults->addItem("Distancia máxima (ISO)");
  // Wire up the UI widgets with our functionality.
  m_Controls.boxIndicadores->addItem("Indicador ISO");
  m_Controls.boxIndicadores->addItem("Indicador Ppk");
  m_Controls.boxIndicadores->addItem("% en Tolerancia");
  m_Controls.boxIndicadores->addItem("Margen oncológico");
  m_Controls.boxPlanos->addItem("Ángulo y centroide");  //Comparacion entre PQ (gold estandar) contra intraop
  m_Controls.boxPlanos->addItem("Normales");
  m_Controls.cbModo->addItem("Sólo pieza quirúrgica");
  m_Controls.cbModo->addItem("Sólo puntos intraoperatorios");
  m_Controls.cbModo->addItem("Comparación");
 
  connect(m_Controls.pbInliers, SIGNAL(clicked()),this, SLOT(onCalcularInliers()));
  connect(m_Controls.pbPlanoPQ,SIGNAL (clicked()),this,SLOT(AjustarPlanoPQ()));
  connect(m_Controls.pbPlanoIO,SIGNAL (clicked()),this,SLOT(AjustarPlanoIO()));
  connect(m_Controls.boxIndicadores, SIGNAL(currentIndexChanged(int)),this,SLOT(onBox_Indicadores()));
  connect(m_Controls.pbRestringirPlanos, SIGNAL(clicked()), this, SLOT(onRestringirAlHueso()));
  connect(m_Controls.UL, SIGNAL(valueChanged(double)), this, SLOT(onUL(double)));
  connect(m_Controls.LL, SIGNAL(valueChanged(double)), this, SLOT(onLL(double)));
  connect(m_Controls.boxPlanos, SIGNAL(currentIndexChanged(int)), this, SLOT(box_Planos(int)));
  connect(m_Controls.chbColorimetria, SIGNAL(toggled(bool)), this, SLOT(Colorimetria()));
  connect(m_Controls.pbIniciar, SIGNAL(clicked()), this, SLOT(Iniciar()));
  connect(m_Controls.cbPlaneNumber, SIGNAL(activated(int)), this, SLOT(onPlaneNumber(int)));
  connect(m_Controls.pbTotal, SIGNAL(clicked()), this, SLOT(onDefinirTotal()));
  connect(m_Controls.cbModo, SIGNAL(activated(int)), this, SLOT(onModo()));
  connect(m_Controls.pbEstimar,SIGNAL(clicked()),this,SLOT(onEstimar()));

  m_Controls.cbPlaneNumber->setVisible(false);
  m_Controls.label_10->setVisible(false);
  m_Controls.fraValidacion->setVisible(false);
  m_Controls.fraResultados->setVisible(false);
  m_Controls.chbColorimetria->setVisible(false);
  m_Controls.pbIniciar->setVisible(false);
  m_Controls.pbEstimar->setVisible(false);
  m_Controls.pbTotal->setVisible(false);




}

void AwesomeView::onEstimar()
{
  int case_number=m_Controls.cbPlaneNumber->currentIndex();
  int salir=1;
  int modo = m_Controls.cbModo->currentIndex();

  if (modo == 0)
  {
	  //solo pq
	  salir = CalcularInliersPQ(case_number);
	  if (salir != 0)
	  {
		  AjustarPlanoPQ();
		  RestringirAlHueso(0);

	  }
  }

  if(modo==1)
  {
    //solo puntos
    AjustarPlanoIO();
    RestringirAlHueso(1);
  }
  if (salir != 0)
  {
	  m_Controls.chbColorimetria->setEnabled(true);
	  m_Controls.chbColorimetria->setVisible(true);
	  m_Controls.fraResultados->setEnabled(true);
	  m_Controls.titResultados->setEnabled(true);
	  m_Controls.titResultados->setVisible(true);

	  m_Controls.pbTotal->setEnabled(true);
  }
}
void AwesomeView::onModo()
{
  m_Controls.pbIniciar->setEnabled(true);
  m_Controls.pbIniciar->setVisible(true);


}
void AwesomeView::SetFocus()
{
  m_Controls.pbInliers->setFocus();
}

void AwesomeView::onUL(double value)
{
  onBox_Indicadores();
}
void AwesomeView::onLL(double value)
{
  onBox_Indicadores();
}
void AwesomeView::onPlaneNumber(int index)
{
	for (vtkIdType i = 0; i < N_casos; i++)
	{
		string name = "Plano objetivo " + to_string(i);
		mitk::DataNode::Pointer current_node = GetDataStorage()->GetNamedNode(name);
    if (current_node)
    {
      if (i == index)
        current_node->SetVisibility(true);
      else
        current_node->SetVisibility(false);
    }


    name = "Peor margen " + to_string(i) + "-PQ";
    current_node = GetDataStorage()->GetNamedNode(name);
    if(current_node)
    {
      if (i == index)
        current_node->SetVisibility(true);
      else
        current_node->SetVisibility(false);
    }

	name = "Peor margen " + to_string(i)+"-Puntos";
	current_node = GetDataStorage()->GetNamedNode(name);
	if (current_node)
	{
		if (i == index)
			current_node->SetVisibility(true);
		else
			current_node->SetVisibility(false);
	}

    if(m_Controls.cbModo->currentIndex()!=1)
    {
        name="InliersPQ " + to_string(i);
        current_node= GetDataStorage()->GetNamedNode(name);
        if(current_node)
        {
          if (i == index)
            current_node->SetVisibility(true);
          else
            current_node->SetVisibility(false);
        }
        name = "Plano ajustado según Pieza Quirúrgica " + to_string(i);
        current_node = GetDataStorage()->GetNamedNode(name);
        if(current_node)
        {
          if (i == index)
            current_node->SetVisibility(true);
          else
            current_node->SetVisibility(false);
        }

        name = "PointSet " + to_string(i);
        current_node = GetDataStorage()->GetNamedNode(name);
        if(current_node)
        {
          if (i == index)
            current_node->SetVisibility(true);
          else
            current_node->SetVisibility(false);
        }
    }

    if(m_Controls.cbModo->currentIndex()!=0)
    {
      name = "Puntos intraoperatorios " + to_string(i);
      mitk::DataNode::Pointer current_node2 = GetDataStorage()->GetNamedNode(name);
      if (current_node2)
      {
        if (i == index)
          current_node2->SetVisibility(true);
			else
        current_node2->SetVisibility(false);
      }
      name = "Plano ajustado según Puntos Intraoperatorios " + to_string(i);
      current_node = GetDataStorage()->GetNamedNode(name);
      if(current_node)
      {
        if (i == index)
          current_node->SetVisibility(true);
        else
          current_node->SetVisibility(false);
      }
    }
	}
	m_Controls.chbColorimetria->setChecked(false);
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}
void AwesomeView::OnSelectionChanged(berry::IWorkbenchPart::Pointer, const QList<mitk::DataNode::Pointer>& dataNodes)
{
  for (const auto& dataNode : dataNodes)
  {
    mitk::PointSet* ps = dynamic_cast<mitk::PointSet*>(dataNode->GetData());
    if (ps != NULL)
    {

    }
  }

  // Nothing is selected or the selection doesn't contain an image.
 // m_Controls.selectImageLabel->setVisible(true);
}

void AwesomeView::onCalcularInliers(){

	CalcularInliersPQ(m_Controls.cbPlaneNumber->currentIndex());
    /*vtkSmartPointer<vtkPolyData> polydataInliers = vtkSmartPointer<vtkPolyData>::New();
    polydataInliers = CalcularInliersPQ_sinPointSet();
    mitk::Surface::Pointer surfaceInliers= mitk::Surface::New();

    surfaceInliers->SetVtkPolyData(polydataInliers);

    mitk::DataNode::Pointer inliers_Node = mitk::DataNode::New();


    inliers_Node->SetData(surfaceInliers);

    inliers_Node->SetColor(R_INL/255.0,G_INL/255.0,B_INL/255.0);

    inliers_Node->SetName("InliersPQ");
    GetDataStorage()->Add(inliers_Node);*/

}

void AwesomeView::OpenAnnotations()
{
  QSettings settings;
  QVariant annPoints = settings.value("MaterialValidacionIntraoperatorio", ".");

  QString fileName = QFileDialog::getOpenFileName(NULL, "Open annotations file", annPoints.toString());
  if (fileName.isNull())
    return;

 
  settings.setValue("MaterialValidacionIntraoperatorio", QFileInfo(fileName).absoluteDir().absolutePath());

  mitk::DataNode* boneNode = GetDataStorage()->GetNamedNode("Hueso");
  if (!boneNode)
  {
      QMessageBox::warning(NULL, "FALTA CARGAR ARCHIVOS", "Por favor cargue la superficie del hueso.");
      return;
  }

  mitk::Point3D boneCenter = boneNode->GetData()->GetGeometry()->GetCenter();

  mitk::PointSet::Pointer pointSet = mitk::PointSet::New();


  std::ifstream fs;
  fs.open(fileName.toStdString().c_str());
  int pointId = 0;
  while (fs)
  {
    std::string line;
    std::getline(fs, line);


    std::transform(line.begin(), line.end(),line.begin(), ::toupper);
    size_t pos = line.find("POINT=");
    if (pos != std::string::npos)
    {
      pos = line.find("=");
      size_t comp1 = line.find(",", pos);
      std::string c1 = line.substr(pos+1, comp1-(pos+1));
      size_t comp2 = line.find(",", comp1+1);
      std::string c2 = line.substr(comp1+1, comp2 - (comp1+1));
      std::string c3 = line.substr(comp2+1);

      ++pointId;
      if (pointId < 1 || pointId > 100)
      {
        continue;
      }

	  setlocale(LC_ALL, "C");
	  mitk::Point3D point;
      point[0] = -1*atof(c1.c_str()) + boneCenter[0];
      point[1] = -1*atof(c2.c_str()) + boneCenter[1];
      point[2] = atof(c3.c_str());

      pointSet->InsertPoint(pointId, point);
    }
  }
  fs.close();

  vtkSmartPointer<vtkPoints> total_puntos =  vtkSmartPointer<vtkPoints>::New();
      //dynamic_cast<vtkPointSet*>(pointSet.GetPointer());


  for (mitk::PointSet::PointsConstIterator it = pointSet->Begin(); it != pointSet->End(); ++it)
  {

    total_puntos->InsertNextPoint(it->Value().GetDataPointer());

    }



  dividirPuntosIntraoperatorios(total_puntos);

  /*mitk::DataNode::Pointer node = mitk::DataNode::New();
  node->SetName("Puntos Intraoperatorios TOTAL");

  node->SetData(pointSet);

  GetDataStorage()->Add(node);
  */
}

void AwesomeView::dividirPuntosIntraoperatorios(vtkPoints *total_puntos)
{

  int N_total = total_puntos->GetNumberOfPoints();


  if (N_total<3)
  {
    QMessageBox::warning(NULL,"Cantidad insuficiente de puntos intraoperatorios", "El archivo que ha cargado tiene menos de tres puntos intraoperatorios, por favor cargue otro.");
    return;
  }


  int k=0;
  double tolerancia = 0.15;
  std::vector<int> indices;
  std::vector<vector<double>> normales;
  normales.resize(3);
  //int indices[N_casos]={0}; //HAY QUE CAMBIAR ESTO, YO NO SE CUANTOS LUGARES VA A TENER INDICES PORQ TENGO QUE ANALIZAR TODOS LOS POINTSETS, NO SLO LOS QUE CARGARON EL PLANO OBJETIVO. TENDRIA QUE SER UN PUNTERO TIPO STD::vECTOR.
  int ind=0;
  while(k<N_total)
  {

    double normal[3]={0};
    int j=k;
    double p0[3];
    double p1[3];
    double p2[3];
    total_puntos->GetPoint(j,p0);
    total_puntos->GetPoint(j+1,p1);
    total_puntos->GetPoint(j+2,p2);
    vtkTriangle::ComputeNormal(p0,p1,p2,normal);

    double pertenece=0;

    j=k+3;

    while((pertenece==0) && (j<=N_total))
    {

      int inside_tolerance=0;
      double new_normal[3]={0};
      double p0[3];
      double p1[3];
      double p2[3];
      total_puntos->GetPoint(j,p0);
      total_puntos->GetPoint(j-1,p1);
      total_puntos->GetPoint(j-2,p2);
      vtkTriangle::ComputeNormal(p0,p1,p2,new_normal);      //Comparamos la normal anterior con la normal al agregar un punto más. Si la normal cambio mucho, el punto no pertenece al mismo plano
      double error_relativo_0=abs(abs(normal[0])-abs(new_normal[0]))/abs(normal[0]);
      double error_relativo_1=abs(abs(normal[1])-abs(new_normal[1]))/abs(normal[1]);
      double error_relativo_2=abs(abs(normal[2])-abs(new_normal[2]))/abs(normal[2]);

      if(error_relativo_0<0.05)
        inside_tolerance++;
      if(error_relativo_1<0.05)
        inside_tolerance++;
      if(error_relativo_2<0.05)
        inside_tolerance++;

      pertenece=((error_relativo_0+error_relativo_1+error_relativo_2)/3)-2*inside_tolerance;
       if(pertenece<1)
      { //el punto pertenece
            pertenece=0;

            j++;
      }

      else
      {

        normales[0].push_back(normal[0]);
        normales[1].push_back(normal[1]);
        normales[2].push_back(normal[2]);
        pertenece=1;
        //k=j;
        ind++;

        if(j>=N_total)
          indices.push_back(N_total-1);
        else
          indices.push_back(j-1);

        }


     //Sale de este while cuando encuentra un punto que ya no pertenece. El punto con indice j ya no entraba

    }

  k=j;
  }

  int pertenece;
  tolerancia = 0.25;
  for (vtkIdType i = 0; i < N_casos; i++) //cambio de plano
  {
	  double* distances = new double[ind];
	  double min_distance = 25000;
	  int chosen_pointset = ind;
	  for (vtkIdType t = 0; t < ind; t++)  //cambio de pointset
	  {
		  if (indices[t] != 0)
		  {
			  int start = 0;
			  if (t > 0)
				  start = indices[t - 1] + 1;
			  distances[t] = 0;


			  pertenece = 0;
			  double *nor_plano = planoOBJ[i]->GetNormal();
			  if ((1 - tolerancia)*abs(1000 * nor_plano[0]) < abs(1000 * normales[0][t]) && abs(1000 * normales[0][t]) < (1 + tolerancia)*abs(1000 * nor_plano[0])) {
				  pertenece++;
			  }
			  if ((1 - tolerancia)*abs(1000 * nor_plano[1]) < abs(1000 * normales[1][t]) && abs(1000 * normales[1][t]) < (1 + tolerancia)*abs(1000 * nor_plano[1])) {
				  pertenece++;
			  }
			  if ((1 - tolerancia)*abs(1000 * nor_plano[2]) < abs(1000 * normales[2][t]) && abs(1000 * normales[2][t]) < (1 + tolerancia)*abs(1000 * nor_plano[2])) {
				  pertenece++;
			  }
			  if (pertenece > 0)
			  {	
				  for (vtkIdType p = start; p <= indices[t]; p++) //cambio de punto
				  {
					  double x[3];
					  total_puntos->GetPoint(p, x);
					  distances[t] = distances[t] + vtkPlane::DistanceToPlane(x, planoOBJ[i]->GetNormal(), planoOBJ[i]->GetOrigin());

				  }
				  if (distances[t] < min_distance)
				  {
					  min_distance = distances[t];
					  chosen_pointset = t;
				  }


			  }

		  }
	  }

	  //Cuando ya probe todos los pointsets en ese plano, le asigno el chosen
	  //vtkSmartPointer<vtkPoints> currentPointset = vtkSmartPointer<vtkPoints>::New();
	  if (chosen_pointset < ind)
	  {
		  mitk::PointSet::Pointer currentPointset = mitk::PointSet::New();
		  int start = 0;
		  if (chosen_pointset > 0)
			  start = indices[chosen_pointset - 1] + 1;
		  int h = 0;
		  for (vtkIdType l = start; l <= indices[chosen_pointset]; l++)
		  {

			  double x[3];
			  total_puntos->GetPoint(l, x);
			  //currentPointset->SetPoint(h,total_puntos->GetPoint(l));
			  currentPointset->InsertPoint(h, x);
			  h++;
		  }
		  //vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();

		  //pd->SetPoints(currentPointset);

		  //mitk::Surface::Pointer surf = mitk::Surface::New();
		  //surf->SetVtkPolyData(pd);
		  if (pertenece > 1)//Si o si este plano es de este pointset
		  {
			  indices[chosen_pointset] = 0;
		  }
		  mitk::DataNode::Pointer node = mitk::DataNode::New();
		  node->SetData(currentPointset);
		  switch (i)
		  {
		  case 0:
			  node->SetColor(R_PO0 / 255.0, G_PO0 / 255.0, B_PO0 / 255.0);
			  node->Update();
			  break;
		  case 1:
			  node->SetColor(R_PO1 / 255.0, G_PO1 / 255.0, B_PO1 / 255.0);
			  node->Update();
			  break;
		  case 2:
			  node->SetColor(R_PO2 / 255.0, G_PO2 / 255.0, B_PO2 / 255.0);
			  node->Update();
			  break;
		  case 3:
			  node->SetColor(R_PO3 / 255.0, G_PO3 / 255.0, B_PO3 / 255.0);
			  node->Update();
			  break;
		  }
		  
		  string name = "Puntos intraoperatorios " + std::to_string(i);
		  node->SetName(name);

		  GetDataStorage()->Add(node);
	  }
  }

}

double AwesomeView::Iniciar()
{



  if(!flag_SeInicio)
  {

  char flag_PQ=0;
  char flag_PO=0;
  char flag_H=0;
  //char flag_PI=0;



  mitk::TNodePredicateDataType<mitk::Surface>::Pointer predicateAllSurfaces =
    mitk::TNodePredicateDataType<mitk::Surface>::New();
  mitk::DataStorage::SetOfObjects::ConstPointer so = GetDataStorage()->GetSubset(predicateAllSurfaces);

  N_casos=0;

  for (mitk::DataStorage::SetOfObjects::ConstIterator it = so->Begin(); it != so->End(); ++it)
  {

    mitk::DataNode *current_node = it->Value();

    if(!current_node)
    {
      QMessageBox::warning(NULL,"FALTA CARGAR ARCHIVOS.", "Por favor cargue ");
      return 1;
    }
    QString current_name = QString::fromStdString(current_node->GetName());

  if (current_name.contains("PQ") || current_name.contains("pq") )
  {
    current_node->SetName("PQ");
    current_node->SetColor(R_PQ/255.0,G_PQ/255.0,B_PQ/255.0);
    current_node->Update();
    flag_PQ=1;
  }
  if (current_name.contains("CP") || current_name.contains("cp") || current_name.contains("Plano objetivo"))
  {
    string name="Plano objetivo" + std::to_string(N_casos);
    current_node->SetName(name);
    switch(N_casos)
    {
    case 0:
    current_node->SetColor(R_PO0/255.0,G_PO0/255.0,B_PO0/255.0);
	current_node->Update();
	break;
    case 1:
    current_node->SetColor(R_PO1/255.0,G_PO1/255.0,B_PO1/255.0);
	current_node->Update();
	break;
    case 2:
    current_node->SetColor(R_PO2/255.0,G_PO2/255.0,B_PO2/255.0);
	current_node->Update();
	break;
    case 3:
    current_node->SetColor(R_PO3/255.0,G_PO3/255.0,B_PO3/255.0);
	current_node->Update();
	break;

    }
    current_node->Update();
    flag_PO=1;
    N_casos++;
  }
  if (current_name.contains("tumor") || current_name.contains("Tumor") || current_name.contains("TUMOR"))
  {
	  current_node->SetName("Tumor");
	  current_node->SetColor(R_T / 255.0, G_T / 255.0, B_T / 255.0);
	  current_node->Update();
	  
  }
  if (current_name.contains("CT") || current_name.contains("ct") || current_name.contains("Hueso"))
  {
    current_node->SetName("Hueso");
    current_node->SetColor(R_H/255.0,G_H/255.0,B_H/255.0);
    current_node->SetOpacity(0.5);
    current_node->Update();
    flag_H=1;
  }
  }



  /*mitk::TNodePredicateDataType<mitk::PointSet>::Pointer predicateAllPointSets =  mitk::TNodePredicateDataType<mitk::PointSet>::New();
  mitk::DataStorage::SetOfObjects::ConstPointer so_ps = GetDataStorage()->GetSubset(predicateAllPointSets);
  for (mitk::DataStorage::SetOfObjects::ConstIterator it2 = so_ps->Begin(); it2 != so_ps->End(); ++it2)
  {
    mitk::DataNode *current_node2 = it2->Value();

    QString current_name2 = QString::fromStdString(current_node2->GetName());


  if (current_name2.contains("Ann") || current_name2.contains("ann") || current_name2.contains("Puntos intraoperatorios"))
  {
    current_node2->SetName("Puntos intraoperatorios");
    current_node2->SetColor(R_PI/255.0,G_PI/255.0,B_PI/255.0);
    current_node2->Update();
    flag_PI=1;
  }


  }*/


  QString error("");
  if(flag_PQ==0 && m_Controls.cbModo->currentIndex()!=1)
  {
  error = error + "Por favor cargue la pieza quirúrgica. \n";
  }

  if(flag_PO==0)
  {
    error = error + "Por favor cargue el plano objetivo. \n";
  }

  if(flag_H==0)
  {
    error = error + "Por favor cargue la superficie del hueso.\n";
  }

 /* if(flag_PI==0)
  {
    error = error + "Por favor cargue los puntos adquiridos de forma intraoperatoria.\n";
  }*/

  if(flag_H==0 || flag_PO==0  || (flag_PQ==0 && m_Controls.cbModo->currentIndex()!=1))
  {
    QMessageBox::warning(NULL, "FALTA CARGAR ARCHIVOS",error );
    return 1;
  }
  else
  {
	  int item = m_Controls.cbModo->currentIndex();
	  m_Controls.fraResultados->setVisible(true);
	  m_Controls.titResultados->setEnabled(true);
	  m_Controls.titResultados->setVisible(true);
	  m_Controls.cbPlaneNumber->setVisible(true);
	  m_Controls.label_10->setVisible(true);
	  m_Controls.cbPlaneNumber->setEnabled(true);
	  m_Controls.pbTotal->setVisible(true);
	  m_Controls.pbTotal->setEnabled(true);

	  if (item != 2)
	  {
		  m_Controls.label_4->setVisible(false);
		  m_Controls.boxPlanos->setVisible(false);
		  m_Controls.pbEstimar->setVisible(true);
	  }

	  if (item == 2)
	  {
		  m_Controls.fraValidacion->setVisible(true);
		  m_Controls.label_4->setVisible(true);
		  m_Controls.boxPlanos->setVisible(true);

	  }
  }

  GetPlanosObjetivo();

  if(m_Controls.cbModo->currentIndex()!=0)
  {
    QMessageBox::information(NULL, "Puntos intraoperatorios adquiridos con navegador", "A continuación, seleccione el archivo 'annotation_points.dat' de la carpeta del navegador.");

    OpenAnnotations();
  }
  distancias = new double[N_casos];
  angulos = new double[N_casos];

  for (int i = 0;i < N_casos ;i++ )
  {
	  distancias[i] = INIT_VAL;
	  angulos[i] = INIT_VAL;
  }

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();



  flag_SeInicio=1;
  return 0;
  m_Controls.pbIniciar->setEnabled(false);
  }
  else
  {
    QMessageBox::warning(NULL, "Este análisis ya fue iniciado.","Si desea reiniciar el análisis del caso, por favor cierre el Plugin y vuelva a abrirlo.");
    return 1;
  }

}

double AwesomeView::box_Planos(int item) {
	string name = "Plano ajustado según Pieza Quirúrgica " + to_string(m_Controls.cbPlaneNumber->currentIndex());
	mitk::DataNode::Pointer planoPQ_node = this->GetDataStorage()->GetNamedNode(name);
	name = "Plano ajustado según Puntos Intraoperatorios " + to_string(m_Controls.cbPlaneNumber->currentIndex());
	mitk::DataNode::Pointer planoIO_node = this->GetDataStorage()->GetNamedNode(name);

  if(!planoPQ_node || !planoIO_node)
  {
    QMessageBox::warning(NULL, "Falta algún plano ajustado.", "Por favor verifique haber ajustado plano para ambos métodos.");
    return 1;
  }
  vtkSmartPointer <vtkPolyData> boundedPlane_PQ = vtkSmartPointer <vtkPolyData> ::New();
  vtkSmartPointer <vtkPolyData> boundedPlane_IO = vtkSmartPointer <vtkPolyData> ::New();

	boundedPlane_PQ = dynamic_cast<mitk::Surface*>(planoPQ_node->GetData())->GetVtkPolyData();
	boundedPlane_IO = dynamic_cast<mitk::Surface*>(planoIO_node->GetData())->GetVtkPolyData();



  vtkSmartPointer<vtkPlane> vtkplane_PQ = vtkSmartPointer<vtkPlane>::New();
  vtkplane_PQ->SetOrigin(planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->origin);
  vtkplane_PQ->SetNormal(planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->normal);



  if (item == 2) //Colorimetria ENTRE plano intraop con plano de pq
  {

      vtkSmartPointer<vtkDoubleArray> distancias = vtkSmartPointer <vtkDoubleArray>::New();
      distancias = CalcularDistancias(boundedPlane_IO, vtkplane_PQ);
      boundedPlane_IO->GetPointData()->SetScalars(distancias);
      planoIO_node->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
      planoIO_node->SetFloatProperty("ScalarsRangeMinimum", -1*MIN_DIST_SCALE);
      planoIO_node->SetFloatProperty("ScalarsRangeMaximum", MAX_DIST_SCALE);
      planoIO_node->SetBoolProperty("scalar visibility", true);
      planoIO_node->SetBoolProperty("color mode", true);

      mitk::VtkScalarModeProperty::Pointer scalarMode = mitk::VtkScalarModeProperty::New();
      scalarMode->SetScalarModeToPointData();
      planoIO_node->SetProperty("scalar mode", scalarMode);
      planoIO_node->Update();

	}
  else
  {
    planoPQ_node->SetColor(R_PLANO_PQ/255.0,G_PLANO_PQ/255.0,B_PLANO_PQ/255.0);
    planoPQ_node->SetBoolProperty("scalar visibility", false);
    planoPQ_node->SetBoolProperty("color mode", false);

    planoIO_node->SetColor(R_PLANO_IO/255.0,G_PLANO_IO/255.0,B_PLANO_IO/255.0);
    planoIO_node->SetBoolProperty("scalar visibility", false);
    planoIO_node->SetBoolProperty("color mode", false);

	if (item == 0)//Angulo diedro y distancia entre centroides
	{
	int case_number = m_Controls.cbPlaneNumber->currentIndex();
	if(distancias[case_number]==INIT_VAL || angulos[case_number]==INIT_VAL)
	{//Angulo entre normales, en radianes

		CalcularIndicadoresPlanos(boundedPlane_PQ, boundedPlane_IO, case_number);
	}

	double distancia, angle;
	distancia = distancias[case_number];
	angle = angulos[case_number];


    QString tit;
    tit="Diferencia entre planos";
    QString PQ("Ángulo: ");
    QString PQ2 = QString::number(angle) + "°";
    QString IO("Distancia: ");
    QString IO2 = QString::number(distancia) + " mm";
		QString textDIST;
		QString textANG;
		switch (GetColorIndicador(4, distancia))
		{
			case 0: //distancia Malo
        textDIST = "<font size=4>%1</font><br><font color=red font size=4>%2</font>";
				break;
			case 1: //distancia regular
        textDIST = "<font size=4>%1</font><br><font color=yellow font size=4>%2</font>";
				break;
			case 2: //distancia buena
        textDIST = "<font size=4>%1</font><br><font color=green font size=4>%2</font>";
				break;				
		}
		switch (GetColorIndicador(5, angle))
		{
		case 0: //distancia Malo
      textANG = "<font size=4>%1</font><br><font color=red font size=4>%2</font>";
			break;
		case 1: //distancia regular
      textANG = "<font size=4>%1</font><br><font color=yellow font size=4>%2</font>";
			break;
		case 2: //distancia buena
      textANG = "<font size=4>%1</font><br><font color=green font size=4>%2</font>";
			break;
		}
		
		m_Controls.titResultados->clear();
		m_Controls.IOResultados->clear();
		m_Controls.PQResultados->clear();
    m_Controls.PQResultados->setText(textANG.arg(PQ,PQ2));
    m_Controls.IOResultados->setText(textDIST.arg(IO,IO2));
    QString text_tit;
    text_tit="<font size=4>%1</font>";
    m_Controls.titResultados->setText(text_tit.arg(tit));

	}
	if (item == 1)
	{

		double error_x, error_y, error_z;
		error_x = abs(planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->normal[0] - planoIO[m_Controls.cbPlaneNumber->currentIndex()]->normal[0]) / planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->normal[0];
		error_y = abs(planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->normal[1] - planoIO[m_Controls.cbPlaneNumber->currentIndex()]->normal[1]) / planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->normal[1];
		error_z = abs(planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->normal[2] - planoIO[m_Controls.cbPlaneNumber->currentIndex()]->normal[2]) / planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->normal[2];

    QString tit;
    tit="Normal de cada plano";
		QString PQ("Plano basado en pieza quirúrgica: \n");
		QString PQ2("X: " + QString::number(planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->normal[0]));
		QString PQ3("Y: " + QString::number(planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->normal[1]));
		QString PQ4("Z: " + QString::number(planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->normal[2]));
		QString IO("Plano basado en puntos intraoperatorio: \n");
		QString IO2("X: " + QString::number(planoIO[m_Controls.cbPlaneNumber->currentIndex()]->normal[0]) + ". \t Error: " + QString::number(error_x) + "%.");
		QString IO3("Y: " + QString::number(planoIO[m_Controls.cbPlaneNumber->currentIndex()]->normal[1]) + ". \t Error: " + QString::number(error_y) + "%.");
		QString IO4("Z : " + QString::number(planoIO[m_Controls.cbPlaneNumber->currentIndex()]->normal[2]) + ". \t Error : " + QString::number(error_z) + "%.");

		QString textpq("<FONT COLOR=orange font size=3>%1</font><br><font size=4>%2</font><br><font size=4>%3</font><br><font size=4>%4</font>");
		QString textio("<FONT COLOR=violet font size=3>%1</font><br><font size=4>%2</font><br><font size=4>%3</font><br><font size=4>%4</font>");

		m_Controls.titResultados->clear();
		m_Controls.IOResultados->clear();
		m_Controls.PQResultados->clear();

		m_Controls.PQResultados->setText(textpq.arg(PQ, PQ2, PQ3, PQ4));
		m_Controls.IOResultados->setText(textio.arg(IO, IO2, IO3, IO4));
    QString text_tit;
    text_tit="<font size=4>%1 </font>";
    m_Controls.titResultados->setText(text_tit.arg(tit));

	}
  }
 mitk::RenderingManager::GetInstance()->RequestUpdateAll();
 return 0;
}

/*double AwesomeView::Hausdorff(vtkPolyData *pq, vtkPolyData *hueso)
{
	//Calcular la distancia de hausdorff entre dos superficies. Solo se considera el contorno de las regiones oseas comunes, y no toda la pieza quirurgica. 
	//eso lo lograremos pidiendo todos los inliers y usando todos los puntos de la pieza quirurgica que no sean inliers. 
	return 0;
}
*/

void AwesomeView::CalcularIndicadoresPlanos(vtkPolyData *boundedPlane_PQ, vtkPolyData *boundedPlane_IO, int case_number) 
{
	double angle, distancia;
	angle = vtkMath::AngleBetweenVectors(planoPQ[case_number]->normal, planoIO[case_number]->normal);

	angle = angle * 180 / M_PI; //Pasar a grados
	if (angle > 90)
		angle = 180 - angle;

	//Calculo de centroides de los planos
	vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter = vtkSmartPointer<vtkCenterOfMass>::New();

	centerOfMassFilter->SetInputData(boundedPlane_PQ);
	centerOfMassFilter->SetUseScalarsAsWeights(false);
	centerOfMassFilter->Update();
	double centerPQ[3];
	centerOfMassFilter->GetCenter(centerPQ);

	centerOfMassFilter->SetInputData(boundedPlane_IO);
	centerOfMassFilter->SetUseScalarsAsWeights(false);
	centerOfMassFilter->Update();

	double centerIO[3];
	centerOfMassFilter->GetCenter(centerIO);

	distancia = std::sqrt(vtkMath::Distance2BetweenPoints(centerIO, centerPQ));


	distancias[case_number] = distancia;
	angulos[case_number] = angle;
}

void AwesomeView::onBox_Indicadores()
{
  double *results = new double[2];
  box_Indicadores(m_Controls.boxIndicadores->currentIndex(),results);


}


void AwesomeView::box_Indicadores(int item, double *results)
{



  vtkSmartPointer<vtkPlane> planoOBJ_vtkplane = vtkSmartPointer<vtkPlane>::New();
  planoOBJ_vtkplane->SetNormal(planoOBJ[m_Controls.cbPlaneNumber->currentIndex()]->GetNormal());
  planoOBJ_vtkplane->SetOrigin(planoOBJ[m_Controls.cbPlaneNumber->currentIndex()]->GetOrigin());

  string name;

  name= "Plano ajustado según Pieza Quirúrgica " + to_string(m_Controls.cbPlaneNumber->currentIndex());
  mitk::DataNode::Pointer planoPQ_node = this->GetDataStorage()->GetNamedNode(name);
  vtkSmartPointer <vtkPolyData> boundedPlane_PQ = vtkSmartPointer <vtkPolyData> ::New();
  

  name = "Plano ajustado según Puntos Intraoperatorios " + to_string(m_Controls.cbPlaneNumber->currentIndex());
  mitk::DataNode::Pointer planoIO_node = this->GetDataStorage()->GetNamedNode(name);
  vtkSmartPointer <vtkPolyData> boundedPlane_IO = vtkSmartPointer <vtkPolyData> ::New();
  

  int modo=m_Controls.cbModo->currentIndex();
  if(modo!=1)
  {
      if(!planoPQ_node)
      {
        QMessageBox::warning(NULL, "Falta plano ajustado.", "Por favor verifique haber ajustado plano para método por Pieza Quirúrgica.");
      return;
      }
	  boundedPlane_PQ = dynamic_cast<mitk::Surface*>(planoPQ_node->GetData())->GetVtkPolyData();
  }

  if(modo!=0)
  {
      if(!planoIO_node)
      {
        QMessageBox::warning(NULL, "Falta algún plano ajustado.", "Por favor verifique haber ajustado plano para método por Puntos Intraoperatorios.");
      return;
      }
	  boundedPlane_IO = dynamic_cast<mitk::Surface*>(planoIO_node->GetData())->GetVtkPolyData();
  }



  if(item==0){
   double L_IO=0;
   double L_PQ=0;
   QString tit;
   tit="Distancia máxima al plano objetivo";

  m_Controls.titResultados->clear();

  QString text_tit;
  text_tit="<font size=4>%1</font>";
  m_Controls.titResultados->setText(text_tit.arg(tit));

   if(modo!=1)
    {
     L_PQ=CalcularIndicadorISO(boundedPlane_PQ, planoOBJ_vtkplane);
     results[0]=L_PQ;
	 QString PQ;
	 if(modo==2)
		 PQ="Con pieza quirúrgica: ";
     QString PQ2(QString::number(L_PQ) + "mm" );
     QString textpq;
     switch (GetColorIndicador(0, L_PQ))
     {
     case 0:
       textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=red font size=8>%2</font>";
       break;
     case 1:
       textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=yellow font size=8>%2</font>";
       break;
     case 2:
       textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=green font size=8>%2</font>";
       break;
     }

     m_Controls.PQResultados->clear();

     m_Controls.PQResultados->setText(textpq.arg(PQ,PQ2));

   }
   if (modo!=0)
   {
     L_IO=CalcularIndicadorISO(boundedPlane_IO, planoOBJ_vtkplane);
    results[1]=L_IO;
	QString IO = "";
	if (modo == 2)
		IO = "Con puntos intraoperatorio: ";
    QString IO2(QString::number(L_IO) + "mm" );
    QString textio;
    switch (GetColorIndicador(0, L_IO))
    {
   case 0:
     textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=red font size=8>%2</font>";
     break;
   case 1:
     textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=yellow font size=8>%2</font>";
     break;
   case 2:
     textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=green font size=8>%2</font>";
     break;
    }
      m_Controls.IOResultados->clear();
      m_Controls.IOResultados->setText(textio.arg(IO, IO2));
   }




  }

  if(item==1)
  {
    double Ppk_IO=0;
    double Ppk_PQ=0;
    QString tit;
    tit="Capacidad del proceso (Ppk)";
    m_Controls.titResultados->clear();
    QString text_tit;

    text_tit="<font size=4>%1</font>";
    m_Controls.titResultados->setText(text_tit.arg(tit));
    if(modo!=1)
    {

     Ppk_PQ=CalcularIndicadorPpk(boundedPlane_PQ,planoOBJ_vtkplane);
     results[0]=Ppk_PQ;

      QString textpq;
	  QString PQ=" ";
	  if (modo == 2)
		  PQ = "Con pieza quirúrgica: ";
      QString PQ2(QString::number(Ppk_PQ));
      switch (GetColorIndicador(1, Ppk_PQ))
      {
        case 0:
          textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=red font size=8>%2</font>";
         break;
        case 1:
          textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=yellow font size=8>%2</font>";
          break;
        case 2:
          textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=green font size=8>%2</font>";
          break;
        }

        m_Controls.PQResultados->clear();

        m_Controls.PQResultados->setText(textpq.arg(PQ, PQ2));
    }
    if(modo!=0)
    {
      Ppk_IO=CalcularIndicadorPpk(boundedPlane_IO,planoOBJ_vtkplane);
      results[1]=Ppk_IO;
	  QString IO = "";
	  if (modo == 2)
		  IO = "Con puntos intraoperatorio: ";
      QString IO2(QString::number(Ppk_IO));
      QString textio;
      switch (GetColorIndicador(1, Ppk_IO))
      {
      case 0:
        textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=red font size=8>%2</font>";
        break;
      case 1:
        textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=yellow font size=8>%2</font>";
        break;
      case 2:
        textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=green font size=8>%2</font>";
        break;
      }
      m_Controls.IOResultados->clear();
      m_Controls.IOResultados->setText(textio.arg(IO, IO2));
    }



  }

  if (item == 2)
  {
	  double Porc_IO = 0;
	  double Porc_PQ= 0;

    QString tit;
    tit="Área de corte dentro \n de tolerancia";
    m_Controls.titResultados->clear();

    QString text_tit;
    text_tit="<font size=4>%1</font>";
    m_Controls.titResultados->setText(text_tit.arg(tit));

    if(modo!=1)
    {
        Porc_PQ = CalcularIndicadorTolerancia(boundedPlane_PQ, planoOBJ_vtkplane);
        results[0]=Porc_PQ;
        QString textpq;
		QString PQ=" ";
		if (modo == 2)
			PQ = "Con pieza quirúrgica: ";
        QString PQ2(QString::number(Porc_PQ) + "%" );
        switch (GetColorIndicador(2, Porc_PQ))
        {
        case 0:
          textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=red font size=8>%2</font>";
          break;
        case 1:
          textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=yellow font size=8>%2</font>";
          break;
        case 2:
          textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=green font size=8>%2</font>";
          break;
        }
        m_Controls.PQResultados->clear();

        m_Controls.PQResultados->setText(textpq.arg(PQ, PQ2));
    }
    if(modo!=0)
    {
        Porc_IO = CalcularIndicadorTolerancia(boundedPlane_IO, planoOBJ_vtkplane);
        results[1]=Porc_IO;
        QString textio;
		QString IO="";
        if (modo==2)
			IO="Con puntos intraoperatorio: ";
        QString IO2(QString::number(Porc_IO) + "%" );
        switch (GetColorIndicador(2, Porc_IO))
        {
        case 0:
          textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=red font size=8>%2</font>";
          break;
        case 1:
          textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=yellow font size=8>%2</font>";
          break;
        case 2:
          textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=green font size=8>%2</font>";
          break;
        }
        m_Controls.IOResultados->clear();
        m_Controls.IOResultados->setText(textio.arg(IO, IO2));
    }


  }
  if (item == 3) //distancia a tumor
  {
    QString tit;
    tit="Margen oncológico \n (distancia mínima al tumor)";
    m_Controls.titResultados->clear();
    QString text_tit;
    text_tit="<font size=4>%1</font>";
    m_Controls.titResultados->setText(text_tit.arg(tit));
	  mitk::DataNode* tumor_node = this->GetDataStorage()->GetNamedNode("Tumor");
	  if (!tumor_node)
	  {
		  QMessageBox::warning(NULL, "Falta cargar el tumor.", "No se encontró nodo llamado 'Tumor'.");
      return;
	  }
	  vtkSmartPointer<vtkPolyData> tumor = vtkSmartPointer<vtkPolyData>::New();
	  tumor = dynamic_cast<mitk::Surface*>(tumor_node->GetData())->GetVtkPolyData();

    if(modo!=1)
    {
        double *margenes_oncologicosPQ =CalcularIndicadorTumor(tumor, boundedPlane_PQ,0);
        results[0]=margenes_oncologicosPQ[0];
        QString textpq;
		QString PQ = " ";
		if (modo == 2)
			PQ = "Con pieza quirúrgica: ";
        QString PQ2(QString::number(margenes_oncologicosPQ[0])+" mm.");
        switch (GetColorIndicador(3, margenes_oncologicosPQ[0]))
        {
        case 0:
          textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=red font size=8>%2</font>";
          break;
        case 1:
          textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=yellow font size=8>%2</font>";
          break;
        case 2:
          textpq = "<FONT COLOR=orange font size=3>%1</font><br><FONT COLOR=green font size=8>%2</font>";
          break;
        }

        m_Controls.PQResultados->clear();
        m_Controls.PQResultados->setText(textpq.arg(PQ,PQ2));
    }
    if(modo!=0)
    {
          double *margenes_oncologicosIO =CalcularIndicadorTumor(tumor, boundedPlane_IO,1);
          results[1]=margenes_oncologicosIO[0];
		  QString IO = "";
		  if (modo == 2)
			  IO = "Con puntos intraoperatorio: ";
          QString IO2(QString::number(margenes_oncologicosIO[0])+" mm.");
          QString textio;

          switch (GetColorIndicador(3, margenes_oncologicosIO[0]))
          {
          case 0:
            textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=red font size=8>%2</font>";
            break;
          case 1:
            textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=yellow font size=8>%2</font>";
            break;
          case 2:
            textio = "<FONT COLOR=violet font size=3>%1</font><br><FONT COLOR=green font size=8>%2</font>";
            break;
          }
          m_Controls.IOResultados->clear();
          m_Controls.IOResultados->setText(textio.arg(IO,IO2));

      }




  }
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();

}

double* AwesomeView::CalcularIndicadorTumor(vtkPolyData *tumor, vtkPolyData *bounded_plane, int tipo)
{
  // Create the tree
  
  
  mitk::PointLocator::Pointer cellLocator = mitk::PointLocator::New();
  cellLocator->SetPoints(tumor);
  
//	vtkSmartPointer<vtkKdTree> cellLocator = vtkSmartPointer<vtkKdTree>::New();

//  cellLocator->SetDataSet(tumor);
 // cellLocator->BuildLocator();
  double *indicadores = new double [3];
  double mediana;
  double media=0;
  double minimo=25000;
  double min_coords[3];


  vtkSmartPointer<vtkDoubleArray> margenes_oncologicos = vtkSmartPointer<vtkDoubleArray>::New();
  margenes_oncologicos->SetNumberOfComponents(1);
  margenes_oncologicos->SetNumberOfValues(bounded_plane->GetNumberOfPoints());

  for(vtkIdType i=0; i<bounded_plane->GetNumberOfPoints();i++)
  {

  double testPoint[3];
  bounded_plane->GetPoint(i,testPoint);

  double closestPoint[3];
  //vtkSmartPointer<vtkIdList> result =  vtkSmartPointer<vtkIdList>::New();
  //vtkIdType k = 2;
  //Find the closest points to TestPoint
  /*//the coordinates of the closest point will be returned here
  double closestPointDist2; //the squared distance to the closest point will be returned here
  vtkIdType cellId; //the cell id of the cell containing the closest point will be returned here
  int subId; //this is rarely used (in triangle strips only, I believe)
  cellLocator->FindClosestPoint(testPoint, closestPoint, cellId, subId, closestPointDist2);
  margenes_oncologicos->SetValue(i,closestPointDist2);
  */
  //cellLocator->FindClosestNPoints(k, testPoint, result);
  //tumor->GetPoint(result->GetId(k-1),closestPoint);
  tumor->GetPoint(cellLocator->FindClosestPoint(testPoint),closestPoint);
  double closestPointDist2 = sqrt(static_cast<double>(vtkMath::Distance2BetweenPoints(testPoint,closestPoint)));

  //cout << "i: " << i << ". testpoint: " << mitk::Point3D(testPoint) << ". closestpoint: "<<mitk::Point3D(closestPoint)<<". D="<<closestPointDist2<<endl;
  //Para calcular indicadores:
  media= media + closestPointDist2;
  if (closestPointDist2 < minimo)
  {
	  minimo = closestPointDist2;
	 // minimo_index = closest;
	  min_coords[0] = closestPoint[0];
	  min_coords[1] = closestPoint[1];
	  min_coords[2] = closestPoint[2];
  }
  }
  mitk::PointLocator::Pointer cellLocator2 = mitk::PointLocator::New();
  cellLocator2->SetPoints(bounded_plane);
  double *new_min = bounded_plane->GetPoint(cellLocator2->FindClosestPoint(min_coords));
  double signo = 1.0;
  vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter = vtkSmartPointer<vtkCenterOfMass>::New();
  centerOfMassFilter->SetInputData(tumor);
  centerOfMassFilter->SetUseScalarsAsWeights(false);
  centerOfMassFilter->Update();
  double tumor_center[3];
  centerOfMassFilter->GetCenter(tumor_center);

  if (vtkMath::Distance2BetweenPoints(min_coords, tumor_center) > vtkMath::Distance2BetweenPoints(new_min, tumor_center))
	  signo = -1.0;

  minimo= signo*sqrt(vtkMath::Distance2BetweenPoints(min_coords,new_min));
  media=media/(bounded_plane->GetNumberOfPoints());


  mitk::PointSet::Pointer peor_margen = mitk::PointSet::New();
  peor_margen->InsertPoint(mitk::Point3D(min_coords));
  peor_margen->InsertPoint(mitk::Point3D(new_min));
  string tipo_str;
  if (tipo == 0) //PQ
  {
	  tipo_str = "-PQ";

  }
  else
  {
	  tipo_str = "-Puntos";

  }

  string name = "Peor margen " + std::to_string(m_Controls.cbPlaneNumber->currentIndex())+tipo_str;

  mitk::DataNode::Pointer peor_margen_nodo = GetDataStorage()->GetNamedNode(name);
  if (!peor_margen_nodo)
  {
	  peor_margen_nodo= mitk::DataNode::New();
	  peor_margen_nodo->SetName(name);
	  peor_margen_nodo->SetData(peor_margen);
	  GetDataStorage()->Add(peor_margen_nodo);
  }
 /* else
  {
	  if (m_Controls.cbModo->currentIndex() != 2)
	  {
		  peor_margen_nodo->SetData(peor_margen);
		  mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	  }
	  else
	  {
		  mitk::DataNode::Pointer peor_margen_nodo2 = mitk::DataNode::New();

		  name2 = name + tipo_str;
		  name = name + tipo_str2;

		  peor_margen_nodo2->SetName(name2);
		  peor_margen_nodo2->SetData(peor_margen);
		  GetDataStorage()->Add(peor_margen_nodo2);

		  peor_margen_nodo->SetName(name);
	  }
  }*/


  vtkSmartPointer<vtkSortDataArray> sorted_distancias = vtkSmartPointer<vtkSortDataArray>::New();
  sorted_distancias->Sort(margenes_oncologicos);
  int med=static_cast<int>(bounded_plane->GetNumberOfPoints()/2);
  mediana=margenes_oncologicos->GetValue(med);


  indicadores[0]=minimo;
  indicadores[1]=mediana;
  indicadores[2]=media;

  return indicadores;
}

double AwesomeView::CalcularIndicadorTolerancia(vtkPolyData *plano_ejecutado, vtkPlane *plano_objetivo) {
	double Porc = 0;
  vtkSmartPointer<vtkDoubleArray> distancias = vtkSmartPointer <vtkDoubleArray>::New();
	distancias = CalcularDistancias(plano_ejecutado, plano_objetivo);


	vtkSmartPointer<vtkSortDataArray> sorted_distancias = vtkSmartPointer<vtkSortDataArray>::New();
	sorted_distancias->Sort(distancias);
	int N = distancias->GetNumberOfValues();
	double UL = m_Controls.UL->value();
	double LL = -1 * m_Controls.LL->value();
	double curr_val;
	int contador = 0; 
	for (vtkIdType i = 0; i < N; i++) 
	{
		curr_val = distancias->GetValue(i);
		if (curr_val >= LL && curr_val<=UL)
			contador++;
		
	}

	Porc = 100.0 * static_cast<double>(contador) / static_cast<double>(N);

	return Porc;
}
void AwesomeView::Colorimetria() {
	static int on_off = 0;
	int modo = m_Controls.cbModo->currentIndex();

	string name = "Plano ajustado según Pieza Quirúrgica " + to_string(m_Controls.cbPlaneNumber->currentIndex());
	mitk::DataNode::Pointer planoPQ_node = this->GetDataStorage()->GetNamedNode(name);
	name = "Plano ajustado según Puntos Intraoperatorios " + to_string(m_Controls.cbPlaneNumber->currentIndex());
	mitk::DataNode::Pointer planoIO_node = this->GetDataStorage()->GetNamedNode(name);
	vtkSmartPointer <vtkPolyData> boundedPlane_PQ = vtkSmartPointer <vtkPolyData> ::New();
	vtkSmartPointer <vtkPolyData> boundedPlane_IO = vtkSmartPointer <vtkPolyData> ::New();

	if ((modo==0 && !planoPQ_node) || (modo==1 && !planoIO_node) || (!planoPQ_node && !planoIO_node))
	{
		QMessageBox::warning(NULL, "Falta algún plano ajustado.", "Por favor verifique haber ajustado plano para ambos métodos.");
		return;
	}


	if(modo!=1)
		boundedPlane_PQ = dynamic_cast<mitk::Surface*>(planoPQ_node->GetData())->GetVtkPolyData();
	if(modo!=0)
		boundedPlane_IO = dynamic_cast<mitk::Surface*>(planoIO_node->GetData())->GetVtkPolyData();


  vtkSmartPointer<vtkPlane> planoOBJ_vtkplane= vtkSmartPointer<vtkPlane>::New();
  planoOBJ_vtkplane->SetOrigin(planoOBJ[m_Controls.cbPlaneNumber->currentIndex()]->GetOrigin());
  planoOBJ_vtkplane->SetNormal(planoOBJ[m_Controls.cbPlaneNumber->currentIndex()]->GetNormal());

  if (on_off == 0) //AGregar colorimetria a ambos planos contra el plano objetivo
	{ 
    mitk::VtkScalarModeProperty::Pointer scalarMode = mitk::VtkScalarModeProperty::New();
    scalarMode->SetScalarModeToPointData();
	if (modo != 1)
	{
		//Para plano de pieza quirurgica
		vtkSmartPointer<vtkDoubleArray> distancias_PQ = vtkSmartPointer <vtkDoubleArray>::New();
		distancias_PQ = CalcularDistancias(boundedPlane_PQ, planoOBJ_vtkplane);
		boundedPlane_PQ->GetPointData()->SetScalars(distancias_PQ);
		boundedPlane_PQ->GetPointData()->SetScalars(distancias_PQ);

		mitk::Surface::Pointer surfpq = mitk::Surface::New();
		surfpq->SetVtkPolyData(boundedPlane_PQ);
		planoPQ_node->SetData(surfpq);
		planoPQ_node->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
		planoPQ_node->SetFloatProperty("ScalarsRangeMinimum", -1.0*MIN_DIST_SCALE);
		planoPQ_node->SetFloatProperty("ScalarsRangeMaximum", 1.0*MAX_DIST_SCALE);
		planoPQ_node->SetBoolProperty("scalar visibility", true);
		planoPQ_node->SetBoolProperty("color mode", true);
		planoPQ_node->SetProperty("scalar mode", scalarMode);
	}


	if (modo != 0)
	{
		//Para plano de puntos intraop
		vtkSmartPointer<vtkDoubleArray> distancias_IO = vtkSmartPointer <vtkDoubleArray>::New();
		distancias_IO = CalcularDistancias(boundedPlane_IO, planoOBJ_vtkplane);
		boundedPlane_IO->GetPointData()->SetScalars(distancias_IO);


		mitk::Surface::Pointer surfio = mitk::Surface::New();
		surfio->SetVtkPolyData(boundedPlane_IO);
		planoIO_node->SetData(surfio);
		planoIO_node->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
		planoIO_node->SetFloatProperty("ScalarsRangeMinimum", -1.0*MIN_DIST_SCALE);
		planoIO_node->SetFloatProperty("ScalarsRangeMaximum", 1.0*MAX_DIST_SCALE);
		planoIO_node->SetBoolProperty("scalar visibility", true);
		planoIO_node->SetBoolProperty("color mode", true);
		planoIO_node->SetProperty("scalar mode", scalarMode);
	}
    on_off=1;

	}	
  else
  {
   
	  if (modo != 1)
	  {
		  planoPQ_node->SetColor(R_PLANO_PQ / 255.0, G_PLANO_PQ / 255.0, B_PLANO_PQ / 255.0);
		  planoPQ_node->SetBoolProperty("scalar visibility", false);
		  planoPQ_node->SetBoolProperty("color mode", false);
	  }
	  if (modo != 0)
	  {
		  planoIO_node->SetColor(R_PLANO_IO / 255.0, G_PLANO_IO / 255.0, B_PLANO_IO / 255.0);
		  planoIO_node->SetBoolProperty("scalar visibility", false);
		  planoIO_node->SetBoolProperty("color mode", false);
		  
	  }
	  on_off = 0;
  }
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();

	}




double AwesomeView::CalcularIndicadorPpk(vtkPolyData *plano_ejecutado, vtkPlane *plano_objetivo) {
	double Ppk = 0;


  vtkSmartPointer<vtkPlane> planoOBJ_vtkplane= vtkSmartPointer<vtkPlane>::New();
  planoOBJ_vtkplane->SetOrigin(planoOBJ[m_Controls.cbPlaneNumber->currentIndex()]->GetOrigin());
  planoOBJ_vtkplane->SetNormal(planoOBJ[m_Controls.cbPlaneNumber->currentIndex()]->GetNormal());

  vtkSmartPointer<vtkDoubleArray> distancias = vtkSmartPointer <vtkDoubleArray>::New();
	distancias = CalcularDistancias(plano_ejecutado, plano_objetivo);


	vtkSmartPointer<vtkSortDataArray> sorted_distancias = vtkSmartPointer<vtkSortDataArray>::New();
	sorted_distancias->Sort(distancias);
	int N = distancias->GetNumberOfValues();
	int index_percentile_99 = static_cast<int>(PERCENTILE_99865 * N / 100000);
	int index_percentile_013 = static_cast<int>(PERCENTILE_0135 * N / 100000);
	int index_mediana = static_cast<int>(5 * N / 10);

	double Q_05, Q_99, Q_01;
	Q_05 = distancias->GetValue(index_mediana);
	Q_99 = distancias->GetValue(index_percentile_99);
	Q_01 = distancias->GetValue(index_percentile_013);
	double UL = m_Controls.UL->value();
	double LL = -1 * m_Controls.LL->value();


	double Ppk1 = (UL - Q_05) / (Q_99 - Q_05);
	double Ppk2 = (Q_05 - LL) / (Q_05 - Q_01);

	Ppk = Ppk1;
	if (Ppk > Ppk2)
		Ppk = Ppk2;

	return Ppk;
}

//mitk::DataNode* AwesomeView::encontrarNodo(QRegExp regexp)
//{



//}

void AwesomeView::GetPlanosObjetivo(){

  mitk::TNodePredicateDataType<mitk::Surface>::Pointer predicateAllSurfaces =
    mitk::TNodePredicateDataType<mitk::Surface>::New();
  mitk::DataStorage::SetOfObjects::ConstPointer so = GetDataStorage()->GetSubset(predicateAllSurfaces);
  int plane_number = 0;
  for (mitk::DataStorage::SetOfObjects::ConstIterator it = so->Begin(); it != so->End(); ++it)
  {

      mitk::DataNode *current_node = it->Value();

      QString current_name = QString::fromStdString(current_node->GetName());

      if (current_name.contains("Plano objetivo"))
      {

          vtkSmartPointer <vtkPolyData> ps = vtkSmartPointer <vtkPolyData> ::New();
          ps = dynamic_cast<mitk::Surface*>(current_node->GetData())->GetVtkPolyData();

            double normal[3] = { 0 };
            double normal2[3] = { 0 };


            double* p1 = ps->GetPoint(7);

            double p_1[3];
            p_1[0] = p1[0];
            p_1[1] = p1[1];
            p_1[2] = p1[2];

            double* p2 = ps->GetPoint(1);

            double p_2[3];
            p_2[0] = p2[0];
            p_2[1] = p2[1];
            p_2[2] = p2[2];

            double* p3 = ps->GetPoint(6);

            double p_3[3];
            p_3[0] = p3[0];
            p_3[1] = p3[1];
            p_3[2] = p3[2];

            double* p4 = ps->GetPoint(5);


            double p_4[3];
            p_4[0] = p4[0];
            p_4[1] = p4[1];
            p_4[2] = p4[2];

            double* p5 = ps->GetPoint(3);

            double p_5[3];
            p_5[0] = p5[0];
            p_5[1] = p5[1];
            p_5[2] = p5[2];

            double* p6 = ps->GetPoint(4);

            double p_6[3];
            p_6[0] = p6[0];
            p_6[1] = p6[1];
            p_6[2] = p6[2];

            vtkTriangle::ComputeNormal(p_1, p_2, p_3, normal);
            vtkTriangle::ComputeNormal(p_4, p_5, p_6, normal2);

			vtkSmartPointer<vtkPlaneSource> selected_plane = vtkSmartPointer<vtkPlaneSource>::New();
			mitk::DataNode* PQ_node = this->GetDataStorage()->GetNamedNode("PQ"); //-->REEMPLAZAR POR EXPRESION REGULAR


			if (PQ_node)
			{

				vtkSmartPointer <vtkPolyData> PQ = vtkSmartPointer <vtkPolyData> ::New();
				PQ = dynamic_cast<mitk::Surface*>(PQ_node->GetData())->GetVtkPolyData();

				//Necesito saber si las opciones de plano CORTAN a la pq o si estan por afuera
				char flag_plano_corta = 0;
				char flag_plano2_corta = 0;
				double curr_signo;
				double prev_signo = 0;

				double curr_signo2;
				double prev_signo2 = 0;

				for (vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
				{

					double p0[3];
					PQ->GetPoint(i, p0);

					double Distance = vtkPlane::Evaluate(normal, p_1, p0);
					double Distance2 = vtkPlane::Evaluate(normal2, p_4, p0);


					if (Distance > 0)
						curr_signo = 1;
					else
						curr_signo = -1;

					if (prev_signo / curr_signo < 0)

					{

						flag_plano_corta = 1;


					}
					prev_signo = curr_signo;

					if (Distance2 > 0)
						curr_signo2 = 1;
					else
						curr_signo2 = -1;

					if (prev_signo2 / curr_signo2 < 0)

					{
						//Hubo cambio de signo
						flag_plano2_corta = 1;


					}
					prev_signo2 = curr_signo2;

				}


				vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter = vtkSmartPointer<vtkCenterOfMass>::New();

				centerOfMassFilter->SetInputData(PQ);

				centerOfMassFilter->SetUseScalarsAsWeights(false);
				centerOfMassFilter->Update();

				double center[3];
				centerOfMassFilter->GetCenter(center);

				double distance_to_center = vtkPlane::Evaluate(normal, p_1, center);
				double distance_to_center2 = vtkPlane::Evaluate(normal2, p_4, center);

				double temp[3];
				//Verificamos que la normal apunte siempre hacia afuera del centro de la PQ, para que quede bien ajustada luego la escala colorimétrica
				if (distance_to_center > 0) //La normal apunta hacia el centro
				{
					normal[0] = -1.0*normal[0];
					normal[1] = -1.0*normal[1];
					normal[2] = -1.0*normal[2];

					temp[0] = p_2[0];
					temp[1] = p_2[1];
					temp[2] = p_2[2];

					p_2[0] = p_3[0];
					p_2[1] = p_3[1];
					p_2[2] = p_3[2];

					p_3[0] = temp[0];
					p_3[1] = temp[1];
					p_3[2] = temp[2];


				}
				if (distance_to_center2 > 0) //La normal apunta hacia el centro
				{
					normal2[0] = -1.0*normal2[0];
					normal2[1] = -1.0*normal2[1];
					normal2[2] = -1.0*normal2[2];

					temp[0] = p_5[0];
					temp[1] = p_5[1];
					temp[2] = p_5[2];

					p_5[0] = p_6[0];
					p_5[1] = p_6[1];
					p_5[2] = p_6[2];

					p_6[0] = temp[0];
					p_6[1] = temp[1];
					p_6[2] = temp[2];
				}



				//Regla de decision para seleccionar plano




				if (flag_plano_corta == 0 && flag_plano2_corta == 0)
				{
					//NINGUN PLANO CORTA LA PQ
					//ELIJO AL QUE ESTE MAS LEJOS-->PEOR CASO

					if (distance_to_center >= distance_to_center2)
					{
						selected_plane->SetNormal(normal);
						selected_plane->SetPoint1(p_2);
						selected_plane->SetPoint2(p_3);
						selected_plane->SetOrigin(p_1);
						selected_plane->Update();


					}
					else
					{

						selected_plane->SetNormal(normal2);
						selected_plane->SetPoint1(p_5);
						selected_plane->SetPoint2(p_6);
						selected_plane->SetOrigin(p_4);
						selected_plane->Update();
					}
				}
				else
				{
					if (flag_plano2_corta == 1 && flag_plano_corta == 1)
					{
						//Los dos planos cortan la pieza quirurgica. me quedo con el que este mas CERCA del centro -->PEOR CASO
						if (distance_to_center < distance_to_center2)
						{
							selected_plane->SetNormal(normal);
							selected_plane->SetPoint1(p_2);
							selected_plane->SetPoint2(p_3);
							selected_plane->SetOrigin(p_1);
							selected_plane->Update();


						}
						else
						{

							selected_plane->SetNormal(normal2);
							selected_plane->SetPoint1(p_5);
							selected_plane->SetPoint2(p_6);
							selected_plane->SetOrigin(p_4);
							selected_plane->Update();
						}

					}
					else
					{
						if (flag_plano2_corta < flag_plano_corta)
							//SOLO EL PLANO2 no CORTA LA PQ-->ELIJO EL QUE NO CORTA.
						{
							selected_plane->SetNormal(normal2);
							selected_plane->SetPoint1(p_5);
							selected_plane->SetPoint2(p_6);
							selected_plane->SetOrigin(p_4);
							selected_plane->Update();
						}
						else
						{ //solo el plano1 no corta la pq, me quedo con ese
							selected_plane->SetNormal(normal);
							selected_plane->SetPoint1(p_2);
							selected_plane->SetPoint2(p_3);
							selected_plane->SetOrigin(p_1);
							selected_plane->Update();

						}
					}
				}


				selected_plane->SetResolution(10, 10);
				selected_plane->Update();
			}
			else
			{
				selected_plane->SetNormal(normal);
				selected_plane->SetPoint1(p_2);
				selected_plane->SetPoint2(p_3);
				selected_plane->SetOrigin(p_1);
				selected_plane->SetResolution(10, 10);
				selected_plane->Update();
			}
          /*mitk::DataNode::Pointer planoobj_node = mitk::DataNode::New();
          mitk::Surface::Pointer surf = mitk::Surface::New();
          surf->SetVtkPolyData(selected_plane->GetOutput());
          planoobj_node->SetData(surf);
          planoobj_node->SetName("Plano objetivo seleccionado");
          GetDataStorage()->Add(planoobj_node);*/

          planoOBJ.push_back(vtkSmartPointer<vtkPlaneSource>::New());
		  planoIO.push_back(new vtkRANSACPlane);
		  planoPQ.push_back(new vtkRANSACPlane);
          planoOBJ[plane_number] = selected_plane;
         
          m_Controls.cbPlaneNumber->addItem(QString::number(plane_number));
          string name = "Plano objetivo " + to_string(plane_number);
          current_node->SetName(name);
          current_node->Update();

          plane_number++;
      }
    }
    N_casos=plane_number;

}
int AwesomeView::RestringirAlHueso(int PQoIo)
{
  if(PQoIo==0)
  {
    string name = "Plano ajustado según Pieza Quirúrgica " + to_string(m_Controls.cbPlaneNumber->currentIndex());
    mitk::DataNode::Pointer node_plano_ajustado_PQ = this->GetDataStorage()->GetNamedNode(name);
    if(!node_plano_ajustado_PQ)
    {
      QMessageBox::warning(NULL, "Falta plano ajustado.", "Por favor verifique haber ajustado plano el método de Pieza quirúrgica.");
      return 1;
    }
    vtkSmartPointer<vtkPolyData> boundedPlane_PQ = vtkSmartPointer<vtkPolyData>::New();
    boundedPlane_PQ = RestringirPlanoAlHueso(planoPQ[m_Controls.cbPlaneNumber->currentIndex()],planoOBJ[m_Controls.cbPlaneNumber->currentIndex()]);

    mitk::Surface::Pointer surface_plano_ajustado_PQ = mitk::Surface::New();
    surface_plano_ajustado_PQ->SetVtkPolyData(boundedPlane_PQ);

    node_plano_ajustado_PQ->SetData(surface_plano_ajustado_PQ);
    node_plano_ajustado_PQ->Update();
  }

  if(PQoIo==1)
  {
    string name= "Plano ajustado según Puntos Intraoperatorios " + to_string(m_Controls.cbPlaneNumber->currentIndex());
    mitk::DataNode::Pointer node_plano_ajustado_IO = this->GetDataStorage()->GetNamedNode(name);
    if(!node_plano_ajustado_IO)
    {
      QMessageBox::warning(NULL, "Falta plano ajustado.", "Por favor verifique haber ajustado plano para el método de Puntos Intraoperatorios.");
      return 1;
    }
    vtkSmartPointer<vtkPolyData> boundedPlane_IO = vtkSmartPointer<vtkPolyData>::New();
    boundedPlane_IO = RestringirPlanoAlHueso(planoIO[m_Controls.cbPlaneNumber->currentIndex()], planoOBJ[m_Controls.cbPlaneNumber->currentIndex()]);

    mitk::Surface::Pointer surface_plano_ajustado_IO = mitk::Surface::New();
    surface_plano_ajustado_IO->SetVtkPolyData(boundedPlane_IO);

    node_plano_ajustado_IO->SetData(surface_plano_ajustado_IO);
    node_plano_ajustado_IO->Update();
  }
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  return 0;
}

void AwesomeView::onRestringirAlHueso()
{
	int ok;
	ok = 0;
  ok=RestringirAlHueso(0); //PQ
  ok=RestringirAlHueso(1); //IO
  if (ok == 0)
  {
	  m_Controls.fraResultados->setEnabled(true);
	  m_Controls.titResultados->setEnabled(true);
	  m_Controls.titResultados->setVisible(true);
	  m_Controls.pbTotal->setEnabled(true);
	  m_Controls.chbColorimetria->setEnabled(true);
	  m_Controls.chbColorimetria->setVisible(true);
  }
}
double AwesomeView::CalcularIndicadorISO(vtkPolyData *plano_ejecutado, vtkPlane *plano_objetivo){

  vtkSmartPointer<vtkPlane> planoOBJ_vtkplane= vtkSmartPointer<vtkPlane>::New();
  planoOBJ_vtkplane->SetOrigin(planoOBJ[m_Controls.cbPlaneNumber->currentIndex()]->GetOrigin());
  planoOBJ_vtkplane->SetNormal(planoOBJ[m_Controls.cbPlaneNumber->currentIndex()]->GetNormal());

  vtkSmartPointer<vtkDoubleArray> distancias = vtkSmartPointer <vtkDoubleArray>::New();
  distancias = CalcularDistancias(plano_ejecutado,plano_objetivo);



  double L = 0;

  for (vtkIdType i=0; i<distancias->GetNumberOfValues(); i++)
  {
    double d=distancias->GetValue(i);
    if (abs(d)>abs(L))
    {
      L=distancias->GetValue(i);

    }

  }



  return L;


}
vtkSmartPointer<vtkPolyData> AwesomeView::RestringirPlanoAlHueso(vtkRANSACPlane *plano_ejecutado, vtkPlaneSource *plano_objetivo) {
	double *n_ransac;
	n_ransac = plano_ejecutado->normal;

	double *o_ransac;
	o_ransac = plano_ejecutado->origin;


	double projOrigin[3];
	vtkPlane::ProjectPoint(plano_objetivo->GetOrigin(), o_ransac, n_ransac, projOrigin);

	double projPoint1[3];
	vtkPlane::ProjectPoint(plano_objetivo->GetPoint1(), o_ransac, n_ransac, projPoint1);

	double projPoint2[3];
	vtkPlane::ProjectPoint(plano_objetivo->GetPoint2(), o_ransac, n_ransac, projPoint2);

	//----------Creo un nuevo plano cuyos puntos son las proyecciones del plano objetivo sobre el plano ajustado--------------------------------//
	// es como que recorto el plano ajustado con la sombra que el plano objetivo hace sobre él. y le agrego cantidad de puntos: aumento la resolucion
	vtkSmartPointer<vtkPlaneSource> ejecutado_recortado = vtkSmartPointer<vtkPlaneSource>::New();
	ejecutado_recortado->SetPoint1(projPoint1);
	ejecutado_recortado->SetOrigin(projOrigin);
	ejecutado_recortado->SetPoint2(projPoint2);
	ejecutado_recortado->SetXResolution(100);
	ejecutado_recortado->SetYResolution(100);
	ejecutado_recortado->Update();



	//--------------------Uso el hueso original para tomar solo los puntos que estan dentro de ese contorno-------------//
	mitk::DataNode* boneNode = GetDataStorage()->GetNamedNode("Hueso");
	if (!boneNode)
	{
		QMessageBox::warning(NULL, "Cargue la superficie del hueso", "No se encontró nodo llamado 'Hueso'.");
		return 0;
	}

	vtkPolyData* bone = dynamic_cast<mitk::Surface*>(boneNode->GetData())->GetVtkPolyData();


	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
	selectEnclosedPoints->SetTolerance(0);
	selectEnclosedPoints->CheckSurfaceOn();
	selectEnclosedPoints->SetSurfaceData(bone);
	selectEnclosedPoints->Update();


	selectEnclosedPoints->SetInputData(ejecutado_recortado->GetOutput());
	selectEnclosedPoints->Update();

	vtkSmartPointer<vtkPolyData> boundedPlane = selectEnclosedPoints->GetPolyDataOutput();
  //int n = boundedPlane->GetNumberOfPoints();

	vtkSmartPointer<vtkThresholdPoints> threshold = vtkSmartPointer<vtkThresholdPoints>::New();
	threshold->SetInputData(selectEnclosedPoints->GetOutput());
	threshold->ThresholdByUpper(1);
	threshold->Update();

	vtkSmartPointer<vtkDelaunay2D> delaunay = vtkSmartPointer<vtkDelaunay2D>::New();
	delaunay->SetInputData(threshold->GetOutput());
	delaunay->Update();


	boundedPlane->DeepCopy(delaunay->GetOutput());

	return boundedPlane;

}
vtkSmartPointer<vtkDoubleArray> AwesomeView::CalcularDistancias(vtkPolyData *boundedPlane, vtkPlane *plano_objetivo){

 
 int n = boundedPlane->GetNumberOfPoints(); 
  
  vtkSmartPointer<vtkDoubleArray> distancias = vtkSmartPointer<vtkDoubleArray>::New();
  distancias->SetNumberOfComponents(1);
  distancias->SetNumberOfValues(n);

  for (vtkIdType i = 0; i < boundedPlane->GetNumberOfPoints(); ++i)
  {
    double d = vtkPlane::DistanceToPlane(boundedPlane->GetPoint(i), plano_objetivo->GetNormal(), plano_objetivo->GetOrigin())
              * (vtkPlane::Evaluate(plano_objetivo->GetNormal(), plano_objetivo->GetOrigin(), boundedPlane->GetPoint(i)) >= 0 ? 1. : -1.); //si es mayor que cero multiplico por 1, sino por -1.


    distancias->SetValue(i, d);
    //cout<<"Distancia"<<i<<": "<<d<<endl;


  }

return distancias;
}
/*
void AwesomeView::CalcularInliersPQ_conPlanoObjetivo()
{
	mitk::DataNode* PQ_Node = this->GetDataStorage()->GetNamedNode("PQ");
	if (!PQ_Node)
	{
		QMessageBox::warning(NULL, "No se encontró la pieza quirúrgica.", "Por favor cargue la pieza quirúrgica");
		return;
	}

	vtkPolyData* PQ = dynamic_cast<mitk::Surface*>(PQ_Node->GetData())->GetVtkPolyData();


	mitk::DataNode* planoOBJ_node = this->GetDataStorage()->GetNamedNode("Plano objetivo");

	if (!planoOBJ_node)
	{
		QMessageBox::warning(NULL, "Cargue el plano objetivo", "No se encontró nodo llamado 'Plano objetivo'.");
		return;
	}
	
	vtkSmartPointer <vtkPolyData> ps = vtkSmartPointer <vtkPolyData> ::New();


	ps = dynamic_cast<mitk::Surface*>(planoOBJ_node->GetData())->GetVtkPolyData();

	vtkSmartPointer<vtkPlane> planoOBJ = vtkSmartPointer<vtkPlane>::New();


		double normal[3] = { 0 };

		double* p1 = ps->GetPoint(7);


		double p_1[3];
		p_1[0] = p1[0];
		p_1[1] = p1[1];
		p_1[2] = p1[2];

		double* p2 = ps->GetPoint(1);

		double p_2[3];
		p_2[0] = p2[0];
		p_2[1] = p2[1];
		p_2[2] = p2[2];

		double* p3 = ps->GetPoint(6);

		double p_3[3];
		p_3[0] = p3[0];
		p_3[1] = p3[1];
		p_3[2] = p3[2];



		vtkTriangle::ComputeNormal(p_1, p_2, p_3, normal);

		planoOBJ->SetNormal(normal);

		planoOBJ->SetOrigin(p_1);


	//-------------------------CALCULO THRESHOLD ADAPTADO--------------------------//
	int n = PQ->GetNumberOfPoints();

	double threshold;

	double minD = 85000;
	double maxD = 0;
	for (vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
	{

		double p0[3];
		PQ->GetPoint(i, p0);


		double squaredDistance = planoOBJ->DistanceToPlane(p0);

		if (squaredDistance<minD)
		{

			minD = squaredDistance;
		}
		if (squaredDistance>maxD)
		{

			maxD = squaredDistance;
		}
	}


	int push = 10;
	planoOBJ->Push(push);
	double *D = (double*)malloc(n * sizeof(double));
	double minD2 = 87000;
	double maxD2 = 0;

	for (vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
	{

		double p_0[3];
		PQ->GetPoint(i, p_0);


		double squaredDistance = planoOBJ->DistanceToPlane(p_0);


		D[i] = squaredDistance;

		if (squaredDistance<minD2)
		{

			minD2 = squaredDistance;
		}
		if (squaredDistance>maxD2)
		{

			maxD2 = squaredDistance;
		}
	}


	if (maxD2<maxD)  //El plano se movio hacia ADENTRO DE LA PIEZA QUIRURGICA. Hay que moverlo para el otro lado
	{

		planoOBJ->Push(-2 * push);

		double minD3 = 87000;
		double maxD3 = 0;
		for (vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
		{

			double p[3];
			PQ->GetPoint(i, p);


			double squaredDistance = planoOBJ->DistanceToPlane(p);


			D[i] = squaredDistance;

			if (squaredDistance<minD3)
			{

				minD3 = squaredDistance;
			}
			if (squaredDistance>maxD3)
			{

				maxD3 = squaredDistance;
			}
		}

		minD2 = minD3;
		maxD2 = maxD3;
	}




	threshold = minD2 + maxD2 / 20;


	// ---------- Una vez que tengo el threshold: Identificar puntos de la PQ que cumplen el threshold ---------------//

	//Creo conjunto de puntos donde se cargarán los puntos cercanos al plano objetivo
	vtkSmartPointer<vtkPoints> CutSurfacePoints = vtkSmartPointer<vtkPoints>::New();

	vtkSmartPointer<vtkCellArray> verts = vtkSmartPointer<vtkCellArray>::New();

	for (vtkIdType i = 0; i<n; i++)
	{
		if (D[i]<threshold)
		{

			double p0[3];
			PQ->GetPoint(i, p0);
			CutSurfacePoints->InsertNextPoint(p0);

		}
	}

	verts->InsertNextCell(CutSurfacePoints->GetNumberOfPoints());

	for (vtkIdType k = 0; k<CutSurfacePoints->GetNumberOfPoints(); k++)
	{
		double p1[3];
		CutSurfacePoints->GetPoint(k, p1);
		verts->InsertCellPoint(k);
	}
	vtkSmartPointer<vtkPolyData> polydataInliers = vtkSmartPointer<vtkPolyData>::New();
	polydataInliers->SetPoints(CutSurfacePoints);
	polydataInliers->SetVerts(verts);


	mitk::Surface::Pointer surfaceInliers = mitk::Surface::New();

	surfaceInliers->SetVtkPolyData(polydataInliers);

	mitk::DataNode::Pointer inliers_Node = mitk::DataNode::New();


	inliers_Node->SetData(surfaceInliers);
	inliers_Node->SetName("InliersPQ");

	GetDataStorage()->Add(inliers_Node);


}
*/
vtkSmartPointer<vtkPolyData> AwesomeView::CalcularInliersPQ_sinPointSet()
{
  mitk::DataNode* PQ_Node = this->GetDataStorage()->GetNamedNode("PQ");
  if (!PQ_Node)
  {
      QMessageBox::warning(NULL, "No se encontró la pieza quirúrgica.", "Por favor cargue la pieza quirúrgica");
      return NULL;
  }

  vtkPolyData* PQ = dynamic_cast<mitk::Surface*>(PQ_Node->GetData())->GetVtkPolyData();

  vtkSmartPointer<vtkPlane> plano_para_threshold = vtkSmartPointer<vtkPlane>::New();
  int plane= m_Controls.cbPlaneNumber->currentIndex();
  plano_para_threshold->SetNormal(planoOBJ[plane]->GetNormal());
  plano_para_threshold->SetOrigin(planoOBJ[plane]->GetOrigin());
  int n=PQ->GetNumberOfPoints();

  double minD=85000;
  double maxD=0;
        for(vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
           {

               double p0[3];
               PQ->GetPoint(i,p0);


              double squaredDistance = plano_para_threshold->DistanceToPlane(p0);

              if (squaredDistance<minD)
              {

                  minD=squaredDistance;
              }
              if (squaredDistance>maxD)
              {

                     maxD=squaredDistance;
              }
          }


    int push =10;
    plano_para_threshold->Push(push);
  double *D=(double*)malloc(n*sizeof(double));
    double minD2=87000;
    double maxD2=0;

    for(vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
       {

           double p_0[3];
           PQ->GetPoint(i,p_0);


          double squaredDistance = plano_para_threshold->DistanceToPlane(p_0);


          D[i]=squaredDistance;

          if (squaredDistance<minD2)
          {

              minD2=squaredDistance;
          }
          if (squaredDistance>maxD2)
          {

                 maxD2=squaredDistance;
          }
      }


	if (maxD2 < maxD)  //El plano se movio hacia ADENTRO DE LA PIEZA QUIRURGICA. Hay que moverlo para el otro lado
	{
		plano_para_threshold->Push(-2 * push);
	}
	double *normal = plano_para_threshold->GetNormal();
	double *origin = plano_para_threshold->GetOrigin();
	/*
    vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter =   vtkSmartPointer<vtkCenterOfMass>::New();


    centerOfMassFilter->SetUseScalarsAsWeights(false);
    double center[3];
    double distance_to_center[4];

    centerOfMassFilter->SetInputData(PQ);
    centerOfMassFilter->Update();
    centerOfMassFilter->GetCenter(center);



    //Rota al plano en tres direcciones distintas y se fija como cambia la distancia al centro de masa de la PQ
    distance_to_center[0] = abs(vtkPlane::Evaluate(normal,origin,center));
    
	normal[0]=0.9*normal[0];
    distance_to_center[1]=abs(vtkPlane::Evaluate(normal,origin,center));
    normal[0]=normal[0]/0.9;
    
	normal[1]=0.9*normal[1];
    distance_to_center[2]=abs(vtkPlane::Evaluate(normal,origin,center));
    normal[1]=normal[1]/0.9;
    
	normal[2]=0.9*normal[2];
    distance_to_center[3]=abs(vtkPlane::Evaluate(normal,origin,center));
    normal[2]=normal[2]/0.9;
 


    double curr_distance = distance_to_center[0];
    double prev_distance = distance_to_center[0];


    for (int i=1;i<4;i++)
    {
      cout<<"Primer distancia antes de rotar en "<<i<<": "<<curr_distance<<endl;
	  double curr_distance = distance_to_center[0];
	  double prev_distance = distance_to_center[0];
      if(distance_to_center[i]<distance_to_center[0]) //Achicar la normal en este eje dio buenos resultados (disminuyo la distancia al centro de masa)
      {
        double j1=0;
        while(curr_distance>=prev_distance && j1<50)
        {
          normal[i-1] = 0.85*normal[i-1];
          prev_distance=curr_distance;
          curr_distance=abs(vtkPlane::Evaluate(normal,origin,center));
          cout<<curr_distance<<endl;
          j1++;
          }
        double j2=0;
        while(curr_distance>=prev_distance && j2<50)
        {
          normal[i-1] = 1.02*normal[i-1];
          prev_distance=curr_distance;
          curr_distance=abs(vtkPlane::Evaluate(normal,origin,center));
          cout<<curr_distance<<endl;
          j2++;
        }
		if (prev_distance<curr_distance) //anulo el ultimo
		{
			normal[i - 1] = normal[i - 1] / 1.02;
		}
      }
      else //Hay que agrandar la normal en esta direccion, porque achicarla rotó el plano hacia el otro lado, alejandolo del centro de masa
      {
        double j3=0;
        while(curr_distance>=prev_distance && j3<50)
        {
          normal[i-1] = 1.2*normal[i-1];
          prev_distance=curr_distance;
          curr_distance=abs(vtkPlane::Evaluate(normal,origin,center));
          cout<<curr_distance<<endl;
          j3++;
        }
        double j4=0;
        while(curr_distance>=prev_distance && j4<50)
        {
          normal[i-1] = 0.95*normal[i-1];
          prev_distance=curr_distance;
          curr_distance=abs(vtkPlane::Evaluate(normal,origin,center));
          cout<<curr_distance<<endl;
          j4++;
          }
		if (prev_distance<curr_distance)
		{
			normal[i - 1] = normal[i - 1] / 0.95;
		}

      }
	  
	  vtkSmartPointer<vtkPlaneSource> rotacion = vtkSmartPointer<vtkPlaneSource>::New();
	  double projOrigin[3];
	  vtkPlane::ProjectPoint(origin, origin, normal, projOrigin);

	  double projPoint1[3];
	  vtkPlane::ProjectPoint(planoOBJ[plane]->GetPoint1(), origin, normal, projPoint1);

	  double projPoint2[3];
	  vtkPlane::ProjectPoint(planoOBJ[plane]->GetPoint2(), origin, normal, projPoint2);

	  rotacion->SetOrigin(projOrigin);
	  rotacion->SetPoint1(projPoint1);
	  rotacion->SetPoint2(projPoint2);
	  rotacion->Update();
	  mitk::Surface::Pointer surf = mitk::Surface::New();
	  surf->SetVtkPolyData(rotacion->GetOutput());

	  mitk::DataNode::Pointer nodo = mitk::DataNode::New();
	  nodo->SetData(surf);
	  nodo->SetName("Rotado" + to_string(i));
	  GetDataStorage()->Add(nodo);


    }*/
	double prev_min = 0;

	for (vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
	{

		double p_0[3];
		PQ->GetPoint(i, p_0);


		double squaredDistance = plano_para_threshold->DistanceToPlane(p_0);


		D[i] = squaredDistance;

		if (squaredDistance<prev_min)
		{

			prev_min = squaredDistance;
		}

	}
	
	double curr_min = prev_min;


	for (int i = 0; i<2; i++)
	{
		//busco alcanzar un equilibrio donde la minima distancia sea la maxima posible 
			normal[i] = 0.85*normal[i];
			plano_para_threshold->SetNormal(normal);
			for (vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
			{

				double p_0[3];
				PQ->GetPoint(i, p_0);


				double squaredDistance = plano_para_threshold->DistanceToPlane(p_0);


				D[i] = squaredDistance;

				if (squaredDistance<curr_min)
				{

					curr_min = squaredDistance;
				}

			}

			//La rotacion sirvio para maximizar la minima distancia? curr_min deberia ser mayor que prev_min
			if (prev_min <= curr_min)
			{
				double j1 = 0;
				while (prev_min <= curr_min && j1 < 50) //yo quiero que curr_min sea mas grande 
				{
					prev_min = curr_min;
					normal[i] = 0.95*normal[i];
					plano_para_threshold->SetNormal(normal);
					for (vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
					{

						double p_0[3];
						PQ->GetPoint(i, p_0);


						double squaredDistance = plano_para_threshold->DistanceToPlane(p_0);


						D[i] = squaredDistance;

						if (squaredDistance<curr_min)
						{

							curr_min = squaredDistance;
						}

					}

					cout << "Minimo para rotacion en "<<i<<": "<<curr_min << endl;
					j1++;
				}
			}
			else
			{
				double j2 = 0;
				while (prev_min <= curr_min && j2 < 50) //yo quiero que curr_min sea mas grande 
				{
					prev_min = curr_min;
					normal[i] = 1.05*normal[i];
					plano_para_threshold->SetNormal(normal);
					for (vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
					{

						double p_0[3];
						PQ->GetPoint(i, p_0);


						double squaredDistance = plano_para_threshold->DistanceToPlane(p_0);


						D[i] = squaredDistance;

						if (squaredDistance<curr_min)
						{

							curr_min = squaredDistance;
						}

					}

					cout << "Minimo para rotacion en " << i << ": " << curr_min << endl;
					j2++;
				}
			}

		
		
		vtkSmartPointer<vtkPlaneSource> rotacion = vtkSmartPointer<vtkPlaneSource>::New();
		double projOrigin[3];
		vtkPlane::ProjectPoint(origin, origin, normal, projOrigin);

		double projPoint1[3];
		vtkPlane::ProjectPoint(planoOBJ[plane]->GetPoint1(), origin, normal, projPoint1);

		double projPoint2[3];
		vtkPlane::ProjectPoint(planoOBJ[plane]->GetPoint2(), origin, normal, projPoint2);

		rotacion->SetOrigin(projOrigin);
		rotacion->SetPoint1(projPoint1);
		rotacion->SetPoint2(projPoint2);
		rotacion->Update();
		mitk::Surface::Pointer surf = mitk::Surface::New();
		surf->SetVtkPolyData(rotacion->GetOutput());

		mitk::DataNode::Pointer nodo = mitk::DataNode::New();
		nodo->SetData(surf);
		nodo->SetName("Rotado" + to_string(i));
		GetDataStorage()->Add(nodo);


	}

          plano_para_threshold->SetNormal(normal);
		  double *Dfin = (double*)malloc(n * sizeof(double));
          double minDfin=87000;
          double maxDfin=0;
          for(vtkIdType k = 0; k < PQ->GetNumberOfPoints(); k++)
             {

                 double p[3];
                 PQ->GetPoint(k,p);


				 double squaredDistance = abs(vtkPlane::Evaluate(normal, origin, p));


                Dfin[k]=squaredDistance;

                if (squaredDistance<minDfin)
                {

                    minDfin=squaredDistance;
                }
                if (squaredDistance>maxDfin)
                {

                       maxDfin=squaredDistance;
                }
            }


    double threshold=minDfin+maxDfin/10;
    vtkSmartPointer<vtkPoints> CutSurfacePoints =  vtkSmartPointer<vtkPoints>::New();

    vtkSmartPointer<vtkCellArray> verts =  vtkSmartPointer<vtkCellArray>::New();

    for (vtkIdType i=0; i<n; i++)
     {
        if(Dfin[i]<threshold)
        {

          double p0[3];
          PQ->GetPoint(i,p0);
          CutSurfacePoints->InsertNextPoint(p0);
          //cout<<i<<";"<<D[i]<<";"<<p0[0]<<";"<<p0[1]<<";"<<p0[2]<<endl;
         }
       }


     verts->InsertNextCell(CutSurfacePoints->GetNumberOfPoints());

     for (vtkIdType k=0; k<CutSurfacePoints->GetNumberOfPoints();k++)
     {
        double p1[3];
        CutSurfacePoints->GetPoint(k,p1);
        verts->InsertCellPoint(k);
     }
     vtkSmartPointer<vtkPolyData> polydataInliers =vtkSmartPointer<vtkPolyData>::New();
     polydataInliers->SetPoints(CutSurfacePoints);
     polydataInliers->SetVerts(verts);

return polydataInliers;



}
int AwesomeView::GetColorIndicador(int type, double value)
//Devuelve el indice de semaforo_desempeño que corresponde al valor obtenido.
{
	switch (type)
	{
	case 0:
		//Indicador ISO
		//El valor es malo si es positivo y mayor que UL, o negativo y menor que LL
		if ((value < (-1.0*m_Controls.LL->value())) || (value > m_Controls.UL->value()))
		{
			return 0;
		}
		else if ((value > (-0.5*m_Controls.LL->value())) && (value < 0.5*m_Controls.UL->value()))
		{
			return 2;
		}
		else
			return 1;
		break;
	case 1:
		//Indicador Ppk
		if (value > 1)
			return 2;
		else if (value > 0.5)
			return 1;
		else
			return 0;
		break;
	case 2:
		//Porcentaje en tolerancia
		if (value > 99.7)
			return 2;
		else if (value > 49.7)
			return 1;
		else
			return 0;
		break;
	case 3:
		//Margen oncológico. 
		//Se considera malo si está a 1mm o menos del tumor. 
		if (value > MARG_ONC_IDEAL-1 && value <MARG_ONC_IDEAL + 1)
			return 2;
		else if ((value > MARG_ONC_MINIMO && value <(MARG_ONC_IDEAL - 1)) || (value > (MARG_ONC_IDEAL +1) && value <MARG_ONC_DEMASIADO) )
			return 1;
		else if (value < MARG_ONC_MINIMO || value > MARG_ONC_DEMASIADO)
			return 0;
		else if (value < 0)
		{
      QString error;
      error="No se resecó todo el tumor (Plano " + QString::number(m_Controls.cbPlaneNumber->currentIndex());
      QMessageBox::warning(NULL, "Margen oncológico crítico", error);
			return 0;
		}
		break;
	case 4:
		//Indicador distancia centroide 
		if (value < 2*UNCERTAINTY)
			return 2;
		else if (value < 4 * UNCERTAINTY)
			return 1;
		else
			return 0;
		break;
	case 5:
		//Indicador angulo
		if (value < ANGULAR_UNCERTAINTY)
			return 2;
		else if (value < 3*ANGULAR_UNCERTAINTY)
			return 1;
		else
			return 0;
		break;
	}
  return 3;
}

int AwesomeView::DefinirTotalPQ(QTableWidget *tableWidget)
{

	QFont fnt;
	fnt.setPointSize(8);
	fnt.setBold(true);

	int cantIndicadores = 4;
	resultsWidget->setMaximumSize(1300, 800);
	pbExport->setGeometry(300, 500, 200, 40);
	tableWidget->setRowCount(cantIndicadores + 2);
	tableWidget->setColumnCount(N_casos + 2);
	tableWidget->setGeometry(100, 100, 1000, 600);
	tableWidget->setColumnWidth(0, 250);
	tableWidget->setColumnWidth(N_casos+1,150);
	
	QTableWidgetItem *plano = new QTableWidgetItem("Planos");
	plano->setBackground(QColor::fromRgb(0, 0, 0));
	plano->setForeground(QColor::fromRgb(255, 255, 255));
	tableWidget->setItem(0, 0, plano);

	QTableWidgetItem *tit_cirugia = new QTableWidgetItem("Total Cirugía");
	tit_cirugia->setBackground(QColor::fromRgb(0, 0, 0));
	tit_cirugia->setForeground(QColor::fromRgb(255, 255, 255));
	tit_cirugia->setFont(fnt);
	tit_cirugia->setTextAlignment(Qt::AlignCenter);
	//tit_cirugia->setTextAlignment(Qt::AlignHCenter);
	tableWidget->setItem(0, N_casos + 1, tit_cirugia);

	QTableWidgetItem *dist = new QTableWidgetItem("Distancia máx (mm)");
	dist->setForeground(QColor::fromRgb(255, 255, 255));
	dist->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(1, 0, dist);

	QTableWidgetItem *ppk = new QTableWidgetItem("Ppk");
	ppk->setForeground(QColor::fromRgb(255, 255, 255));
	ppk->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(2, 0, ppk);


	QTableWidgetItem *area = new QTableWidgetItem("% Área en tolerancia");
	area->setForeground(QColor::fromRgb(255, 255, 255));
	area->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(3, 0, area);

	QTableWidgetItem *mo = new QTableWidgetItem("Margen oncológico (mm)");
	mo->setForeground(QColor::fromRgb(255, 255, 255));
	mo->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(4, 0, mo);

	QTableWidgetItem *tot = new QTableWidgetItem("Total plano");
	tot->setForeground(QColor::fromRgb(255, 255, 255));
	tot->setBackground(QColor::fromRgb(0, 0, 0));
	tot->setFont(fnt);
	tableWidget->setItem(5, 0, tot);
	
	QString textValuePQ;
	double totCirugia_PQ = 0;

	for (int j = 1; j < 1 + N_casos; j++) //Recorro los planos
	{

		QTableWidgetItem *num = new QTableWidgetItem(QString::number(j - 1));
		num->setForeground(QColor::fromRgb(0, 0, 0));
		num->setTextAlignment(Qt::AlignCenter);
		//num->setTextAlignment(Qt::AlignHCenter);
		switch (j-1)
		{
		case 0:
			num->setBackground(QColor::fromRgb(R_PO0, G_PO0, B_PO0));
			break;
		case 1:
			num->setBackground(QColor::fromRgb(R_PO1, G_PO1, B_PO1));
			break;
		case 2:
			num->setBackground(QColor::fromRgb(0, 102, 255));
			break;
		case 3:
			num->setBackground(QColor::fromRgb(R_PO3, G_PO3, B_PO3));
			break;
		}

		
		tableWidget->setItem(0, j, num);
		m_Controls.cbPlaneNumber->setCurrentIndex(j - 1);
		string name;
		mitk::DataNode::Pointer nodo_j = mitk::DataNode::New();



		name = "InliersPQ " + std::to_string(j - 1);
		nodo_j = GetDataStorage()->GetNamedNode(name);
		if (!nodo_j)
		{
			int error;
			error = CalcularInliersPQ(j - 1);
			if (error == 0)
				return 0;
		}
		name = "Plano ajustado según Pieza Quirúrgica " + std::to_string(j - 1);
		nodo_j = GetDataStorage()->GetNamedNode(name);
		if (!nodo_j)
		{
			AjustarPlanoPQ();
			RestringirAlHueso(0);
		}
		
		double subtot_PQ = 0;
		for (int i = 1; i < cantIndicadores + 1; i++) //Recorro tipo de indicador (del 0 al 4, en la tabla del 1 al 5)
		{
			double *results = new double[2];
			box_Indicadores(i - 1, results);

			subtot_PQ += GetColorIndicador(i - 1, results[0]);


			textValuePQ = QString::number(results[0]);
			QTableWidgetItem *newItemPQ = new QTableWidgetItem(textValuePQ);
			switch (GetColorIndicador(i - 1, results[0]))
			{
			case 0:
				newItemPQ->setForeground(QColor::fromRgb(255, 0, 0));
				newItemPQ->setBackground(QColor::fromRgb(220, 223, 224));
				break;
			case 1:
				newItemPQ->setForeground(QColor::fromRgb(255, 255, 51));
				newItemPQ->setBackground(QColor::fromRgb(220, 223, 224));
				break;
			case 2:
				newItemPQ->setForeground(QColor::fromRgb(14, 162, 42));
				newItemPQ->setBackground(QColor::fromRgb(220, 223, 224));
				break;
			}
			tableWidget->setItem(i, j, newItemPQ);


		}
		totCirugia_PQ += subtot_PQ;

		subtot_PQ = subtot_PQ / (1.0*cantIndicadores);
		subtot_PQ = 100 * subtot_PQ / 2;
		

		QTableWidgetItem *subtot = new QTableWidgetItem(QString::number(subtot_PQ) + "%");

		switch (j - 1)
		{
		case 0:
			subtot->setBackground(QColor::fromRgb(R_PO0, G_PO0, B_PO0));
			break;
		case 1:
			subtot->setBackground(QColor::fromRgb(R_PO1, G_PO1, B_PO1));
			break;
		case 2:
			subtot->setBackground(QColor::fromRgb(0, 102, 255));
			break;
		case 3:
			subtot->setBackground(QColor::fromRgb(R_PO3, G_PO3, B_PO3));
			break;
		}
		fnt.setPointSize(10);
		subtot->setForeground(QColor::fromRgb(0, 0, 0));
		subtot->setFont(fnt);
		subtot->setTextAlignment(Qt::AlignCenter);
		//subtot->setTextAlignment(Qt::AlignHCenter);
		tableWidget->setItem(cantIndicadores + 1, j, subtot);

	}
	totCirugia_PQ = totCirugia_PQ / (N_casos*cantIndicadores);
	totCirugia_PQ = 100 * totCirugia_PQ / 2 ;

	fnt.setPointSize(15);
	fnt.setBold(true);
	

	QTableWidgetItem *tot_cirugia = new QTableWidgetItem(QString::number(totCirugia_PQ) + "%");
	tot_cirugia->setBackground(QColor::fromRgb(200, 200, 200));
	tot_cirugia->setForeground(QColor::fromRgb(0, 0, 0));
	tot_cirugia->setTextAlignment(Qt::AlignCenter);
	//tot_cirugia->setTextAlignment(Qt::AlignHCenter);
	tot_cirugia->setFont(fnt);
	tableWidget->setItem(1, N_casos + 1, tot_cirugia);

	tableWidget->setSpan(1, N_casos + 1, cantIndicadores + 1, 1);
	return 1;
}
int AwesomeView::DefinirTotalIO(QTableWidget *tableWidget)
{
	QFont fnt;
	fnt.setPointSize(8);
	fnt.setBold(true);
	int cantIndicadores = 4;
	resultsWidget->setMaximumSize(1300, 800);
	pbExport->setGeometry(300, 500, 200, 40);
	tableWidget->setRowCount(cantIndicadores + 2);
	tableWidget->setColumnCount(N_casos + 2);
	tableWidget->setGeometry(100, 100, 1000, 600);
	tableWidget->setColumnWidth(0, 250);
	tableWidget->setColumnWidth(N_casos+1,150);
	QTableWidgetItem *plano = new QTableWidgetItem("Planos");
	plano->setBackground(QColor::fromRgb(0, 0, 0));
	plano->setForeground(QColor::fromRgb(255, 255, 255));
	tableWidget->setItem(0, 0, plano);

	QTableWidgetItem *tit_cirugia = new QTableWidgetItem("Total Cirugía");
	tit_cirugia->setBackground(QColor::fromRgb(0, 0, 0));
	tit_cirugia->setForeground(QColor::fromRgb(255, 255, 255));
	tit_cirugia->setFont(fnt);
	tit_cirugia->setTextAlignment(Qt::AlignCenter);
	//tit_cirugia->setTextAlignment(Qt::AlignHCenter);

	tableWidget->setItem(0, N_casos+1, tit_cirugia);


	QTableWidgetItem *dist = new QTableWidgetItem("Distancia máx (mm)");
	dist->setForeground(QColor::fromRgb(255, 255, 255));
	dist->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(1, 0, dist);

	QTableWidgetItem *ppk = new QTableWidgetItem("Ppk");
	ppk->setForeground(QColor::fromRgb(255, 255, 255));
	ppk->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(2, 0, ppk);


	QTableWidgetItem *area = new QTableWidgetItem("% Área en tolerancia");
	area->setForeground(QColor::fromRgb(255, 255, 255));
	area->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(3, 0, area);

	QTableWidgetItem *mo = new QTableWidgetItem("Margen oncológico (mm)");
	mo->setForeground(QColor::fromRgb(255, 255, 255));
	mo->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(4, 0, mo);

	QTableWidgetItem *tot = new QTableWidgetItem("Total plano");
	tot->setForeground(QColor::fromRgb(255, 255, 255));
	tot->setBackground(QColor::fromRgb(0, 0, 0));
	tot->setFont(fnt);
	tableWidget->setItem(5, 0, tot);

	QString textValueIO;
	double totCirugia_IO = 0;

	for (int j = 1; j < 1 + N_casos; j++) //Recorro los planos
	{


		QTableWidgetItem *num = new QTableWidgetItem(QString::number(j - 1));
		num->setForeground(QColor::fromRgb(0, 0, 0));
		num->setTextAlignment(Qt::AlignCenter);
		//num->setTextAlignment(Qt::AlignHCenter);
		switch (j - 1)
		{
		case 0:
			num->setBackground(QColor::fromRgb(R_PO0, G_PO0, B_PO0));
			break;
		case 1:
			num->setBackground(QColor::fromRgb(R_PO1, G_PO1, B_PO1));
			break;
		case 2:
			num->setBackground(QColor::fromRgb(0, 102, 255));
			break;
		case 3:
			num->setBackground(QColor::fromRgb(R_PO3, G_PO3, B_PO3));
			break;
		}
		tableWidget->setItem(0, j, num);
		m_Controls.cbPlaneNumber->setCurrentIndex(j - 1);
		string name;
		mitk::DataNode::Pointer nodo_j = mitk::DataNode::New();

		name = "Plano ajustado según Puntos Intraoperatorios " + std::to_string(j - 1);
		nodo_j = GetDataStorage()->GetNamedNode(name);
		if (!nodo_j)
		{
			AjustarPlanoIO();
			RestringirAlHueso(1);
		}

		double subtot_IO = 0;
		for (int i = 1; i < cantIndicadores + 1; i++) //Recorro tipo de indicador (del 0 al 4, en la tabla del 1 al 5)
		{
			double *results = new double[2];
			box_Indicadores(i - 1, results);

			subtot_IO += GetColorIndicador(i - 1, results[1]);


			textValueIO = QString::number(results[1]);
			QTableWidgetItem *newItemIO = new QTableWidgetItem(textValueIO);
			switch (GetColorIndicador(i - 1, results[1]))
			{
			case 0:
				newItemIO->setForeground(QColor::fromRgb(255, 0, 0));
				newItemIO->setBackground(QColor::fromRgb(192, 192, 192));
				break;
			case 1:
				newItemIO->setForeground(QColor::fromRgb(255, 255, 51));
				newItemIO->setBackground(QColor::fromRgb(192, 192, 192));
				break;
			case 2:
				newItemIO->setForeground(QColor::fromRgb(14, 162, 42));
				newItemIO->setBackground(QColor::fromRgb(192, 192, 192));
				break;
			}
			tableWidget->setItem(i, j, newItemIO);


		}
		totCirugia_IO += subtot_IO;

		subtot_IO = subtot_IO / (1.0*cantIndicadores);
		subtot_IO = 100 * subtot_IO / 2 ;


		QTableWidgetItem *subtot = new QTableWidgetItem(QString::number(subtot_IO) + "%");

		switch (j - 1)
		{
		case 0:
			subtot->setBackground(QColor::fromRgb(R_PO0, G_PO0, B_PO0));
			break;
		case 1:
			subtot->setBackground(QColor::fromRgb(R_PO1, G_PO1, B_PO1));
			break;
		case 2:
			subtot->setBackground(QColor::fromRgb(0, 102, 255));
			break;
		case 3:
			subtot->setBackground(QColor::fromRgb(R_PO3, G_PO3, B_PO3));
			break;
		}
		fnt.setPointSize(10);
		subtot->setForeground(QColor::fromRgb(0,0,0));
		subtot->setTextAlignment(Qt::AlignCenter);
		//subtot->setTextAlignment(Qt::AlignHCenter);
		subtot->setFont(fnt);
		tableWidget->setItem(cantIndicadores + 1, j, subtot);
	}
	totCirugia_IO = totCirugia_IO / (N_casos*cantIndicadores);
	totCirugia_IO = 100 * totCirugia_IO / 2 ;



	fnt.setPointSize(15);
	fnt.setBold(true);


	QTableWidgetItem *tot_cirugia = new QTableWidgetItem(QString::number(totCirugia_IO) + "%");
	tot_cirugia->setBackground(QColor::fromRgb(200, 200, 200));
	tot_cirugia->setForeground(QColor::fromRgb(0, 0, 0));
	tot_cirugia->setTextAlignment(Qt::AlignCenter);
	//tot_cirugia->setTextAlignment(Qt::AlignHCenter);
	tot_cirugia->setFont(fnt);
	tableWidget->setItem(1, N_casos + 1, tot_cirugia);
	tableWidget->setSpan(1, N_casos + 1, cantIndicadores + 1, 1);

	return 1;
}

int AwesomeView::DefinirComparacion(QTableWidget *tableWidget)
{

	QFont fnt;
	fnt.setPointSize(8);
	fnt.setBold(true);

	int cantIndicadores = 4;
	resultsWidget->setMinimumSize(1200, 800);
	tableWidget->setRowCount((cantIndicadores + 1) * 2 + 1);
	tableWidget->setColumnCount(N_casos + 3);
	tableWidget->setGeometry(100, 100, 1200, 600);
	tableWidget->setColumnWidth(1, 250);
	tableWidget->setColumnWidth(N_casos+2,150);
	QTableWidgetItem *plano = new QTableWidgetItem("Planos");
	plano->setBackground(QColor::fromRgb(0, 0, 0));
	plano->setForeground(QColor::fromRgb(255, 255, 255));
	tableWidget->setItem(0, 0, plano);

	tableWidget->setSpan(0, 0, 1, 2);

	QTableWidgetItem *pq = new QTableWidgetItem("Pieza Quirúrgica");
	pq->setBackground(QColor::fromRgb(0, 0, 0));
	pq->setForeground(QColor::fromRgb(255, 255, 255));
	pq->setFont(fnt);
	//pq->setTextAlignment(Qt::AlignHCenter);
	tableWidget->setItem(1, 0, pq);
	tableWidget->setSpan(1, 0, 5, 1);

	QTableWidgetItem *io = new QTableWidgetItem("Puntos Intraoperatorios");
	io->setBackground(QColor::fromRgb(0, 0, 0));
	io->setForeground(QColor::fromRgb(255, 255, 255));
	io->setFont(fnt);
	//io->setTextAlignment(Qt::AlignHCenter);
	tableWidget->setItem(2 + cantIndicadores, 0, io);
	tableWidget->setSpan(2 + cantIndicadores, 0, 5, 1);

	QTableWidgetItem *tit_cirugia = new QTableWidgetItem("Total Cirugía");
	tit_cirugia->setBackground(QColor::fromRgb(0, 0, 0));
	tit_cirugia->setForeground(QColor::fromRgb(255, 255, 255));
	tit_cirugia->setFont(fnt);
	//tit_cirugia->setTextAlignment(Qt::AlignHCenter);
	tit_cirugia->setTextAlignment(Qt::AlignCenter);
	tableWidget->setItem(0, 6, tit_cirugia);

	QTableWidgetItem *dist = new QTableWidgetItem("Distancia máx (mm)");
	dist->setForeground(QColor::fromRgb(255, 255, 255));
	dist->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(1, 1, dist);

	QTableWidgetItem *ppk = new QTableWidgetItem("Ppk");
	ppk->setForeground(QColor::fromRgb(255, 255, 255));
	ppk->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(2, 1, ppk);


	QTableWidgetItem *area = new QTableWidgetItem("% Área en tolerancia");
	area->setForeground(QColor::fromRgb(255, 255, 255));
	area->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(3, 1, area);

	QTableWidgetItem *mo = new QTableWidgetItem("Margen oncológico (mm)");
	mo->setForeground(QColor::fromRgb(255, 255, 255));
	mo->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(4, 1, mo);

	QTableWidgetItem *tot = new QTableWidgetItem("Total plano");
	tot->setForeground(QColor::fromRgb(255, 255, 255));
	tot->setBackground(QColor::fromRgb(0, 0, 0));
	tot->setFont(fnt);
	tableWidget->setItem(5, 1, tot);


	QTableWidgetItem *dist2 = new QTableWidgetItem("Distancia máx (mm)");
	dist2->setForeground(QColor::fromRgb(255, 255, 255));
	dist2->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(6, 1, dist2);

	QTableWidgetItem *ppk2 = new QTableWidgetItem("Ppk");
	ppk2->setForeground(QColor::fromRgb(255, 255, 255));
	ppk2->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(7, 1, ppk2);


	QTableWidgetItem *area2 = new QTableWidgetItem("% Área en tolerancia");
	area2->setForeground(QColor::fromRgb(255, 255, 255));
	area2->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(8, 1, area2);

	QTableWidgetItem *mo2 = new QTableWidgetItem("Margen oncológico (mm)");
	mo2->setForeground(QColor::fromRgb(255, 255, 255));
	mo2->setBackground(QColor::fromRgb(0, 0, 0));
	tableWidget->setItem(9, 1, mo2);

	QTableWidgetItem *tot2 = new QTableWidgetItem("Total plano");
	tot2->setForeground(QColor::fromRgb(255, 255, 255));
	tot2->setBackground(QColor::fromRgb(0, 0, 0));
	tot2->setFont(fnt);
	tableWidget->setItem(10, 1, tot2);




	QString textValuePQ;
	QString textValueIO;
	double totCirugia_PQ = 0;
	double totCirugia_IO = 0;
	for (int j = 2; j < 2 + N_casos; j++) //Recorro los planos
	{

		QTableWidgetItem *num = new QTableWidgetItem(QString::number(j - 2));
		num->setForeground(QColor::fromRgb(0, 0, 0));	
		num->setTextAlignment(Qt::AlignCenter);
		//num->setTextAlignment(Qt::AlignHCenter);
		switch (j - 2)
		{
		case 0:
			num->setBackground(QColor::fromRgb(R_PO0, G_PO0, B_PO0));
			break;
		case 1:
			num->setBackground(QColor::fromRgb(R_PO1, G_PO1, B_PO1));
			break;
		case 2:
			num->setBackground(QColor::fromRgb(0, 102, 255));
			break;
		case 3:
			num->setBackground(QColor::fromRgb(R_PO3, G_PO3, B_PO3));
			break;
		}
		tableWidget->setItem(0, j, num);
	

		m_Controls.cbPlaneNumber->setCurrentIndex(j - 2);
		string name;
		mitk::DataNode::Pointer nodo_j = mitk::DataNode::New();


		
			name = "InliersPQ " + std::to_string(j - 2);
			nodo_j = GetDataStorage()->GetNamedNode(name);
			if (!nodo_j)
			{
				int error;
				error=CalcularInliersPQ(j - 2);
				if (error == 0)
				{
					return 0;
				}

			}
			name = "Plano ajustado según Pieza Quirúrgica " + std::to_string(j - 2);
			nodo_j = GetDataStorage()->GetNamedNode(name);
			if (!nodo_j)
			{
				AjustarPlanoPQ();
				RestringirAlHueso(0);
			}
		

		
			name = "Plano ajustado según Puntos Intraoperatorios " + std::to_string(j - 2);
			nodo_j = GetDataStorage()->GetNamedNode(name);
			if (!nodo_j)
			{
				AjustarPlanoIO();
				RestringirAlHueso(1);
			}
		


		double subtot_PQ = 0;
		double subtot_IO = 0;

		for (int i = 1; i < cantIndicadores + 1; i++) //Recorro tipo de indicador (del 0 al 4, en la tabla del 1 al 5)
		{


			double *results = new double[2];
			box_Indicadores(i - 1, results);

			subtot_PQ += GetColorIndicador(i - 1, results[0]);
			subtot_IO += GetColorIndicador(i - 1, results[1]);


			textValuePQ = QString::number(results[0]);

			QTableWidgetItem *newItemPQ = new QTableWidgetItem(textValuePQ);
			switch (GetColorIndicador(i - 1, results[0]))
			{
			case 0:
				newItemPQ->setForeground(QColor::fromRgb(255, 0, 0));
				newItemPQ->setBackground(QColor::fromRgb(192, 192, 192));
				break;
			case 1:
				newItemPQ->setForeground(QColor::fromRgb(255, 255, 51));
				newItemPQ->setBackground(QColor::fromRgb(192, 192, 192));
				break;
			case 2:
				newItemPQ->setForeground(QColor::fromRgb(14, 162, 42));
				newItemPQ->setBackground(QColor::fromRgb(192, 192, 192));
				break;
			}
			tableWidget->setItem(i, j, newItemPQ);


			textValueIO = QString::number(results[1]);

			QTableWidgetItem *newItemIO = new QTableWidgetItem(textValueIO);
			switch (GetColorIndicador(i - 1, results[1]))
			{
			case 0:
				newItemIO->setForeground(QColor::fromRgb(255, 0, 0));
				newItemIO->setBackground(QColor::fromRgb(192, 192, 192));
				break;
			case 1:
				newItemIO->setForeground(QColor::fromRgb(255, 255, 51));
				newItemIO->setBackground(QColor::fromRgb(192, 192, 192));
				break;
			case 2:
				newItemIO->setForeground(QColor::fromRgb(14, 162, 42));
				newItemIO->setBackground(QColor::fromRgb(192, 192, 192));
				break;
			}
			tableWidget->setItem(i + cantIndicadores + 1, j, newItemIO);

		}
		totCirugia_PQ += subtot_PQ;
		totCirugia_IO += subtot_IO;

		subtot_PQ = subtot_PQ / (1.0*cantIndicadores);
		subtot_PQ = 100 * subtot_PQ / 2 ;

		subtot_IO = subtot_IO / (1.0*cantIndicadores);
		subtot_IO = 100 * subtot_IO / 2 ;
		

		
		QTableWidgetItem *subtot = new QTableWidgetItem(QString::number(subtot_PQ) + "%");
		QTableWidgetItem *subtot2 = new QTableWidgetItem(QString::number(subtot_IO) + "%");
		switch (j - 2)
		{
		case 0:
			subtot->setBackground(QColor::fromRgb(R_PO0, G_PO0, B_PO0));
			subtot2->setBackground(QColor::fromRgb(R_PO0, G_PO0, B_PO0));
			break;
		case 1:
			subtot->setBackground(QColor::fromRgb(R_PO1, G_PO1, B_PO1));
			subtot2->setBackground(QColor::fromRgb(R_PO1, G_PO1, B_PO1));
			break;
		case 2:
			subtot->setBackground(QColor::fromRgb(0, 102, 255));
			subtot2->setBackground(QColor::fromRgb(R_PO2, G_PO2, B_PO2));
			break;
		case 3:
			subtot->setBackground(QColor::fromRgb(R_PO3, G_PO3, B_PO3));
			subtot2->setBackground(QColor::fromRgb(R_PO3, G_PO3, B_PO3));
			break;
		}
		
		fnt.setPointSize(10);
		subtot->setForeground(QColor::fromRgb(0, 0, 0));
		subtot->setFont(fnt);
		subtot->setTextAlignment(Qt::AlignCenter);
		//subtot->setTextAlignment(Qt::AlignHCenter);
		tableWidget->setItem(cantIndicadores + 1, j, subtot);

		subtot2->setForeground(QColor::fromRgb(0, 0, 0));
		subtot2->setFont(fnt);
		subtot2->setTextAlignment(Qt::AlignCenter);
		//subtot2->setTextAlignment(Qt::AlignHCenter);
		tableWidget->setItem((cantIndicadores + 1) * 2, j, subtot2);
	}
	totCirugia_PQ = totCirugia_PQ / (N_casos*cantIndicadores);
	totCirugia_PQ = 100 * totCirugia_PQ / 2 ;

	totCirugia_IO = totCirugia_IO / (N_casos*cantIndicadores);
	totCirugia_IO = 100 * totCirugia_IO / 2 ;



	fnt.setPointSize(15);
	fnt.setBold(true);


	QTableWidgetItem *tot_cirugiaio = new QTableWidgetItem(QString::number(totCirugia_IO) + "%");
	tot_cirugiaio->setBackground(QColor::fromRgb(200, 200, 200));
	tot_cirugiaio->setForeground(QColor::fromRgb(0, 0, 0));
	tot_cirugiaio->setTextAlignment(Qt::AlignCenter);
	//tot_cirugiaio->setTextAlignment(Qt::AlignHCenter);
	tot_cirugiaio->setFont(fnt);

	QTableWidgetItem *tot_cirugiapq = new QTableWidgetItem(QString::number(totCirugia_PQ) + "%");
	tot_cirugiapq->setBackground(QColor::fromRgb(200, 200, 200));
	tot_cirugiapq->setForeground(QColor::fromRgb(0, 0, 0));
	tot_cirugiapq->setTextAlignment(Qt::AlignCenter);
	//tot_cirugiapq->setTextAlignment(Qt::AlignHCenter);
	tot_cirugiapq->setFont(fnt);

	tableWidget->setItem(1, N_casos + 2, tot_cirugiapq);
	tableWidget->setSpan(1, N_casos + 2, cantIndicadores + 1, 1);
	tableWidget->setItem(cantIndicadores + 2, N_casos + 2, tot_cirugiaio);
	tableWidget->setSpan(2 + cantIndicadores, N_casos + 2, cantIndicadores + 1, 1);

	return 1;
}
void AwesomeView::onDefinirTotal() 
{
	m_Controls.fraResultados->setVisible(true);
	m_Controls.fraResultados->setEnabled(true);
  m_Controls.titResultados->setEnabled(true);
  m_Controls.titResultados->setVisible(true);
	m_Controls.chbColorimetria->setVisible(true);
	m_Controls.chbColorimetria->setEnabled(true);

	QString tit;
	int modo = m_Controls.cbModo->currentIndex();
	int errores=1;
	if (modo == 0)
	{
		errores = DefinirTotalPQ(tableWidget);
		tit = "Resultados con tolerancia de " + QString::number(m_Controls.UL->value()) + "mm y -" + QString::number(m_Controls.LL->value()) + "mm. \n Método por Pieza quirúrgica";
	}
	if (modo == 1)
	{
		errores=DefinirTotalIO(tableWidget);
		tit = "Resultados con tolerancia de " + QString::number(m_Controls.UL->value()) + "mm y -" + QString::number(m_Controls.LL->value()) + "mm. \n Método por Puntos Intraoperatorios";
	}		
	if (modo == 2)
	{
		errores=DefinirComparacion(tableWidget);
		tit = "Resultados con tolerancia de " + QString::number(m_Controls.UL->value()) + "mm y -" + QString::number(m_Controls.LL->value()) + "mm";

	}
		
	titulo->setText(tit);
	if (errores == 1)
	{
		resultsWidget->show();
	}
}
void AwesomeView::onExportarDatos()
{
	int modo = m_Controls.cbModo->currentIndex();
	
	bool ok;
	
	QString text = QInputDialog::getText(NULL, tr("Exportar CSV"), tr("Nombre de archivo:"), QLineEdit::Normal, QDir::home().dirName(), &ok);

	setlocale(LC_ALL, "C");
	//QDir::setCurrent("C:/Users/Candelaria/Escritorio");
	if (modo == 2)
	{
		if (ok && !text.isEmpty())
		{
			text = text + "-pq-ptos.csv";
			ExportarDatos_DosMetodos(tableWidget, text);
		}
		else return;
	}
	else
	{

		if (ok && !text.isEmpty())
		{
			
			if (modo == 1)
			{
				text = text + "-ptos.csv";
				
			}
			if (modo == 0)
			{
				text = text + "-pq.csv";
				
			}	
		}
		else return;
		ExportarDatos_UnMetodo(tableWidget, text);
	}

}

void AwesomeView::ExportarDatos_DosMetodos(QTableWidget *tableWidget, QString filename)
{
	//QFile f("ResultadosAmbosMetodos.csv");

	QFile f(filename);
	f.setFileName(filename);

	if (f.open(QFile::WriteOnly | QFile::Truncate))
	{
		QTextStream data(&f);
		QStringList strList;
		
		for (int i = 0; i < N_casos;i++ )
		{
			if (distancias[i] == INIT_VAL || angulos[i] == INIT_VAL)
			{

				string name = "Plano ajustado según Pieza Quirúrgica " + to_string(i);
				mitk::DataNode::Pointer planoPQ_node = this->GetDataStorage()->GetNamedNode(name);
				name = "Plano ajustado según Puntos Intraoperatorios " + to_string(i);
				mitk::DataNode::Pointer planoIO_node = this->GetDataStorage()->GetNamedNode(name);

				if (!planoPQ_node || !planoIO_node)
				{
					QMessageBox::warning(NULL, "Falta algún plano ajustado.", "Por favor verifique haber ajustado plano para ambos métodos.");
					return;
				}
				vtkSmartPointer <vtkPolyData> boundedPlane_PQ = vtkSmartPointer <vtkPolyData> ::New();
				vtkSmartPointer <vtkPolyData> boundedPlane_IO = vtkSmartPointer <vtkPolyData> ::New();

				boundedPlane_PQ = dynamic_cast<mitk::Surface*>(planoPQ_node->GetData())->GetVtkPolyData();
				boundedPlane_IO = dynamic_cast<mitk::Surface*>(planoIO_node->GetData())->GetVtkPolyData();

				CalcularIndicadoresPlanos(boundedPlane_PQ, boundedPlane_IO, i);

			}
		}


		strList << "\" " + tableWidget->item(0, 0)->text() + "\" "; //"Plano"
																	//Cargo los numeros de plano
		for (int c = 2; c < tableWidget->columnCount() - 1; ++c)
		{
			strList << "\" " + tableWidget->item(0, c)->text() + "\" ";
		}
		data << strList.join(";") << "\n";

		strList.clear();

		QString tit1("Dist centros");		
		strList << "\" " +tit1+ "\" ";
		QString D;
		for (int i = 0; i < N_casos;i++ )
		{
			D=QString::number(distancias[i]);
			strList << "\" " + D + "\" ";
		}

		data << strList.join(";") << "\n";
		strList.clear();

		QString tit2("Ángulo diedro");
		strList << "\" " + tit2 + "\" ";
		QString A;
		for (int i = 0; i < N_casos; i++)
		{
			A=QString::number(angulos[i]);
			strList << "\" " + A + "\" ";
		}

		data << strList.join(";") << "\n";
		strList.clear();

		//Cargo las tolerancias
		
		QString tolerancia("Tolerancias");
		QString ul(QString::number(m_Controls.UL->value()));
		QString ll(QString::number(m_Controls.LL->value()));
		strList << "\" " + tolerancia + "\" "; //"Tolerancia"
		strList << "\" " + ul + "\" "; //"Upper limit"
		strList << "\" " + ll + "\" "; //"Lower limit"
		data << strList.join(";") << "\n";		
		strList.clear();

		//Cargo info pieza quirurgica
		strList << "\" " + tableWidget->item(1, 0)->text() + "\" ";
		data << strList.join(";") << "\n";

		strList.clear();
		strList<< "\" " + tableWidget->item(0, 0)->text() + "\" "; //"Plano"
		//Cargo los numeros de plano
		for (int c = 2; c < tableWidget->columnCount() - 1; ++c)
		{
			strList << "\" " + tableWidget->item(0, c)->text() + "\" ";
		}
		data << strList.join(";") << "\n";

		
		for (int r = 1; r < (tableWidget->rowCount()+1)/2; ++r)
		{
			strList.clear();
			for (int c = 1; c < tableWidget->columnCount() - 1; ++c)
			{
				strList << "\" " + tableWidget->item(r, c)->text() + "\" ";
			}
			data << strList.join(";") << "\n";

		}
		strList.clear();
		strList << "\" " + tableWidget->item(0, tableWidget->columnCount() - 1)->text() + "\" "; //"Total cirugia"
		strList << "\" " + tableWidget->item(1, tableWidget->columnCount() - 1)->text() + "\" ";
		data << strList.join(";") << "\n";

		data << strList.join(";") << "\n";

		//Cargo puntos intraoperatorios
		strList.clear();
		strList << "\" " + tableWidget->item((tableWidget->rowCount() + 1) / 2, 0)->text() + "\" ";
		data << strList.join(";") << "\n";
		
		strList.clear();
		strList << "\" " + tableWidget->item(0, 0)->text() + "\" "; //"Plano"
		//Cargo los numeros de plano
		for (int c = 2; c < tableWidget->columnCount() - 1; ++c)
		{
			strList << "\" " + tableWidget->item(0, c)->text() + "\" ";
		}
		data << strList.join(";") << "\n";
		
		for (int r = (tableWidget->rowCount() + 1) / 2; r < tableWidget->rowCount(); ++r)
		{
			strList.clear();
			for (int c = 1; c < tableWidget->columnCount() - 1; ++c)
			{
				strList << "\" " + tableWidget->item(r, c)->text() + "\" ";
			}
			data << strList.join(";") << "\n";

		}
		strList.clear();
		strList << "\" " + tableWidget->item(0, tableWidget->columnCount() - 1)->text() + "\" "; //"Total cirugia"
		strList << "\" " + tableWidget->item((tableWidget->rowCount() + 1) / 2, tableWidget->columnCount() - 1)->text() + "\" ";
		data << strList.join(";") << "\n";

		f.close();
	}
}
void AwesomeView::ExportarDatos_UnMetodo(QTableWidget *tableWidget,QString file_name)
{
	//QDir::setCurrent("C:/Users/Candelaria/Escritorio");
	QFile f(file_name);

	if (f.open(QFile::WriteOnly | QFile::Truncate))
	{
		QTextStream data(&f);
		QStringList strList;
		strList.clear();
//Cargo las tolerancias
		QString tolerancia("Tolerancias");
		QString ul(QString::number(m_Controls.UL->value()));
		QString ll(QString::number(m_Controls.LL->value()));
		strList << "\" " + tolerancia + "\" "; //"Tolerancia"
		strList << "\" " + ul + "\" "; //"Upper limit"
		strList << "\" " + ll + "\" "; //"Lower limit"
		data << strList.join(";") << "\n";
//Cargo los numeros de plano
		strList.clear();
		strList << "\" " + tableWidget->item(0, 0)->text() + "\" "; //"Plano"
		
		for (int c = 1; c < tableWidget->columnCount() - 1; ++c)
		{
			strList << "\" " + tableWidget->item(0, c)->text() + "\" ";
		}
		data << strList.join(";") << "\n";

//Cargo los indicadores		
		for (int r = 1; r < tableWidget->rowCount(); ++r)
		{
			strList.clear();
			for (int c = 0; c < tableWidget->columnCount() - 1; ++c)
			{
				strList << "\" " + tableWidget->item(r, c)->text() + "\" ";
			}
			data << strList.join(";") << "\n";

		}
//Cargo el total cirugia %
		strList.clear();
		strList << "\" " + tableWidget->item(0, tableWidget->columnCount() - 1)->text() + "\" "; //"Total cirugia"
		strList << "\" " + tableWidget->item(1, tableWidget->columnCount() - 1)->text() + "\" ";
		data << strList.join(";") << "\n";

		f.close();
	}
}
int AwesomeView::CalcularInliersPQ(int case_number)
{

	//NOTA:

	//FALTA REVISAR QUÉ ONDA LA DISTANCIA QUE SE PUSHEA AL vtkPlane (AHORA ES 10)
	//CHEQUEAR TANTO EL VALOR ABSOLUTO DE ESA DISTANCIA COMO EL SIGNO (que siempre se ALEJE de la PQ sin importar cómo quedó calculada la normal)-->chequeado! 

	//----------------------CARGO LA PIEZA QUIRURGICA -------------------//
	mitk::DataNode* PQ_Node = this->GetDataStorage()->GetNamedNode("PQ");
	if (!PQ_Node)
	{
		QMessageBox::warning(NULL, "No se encontró la pieza quirúrgica.", "Por favor cargue la pieza quirúrgica");
		return 0;
	}

	vtkPolyData* PQ = dynamic_cast<mitk::Surface*>(PQ_Node->GetData())->GetVtkPolyData();
	vtkSmartPointer<vtkPlane> plano_para_threshold = vtkSmartPointer<vtkPlane>::New();
	int algoritmo;
	string name = "PointSet " + to_string(m_Controls.cbPlaneNumber->currentIndex());
	mitk::DataNode* PS_Node = this->GetDataStorage()->GetNamedNode(name);

	if (!PS_Node)
	{
		
		if (QMessageBox::Yes == QMessageBox(QMessageBox::Information, "Estimación de superficie", "No hay Pointset para este número de plano. \n \nPuede obtener una mejor estimación de superficie marcando tres puntos sobre el corte de la pieza quirúrgica (Window--> Show View -->Pointset Interaction). \n\n ¿Desea realizar una estimación menos precisa sin pointset de todos modos?", QMessageBox::Yes | QMessageBox::No).exec())
		{
			algoritmo = 3;
		}
		else
			return 0;
	
	}

	else
	{
		algoritmo = 2;
		PS_Node->SetVisibility(false);
	}
	if (algoritmo == 2)
	{
		//---------------------CARGO LOS PUNTOS MARCADOS EN EL POINT SET--------------------//
		
		PS_Node->SetVisibility(false);
		mitk::PointSet* ps = dynamic_cast<mitk::PointSet*>(PS_Node->GetData());
		

		double normal[3] = { 0 };
		vtkTriangle::ComputeNormal(ps->GetPoint(0).GetDataPointer(), ps->GetPoint(1).GetDataPointer(), ps->GetPoint(2).GetDataPointer(), normal);
	
		plano_para_threshold->SetNormal(normal);
		plano_para_threshold->SetOrigin(ps->GetPoint(1).GetDataPointer());
	}
	if (algoritmo == 3)
	{
		vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter = vtkSmartPointer<vtkCenterOfMass>::New();
		centerOfMassFilter->SetInputData(PQ);
		centerOfMassFilter->SetUseScalarsAsWeights(false);
		centerOfMassFilter->Update();
		double pq_center[3];
		centerOfMassFilter->GetCenter(pq_center);

		vtkSmartPointer<vtkOBBTree> ray_casting = vtkSmartPointer<vtkOBBTree>::New();
		ray_casting->SetDataSet(PQ);
		ray_casting->BuildLocator();
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkIdList> cellID = vtkSmartPointer<vtkIdList>::New();

		double *plane_origin = planoOBJ[case_number]->GetOrigin();
    //double *plane_center = planoOBJ[case_number]->GetCenter();
		double *p1 = planoOBJ[case_number]->GetPoint1();
		double *nor = planoOBJ[case_number]->GetNormal();
		double *p2 = planoOBJ[case_number]->GetPoint2();

		double plane_point1[3];
		double plane_point2[3];
		double plane_point1_opp[3];
		double plane_point2_opp[3];
		double plane_center_displaced[3];
    //double plane_center_displaced_opp[3];

		plane_center_displaced[0] = pq_center[0] + 20 * nor[0];
		plane_center_displaced[1] = pq_center[1] + 20 * nor[1];
		plane_center_displaced[2] = pq_center[2] + 20 * nor[2];

		plane_point1[0] = pq_center[0] + 0.15*(p2[0] - plane_origin[0]);
		plane_point1[1] = pq_center[1] + 0.15*(p2[1] - plane_origin[1]);
		plane_point1[2] = pq_center[2] + 0.15*(p2[2] - plane_origin[2]);

		plane_point2[0] = pq_center[0] + 0.15*(p1[0] - plane_origin[0]);
		plane_point2[1] = pq_center[1] + 0.15*(p1[1] - plane_origin[1]);
		plane_point2[2] = pq_center[2] + 0.15*(p1[2] - plane_origin[2]);

		plane_point1_opp[0] = plane_point1[0] + 20 * nor[0];
		plane_point1_opp[1] = plane_point1[1] + 20 * nor[1];
		plane_point1_opp[2] = plane_point1[2] + 20 * nor[2];

		plane_point2_opp[0] = plane_point2[0] + 20 * nor[0];
		plane_point2_opp[1] = plane_point2[1] + 20 * nor[1];
		plane_point2_opp[2] = plane_point2[2] + 20 * nor[2];

		mitk::PointSet::Pointer ps2 = mitk::PointSet::New();
		ps2->InsertPoint(plane_point1);
		ps2->InsertPoint(plane_point2);
		ps2->InsertPoint(plane_point1_opp);
		ps2->InsertPoint(plane_point2_opp);
		ps2->InsertPoint(plane_center_displaced);
		ps2->InsertPoint(pq_center);

    //mitk::DataNode::Pointer P = mitk::DataNode::New();
    //P->SetData(ps2);
    //P->SetName("Puntos para recta de ray casting");
    //GetDataStorage()->Add(P);

		mitk::PointSet::Pointer ps = mitk::PointSet::New();
		int found = ray_casting->IntersectWithLine(plane_point1_opp, plane_point1, points, cellID);
		if (found >= 1)
			ps->InsertPoint(points->GetPoint(0));

		found = ray_casting->IntersectWithLine(plane_point2_opp, plane_point2, points, cellID);
		if (found >= 1)
			ps->InsertPoint(points->GetPoint(0));

		found = ray_casting->IntersectWithLine(plane_center_displaced, pq_center, points, cellID);
		if (found >= 1)
			ps->InsertPoint(points->GetPoint(0));

    //mitk::DataNode::Pointer PS_Node = mitk::DataNode::New();
    //PS_Node->SetData(ps);
    //PS_Node->SetName("Pointset prueba");
    //GetDataStorage()->Add(PS_Node);


		double normal[3] = { 0 };
		vtkTriangle::ComputeNormal(ps->GetPoint(0).GetDataPointer(), ps->GetPoint(1).GetDataPointer(), ps->GetPoint(2).GetDataPointer(), normal);

		plano_para_threshold->SetNormal(normal);
		plano_para_threshold->SetOrigin(ps->GetPoint(1).GetDataPointer());
	}

	if (algoritmo == 4)
	{
		vtkSmartPointer<vtkOBBTree> ray_casting = vtkSmartPointer<vtkOBBTree>::New();
		ray_casting->SetDataSet(PQ);
		ray_casting->BuildLocator();
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkIdList> cellID = vtkSmartPointer<vtkIdList>::New();

		vtkSmartPointer<vtkPoints> ps = vtkSmartPointer<vtkPoints>::New();

		double *nor = planoOBJ[case_number]->GetNormal();
		


		for (vtkIdType i = 0; i < planoOBJ[case_number]->GetOutput()->GetNumberOfPoints(); i++)
		{
			
			double *extremo1 = new double[3];
			extremo1= planoOBJ[case_number]->GetOutput()->GetPoint(i);
			extremo1[0] = extremo1[0]+100 * nor[0];
			extremo1[1] = extremo1[1]+100 * nor[1];
			extremo1[2] = extremo1[2]+100 * nor[2];

			double *extremo2=new double[3];
			extremo2[0] = extremo1[0] - 200 * nor[0];
			extremo2[1] = extremo1[1] - 200 * nor[1];
			extremo2[2] = extremo1[2] - 200 * nor[2];

			int found = ray_casting->IntersectWithLine(extremo1, extremo2, points, cellID);
			if (abs(found) >= 1)
				ps->InsertNextPoint(points->GetPoint(0));

	
		}

	

		vtkSmartPointer<vtkCellArray> verts = vtkSmartPointer<vtkCellArray>::New();
		verts->InsertNextCell(ps->GetNumberOfPoints());
		for (vtkIdType k = 0; k<ps->GetNumberOfPoints(); k++)
		{
			double p1[3];
			ps->GetPoint(k, p1);
			verts->InsertCellPoint(k);
		}

		vtkSmartPointer<vtkPolyData> pspo = vtkSmartPointer<vtkPolyData>::New();
		pspo->SetPoints(ps);
		pspo->SetVerts(verts);
		
		cout << pspo->GetNumberOfPoints() << endl;

		vtkRANSACPlane *ajustado = AjustarPlano(pspo);

		mitk::DataNode::Pointer ransac_Node = mitk::DataNode::New();
		mitk::Surface::Pointer surf = mitk::Surface::New();
		surf->SetVtkPolyData(ajustado->GetOutput());
		ransac_Node->SetData(surf);
		ransac_Node->SetName("plano guia");
		GetDataStorage()->Add(ransac_Node);

		plano_para_threshold->SetNormal(ajustado->normal);
		plano_para_threshold->SetOrigin(ajustado->origin);




	}
	


  //-------------------------CALCULO THRESHOLD ADAPTADO--------------------------//
  int n=PQ->GetNumberOfPoints();

  double threshold;

  double minD=85000;
  double maxD=0;
        for(vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
           {

               double p0[3];
               PQ->GetPoint(i,p0);


              double squaredDistance = plano_para_threshold->DistanceToPlane(p0);

              if (squaredDistance<minD)
              {

                  minD=squaredDistance;
              }
              if (squaredDistance>maxD)
              {

                     maxD=squaredDistance;
              }
          }


    int push =3;
    plano_para_threshold->Push(push);
	double *D=(double*)malloc(n*sizeof(double));
    double minD2=87000;
    double maxD2=0;

    for(vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
       {

           double p_0[3];
           PQ->GetPoint(i,p_0);


          double squaredDistance = plano_para_threshold->DistanceToPlane(p_0);


          D[i]=squaredDistance;

          if (squaredDistance<minD2)
          {

              minD2=squaredDistance;
          }
          if (squaredDistance>maxD2)
          {

                 maxD2=squaredDistance;
          }
      }

	cout << "maximo inicial: " << maxD << ". Maximo final: " << maxD2 << endl;
    if(maxD2<maxD)  //El plano se movio hacia ADENTRO DE LA PIEZA QUIRURGICA. Hay que moverlo para el otro lado
    {

          plano_para_threshold->Push(-2*push);

          double minD3=87000;
          double maxD3=0;
          for(vtkIdType i = 0; i < PQ->GetNumberOfPoints(); i++)
             {

                 double p[3];
                 PQ->GetPoint(i,p);


                double squaredDistance = plano_para_threshold->DistanceToPlane(p);


                D[i]=squaredDistance;

                if (squaredDistance<minD3)
                {

                    minD3=squaredDistance;
                }
                if (squaredDistance>maxD3)
                {

                       maxD3=squaredDistance;
                }
            }

          minD2=minD3;
          maxD2=maxD3;
    }




    threshold=minD2+maxD2/20;


// ---------- Una vez que tengo el threshold: Identificar puntos de la PQ que cumplen el threshold ---------------//

//Creo conjunto de puntos donde se cargarán los puntos cercanos al plano objetivo
  vtkSmartPointer<vtkPoints> CutSurfacePoints =  vtkSmartPointer<vtkPoints>::New();

  vtkSmartPointer<vtkCellArray> verts =  vtkSmartPointer<vtkCellArray>::New();

  for (vtkIdType i=0; i<n; i++)
   {
      if(D[i]<threshold)
      {

        double p0[3];
        PQ->GetPoint(i,p0);
        CutSurfacePoints->InsertNextPoint(p0);

       }
     }

   verts->InsertNextCell(CutSurfacePoints->GetNumberOfPoints());

   for (vtkIdType k=0; k<CutSurfacePoints->GetNumberOfPoints();k++)
   {
      double p1[3];
      CutSurfacePoints->GetPoint(k,p1);
      verts->InsertCellPoint(k);
   }
   vtkSmartPointer<vtkPolyData> polydataInliers =vtkSmartPointer<vtkPolyData>::New();
   polydataInliers->SetPoints(CutSurfacePoints);
   polydataInliers->SetVerts(verts);

   mitk::Surface::Pointer surfaceInliers = mitk::Surface::New();

   surfaceInliers->SetVtkPolyData(polydataInliers);

   mitk::DataNode::Pointer inliers_Node = mitk::DataNode::New();


   inliers_Node->SetData(surfaceInliers);

   switch (case_number)
   {
   case 0:
	   inliers_Node->SetColor(R_PO0 / 255.0, G_PO0 / 255.0, B_PO0 / 255.0);
	   inliers_Node->Update();
	   break;
   case 1:
	   inliers_Node->SetColor(R_PO1 / 255.0, G_PO1 / 255.0, B_PO1 / 255.0);
	   inliers_Node->Update();
	   break;
   case 2:
	   inliers_Node->SetColor(R_PO2 / 255.0, G_PO2 / 255.0, B_PO2 / 255.0);
	   inliers_Node->Update();
	   break;
   case 3:
	   inliers_Node->SetColor(R_PO3 / 255.0, G_PO3 / 255.0, B_PO3 / 255.0);
	   inliers_Node->Update();
	   break;
   }
   inliers_Node->SetName("InliersPQ " + std::to_string(case_number));


   GetDataStorage()->Add(inliers_Node);
   return 1;

}





void AwesomeView::AjustarPlanoPQ()
{
  string name = "InliersPQ " + std::to_string(m_Controls.cbPlaneNumber->currentIndex());
  mitk::DataNode* inliers_node = this->GetDataStorage()->GetNamedNode(name);
  inliers_node->SetVisibility(false);
  if (!inliers_node)
  {
    QMessageBox::warning(NULL, "Calcule los Inliers", "No se encontró el nodo de InliersPQ para el caso solicitado");
    return;
  }

  vtkPolyData* inliers = dynamic_cast<mitk::Surface*>(inliers_node->GetData())->GetVtkPolyData();

  mitk::Surface::Pointer ransac_surface= mitk::Surface::New();
  planoPQ[m_Controls.cbPlaneNumber->currentIndex()] = AjustarPlano(inliers);
  ransac_surface->SetVtkPolyData(planoPQ[m_Controls.cbPlaneNumber->currentIndex()]->GetOutput());
 

  mitk::DataNode::Pointer ransac_Node = mitk::DataNode::New();


  ransac_Node->SetData(ransac_surface);
  ransac_Node->SetName("Plano ajustado según Pieza Quirúrgica " + std::to_string(m_Controls.cbPlaneNumber->currentIndex()));
  ransac_Node->SetColor(R_PLANO_PQ/255,G_PLANO_PQ/255.0,B_PLANO_PQ/255.0);

  GetDataStorage()->Add(ransac_Node);
}

void AwesomeView::AjustarPlanoIO()
{

  string name = "Puntos intraoperatorios " + std::to_string(m_Controls.cbPlaneNumber->currentIndex());
  mitk::DataNode* inliers_node = this->GetDataStorage()->GetNamedNode(name);
  inliers_node->SetVisibility(false);


  if (!inliers_node)
  {
    QMessageBox::warning(NULL, "Cargue los puntos intraoperatorios", "No se encontró nodo de Puntos Intraoperatorios para el caso solicitado");
	  return;
  }

    mitk::PointSet* ps = dynamic_cast<mitk::PointSet*>(inliers_node->GetData());
  vtkSmartPointer<vtkPoints> PuntosIntraop =  vtkSmartPointer<vtkPoints>::New();

    vtkSmartPointer<vtkCellArray> verts =  vtkSmartPointer<vtkCellArray>::New();
     verts->InsertNextCell(ps->GetSize());

     int count=0;

    for (mitk::PointSet::PointsConstIterator it = ps->Begin(); it != ps->End(); ++it)
    {


      PuntosIntraop->InsertNextPoint(it->Value().GetDataPointer());
      verts->InsertCellPoint(count++);

      }


     vtkSmartPointer<vtkPolyData> inliers =vtkSmartPointer<vtkPolyData>::New();
     inliers->SetPoints(PuntosIntraop);
     inliers->SetVerts(verts);

     mitk::Surface::Pointer surface_plano_ajustado=mitk::Surface::New();
    

	 planoIO[m_Controls.cbPlaneNumber->currentIndex()] = AjustarPlano(inliers);
     surface_plano_ajustado->SetVtkPolyData(planoIO[m_Controls.cbPlaneNumber->currentIndex()]->GetOutput());

     mitk::DataNode::Pointer node_plano_ajustado = mitk::DataNode::New();


     node_plano_ajustado->SetData(surface_plano_ajustado);
     node_plano_ajustado->SetName("Plano ajustado según Puntos Intraoperatorios " + std::to_string(m_Controls.cbPlaneNumber->currentIndex()));
     node_plano_ajustado->SetColor(R_PLANO_IO/255.0,G_PLANO_IO/255.0,B_PLANO_IO/255.0);
     GetDataStorage()->Add(node_plano_ajustado);

}
vtkRANSACPlane* AwesomeView::AjustarPlano(vtkPolyData* inliers)
{
  /*NOTA
   Posible mejora: estudiar los parametros del ransac Plane, por ej: InlierThreshold.
    */


  //-------------------Guardo los puntos en un archivo para leerlos (lo necesito para el RANSAC)-----------//
           vtkSmartPointer<vtkXMLPolyDataWriter> Writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
           std::string outputTh="OUTPUT_Thresholding";
           Writer->SetFileName(outputTh.c_str());
           Writer->SetInputData(inliers);
           Writer->Write();


           //read the input file
           vtkSmartPointer<vtkXMLPolyDataReader> readerransac =
               vtkSmartPointer<vtkXMLPolyDataReader>::New();
           readerransac->SetFileName(outputTh.c_str());
           readerransac->Update();

           //vtkPolyData* inputPoints = Reader->GetOutput();

           //estimate normals
           vtkRANSACPlane* rANSACPlane = new vtkRANSACPlane;


           int n_points_to_fit=0;
           double inlier_threshold;
           if (inliers->GetNumberOfPoints()>3000)
           {
             n_points_to_fit=3000;
             inlier_threshold=0.1;
           }
           else
           {
             n_points_to_fit=inliers->GetNumberOfPoints();
             inlier_threshold=0.6;
           }


		rANSACPlane->SetNumberOfPointsToFit(n_points_to_fit);

		rANSACPlane->SetInlierThreshold(inlier_threshold);

        rANSACPlane->SetInputConnection(readerransac->GetOutputPort());

        rANSACPlane->Update();



        return rANSACPlane;



}


