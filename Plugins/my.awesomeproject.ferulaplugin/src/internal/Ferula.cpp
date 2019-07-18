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


#include "Ferula.h"
#include "FerulaInteractor.h"

//#include "vtkRANSACPlane.h"

#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// us
#include <usModuleRegistry.h>
#include <usModule.h>
#include <usModuleContext.h>
#include <usGetModuleContext.h>

#include <vtkTriangle.h>
#include <vtkCutter.h>
#include <vtkCleanPolyData.h>
#include <vtkAppendPolyData.h>


#include <QMessageBox>
#include <QString>
#include <QSettings>
#include <QFileDialog>
#include <QTableWidget>
#include <QInputDialog>

#include <mitkImageCast.h>
#include <mitkLookupTables.h>





using namespace std;

Ferula::Ferula()
{

}

const std::string Ferula::VIEW_ID = "my.awesomeproject.views.Ferula";

void Ferula::CreateQtPartControl(QWidget* parent)
{
  // Setting up the UI is a true pleasure when using .ui files, isn't it?
  m_Controls.setupUi(parent);

  connect(m_Controls.pbinteractor, SIGNAL (clicked()),this, SLOT(onInteractor()));
  connect(m_Controls.pbAdjust, SIGNAL(clicked()),this, SLOT(onAdjust()));
  connect(m_Controls.pbRadius, SIGNAL(valueChanged(int)), SLOT(setRadius(int)));

}
void Ferula::SetFocus()
{
}
void Ferula::setRadius(int newradius)
{

  m_CurrentInteractor->SetRadiusPointer(newradius);

}
void Ferula::onCreateFerula()
{
  createFerula();
}
void Ferula::onAdjust()
{
  m_CurrentInteractor->adjustToArm();
}

void Ferula::onTubes()
{
  mitk::DataNode::Pointer tube_node = mitk::DataNode::New();

  tube_node->SetName("Tubo");


  m_CurrentInteractor->createTubes(tube_node);

  cout<<"se agrega el tubo"<<endl;
  GetDataStorage()->Add(tube_node);
  tube_node->Modified();
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();

}
void Ferula::onInteractor()
{
  us::Module* module = us::GetModuleContext()->GetModule("MyAwesomeLib");
  mitk::DataNode::Pointer dataNode = mitk::DataNode::New();
  mitk::GeometryData::Pointer gd = mitk::GeometryData::New();
  dataNode->SetData(gd);
  dataNode->SetName("Interactor");
  dataNode->SetVisibility(true);
  GetDataStorage()->Add(dataNode);

  m_CurrentInteractor = FerulaInteractor::New();

  m_CurrentInteractor->SetDataStorage(GetDataStorage());
  m_CurrentInteractor->LoadStateMachine("Ferula.xml", module);
  m_CurrentInteractor->SetEventConfig("FerulaConfig.xml", module);
  m_CurrentInteractor->SetDataNode(dataNode);
  mitk::DataNode::Pointer armnode = this->GetDataStorage()->GetNamedNode("Brazo");
  if (!armnode)
  {
    QMessageBox::warning(NULL, "NO SE ENCONTRÓ EL BRAZO", "Por favor cargue la superficie del brazo y nombrela 'Brazo'.");
    return;
  }
  armnode->SetBoolProperty("pickable",true);
  m_CurrentInteractor->SetArmNode(armnode);

}

int Ferula::getPoints()
{
  ps_mano = mitk::PointSet::New();
  ps_codo = mitk::PointSet::New();
  string name_mano = "PointSet_mano";
  string name_codo = "PointSet_codo";
  mitk::DataNode* PS_Node_mano = this->GetDataStorage()->GetNamedNode(name_mano);
  mitk::DataNode* PS_Node_codo = this->GetDataStorage()->GetNamedNode(name_codo);
  if (!PS_Node_mano || !PS_Node_codo)
  {
    QMessageBox::warning(NULL, "No se encontró puntos de la mano." , "Por favor marque los puntos extremos sobre la superficie de la mano");
    return 0;
  }
  else
  {
    ps_mano = dynamic_cast<mitk::PointSet*>(PS_Node_mano->GetData());
    ps_codo = dynamic_cast<mitk::PointSet*>(PS_Node_codo->GetData());
    return 1;
  }
}

int Ferula::cutters(vtkSmartPointer<vtkPlane> planeMano, vtkSmartPointer<vtkPlane> planeCodo)
{

  double *normal_codo = planeCodo->GetNormal();
  double *normal_mano = planeMano->GetNormal();
  double current_normal[3]={0};

  double *origin_codo = planeCodo->GetOrigin();
  double *origin_mano = planeMano->GetOrigin();
  double current_origin[3]={0};


  vtkSmartPointer<vtkAppendPolyData> appendFilter =
    vtkSmartPointer<vtkAppendPolyData>::New();

  vtkSmartPointer<vtkCutter> cutterDistal =
    vtkSmartPointer<vtkCutter>::New();
  cutterDistal->SetCutFunction(planeMano);
  cutterDistal->SetInputData(polydata_brazo);
  cutterDistal->Update();
  appendFilter->AddInputData(cutterDistal->GetOutput());
  appendFilter->Update();

  vtkSmartPointer<vtkCutter> cutterProximal =
    vtkSmartPointer<vtkCutter>::New();
  cutterProximal->SetCutFunction(planeCodo);
  cutterProximal->SetInputData(polydata_brazo);
  cutterProximal->Update();
  appendFilter->AddInputData(cutterProximal->GetOutput());
  appendFilter->Update();


  int N_horizontales = 5;
  current_origin[0] = origin_mano[0];
  current_origin[1] = origin_mano[1];
  current_origin[2] = origin_mano[2];


  double step[3]={0};
  step[0] = ((origin_mano[0]-origin_codo[0])>= 0 ? -1. : 1. )*abs(origin_codo[0] - origin_mano[0])/N_horizontales;
  step[1] = ((origin_mano[1]-origin_codo[1])>= 0 ? -1. : 1. )*abs(origin_codo[1] - origin_mano[1])/N_horizontales;
  step[2] = ((origin_mano[2]-origin_codo[2])>= 0 ? -1. : 1. )*abs(origin_codo[2] - origin_mano[2])/N_horizontales;

  vtkSmartPointer<vtkPlane> current_plane =
      vtkSmartPointer<vtkPlane>::New();

  for (int i = 0; i< N_horizontales-1; i++)
  {

    current_normal[0] =(1+((rand() % 10 - 5 ))/100)*(abs(normal_codo[0])+abs(normal_mano[0]))/2;
    current_normal[1] =(1+((rand() % 10 - 5 ))/100)*(abs(normal_codo[1])+abs(normal_mano[1]))/2;
    current_normal[2] =(1+((rand() % 10 - 5 ))/100)*(abs(normal_codo[2])+abs(normal_mano[2]))/2;
    current_plane->SetNormal(current_normal);

    current_origin[0] += step[0];
    current_origin[1] += step[1];
    current_origin[2] += step[2];
    current_plane->SetOrigin(current_origin);

    vtkSmartPointer<vtkCutter> cutter =
      vtkSmartPointer<vtkCutter>::New();

    cutter->SetCutFunction(current_plane);
    cutter->SetInputData(polydata_brazo);
    cutter->Update();

    appendFilter->AddInputData(cutter->GetOutput());
    appendFilter->Update();

  }
  cout<<appendFilter->GetNumberOfInputPorts()<<endl;
  cout<<appendFilter->GetTotalNumberOfInputConnections()<<endl;

  // Remove any duplicate points.
  vtkSmartPointer<vtkCleanPolyData> cleanFilter =
    vtkSmartPointer<vtkCleanPolyData>::New();
  cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
  cleanFilter->Update();

  mitk::Surface::Pointer cutter_surface= mitk::Surface::New();
  cutter_surface->SetVtkPolyData(cleanFilter->GetOutput());


  mitk::DataNode::Pointer cutterNode = mitk::DataNode::New();
  cutterNode->SetData(cutter_surface);
  cutterNode->SetName("Cutter");
  GetDataStorage()->Add(cutterNode);
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  return 1;
}

int Ferula::cutHand()
{


  double normal_mano[3] = { 0 };
  vtkTriangle::ComputeNormal(ps_mano->GetPoint(0).GetDataPointer(), ps_mano->GetPoint(1).GetDataPointer(), ps_mano->GetPoint(2).GetDataPointer(), normal_mano);

  vtkSmartPointer<vtkPlane> plane_mano = vtkSmartPointer<vtkPlane>::New();
  plane_mano->SetNormal(normal_mano);
  plane_mano->SetOrigin(ps_mano->GetPoint(1).GetDataPointer());

  double normal_codo[3] = { 0 };
  vtkTriangle::ComputeNormal(ps_codo->GetPoint(0).GetDataPointer(), ps_codo->GetPoint(1).GetDataPointer(), ps_codo->GetPoint(2).GetDataPointer(), normal_codo);

  vtkSmartPointer<vtkPlane> plane_codo = vtkSmartPointer<vtkPlane>::New();
  plane_codo->SetNormal(normal_codo);
  plane_codo->SetOrigin(ps_codo->GetPoint(1).GetDataPointer());


  mitk::DataNode::Pointer node_brazo = this->GetDataStorage()->GetNamedNode("Brazo");
  polydata_brazo = vtkSmartPointer <vtkPolyData> ::New();

  if (!node_brazo)
  {
    QMessageBox::warning(NULL, "No se encontró STL del brazo." , "Por favor cargue el STL del brazo y nombrelo 'Brazo'");
    return 0;
  }
  polydata_brazo = dynamic_cast<mitk::Surface*>(node_brazo->GetData())->GetVtkPolyData();
  cutters(plane_mano,  plane_codo);

  /*
  vtkSmartPointer<vtkCutter> cutter =
    vtkSmartPointer<vtkCutter>::New();
  cutter->SetCutFunction(plane_codo);
  cutter->SetInputData(polydata_brazo);
  cutter->Update();
  mitk::Surface::Pointer cutter_surface= mitk::Surface::New();

  cutter_surface->SetVtkPolyData(cutter->GetOutput());
  mitk::DataNode::Pointer cutterNode = mitk::DataNode::New();

  cutterNode->SetData(cutter_surface);
  cutterNode->SetName("Cutter");

  GetDataStorage()->Add(cutterNode);

  vtkSmartPointer<vtkCutter> cutter2 =
    vtkSmartPointer<vtkCutter>::New();
  cutter2->SetCutFunction(plane_mano);
  cutter2->SetInputData(polydata_brazo);
  cutter2->Update();
  */
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();

  return 0;
}

int Ferula::createFerula(){

  //ImageType::Pointer Net_image = ImageType::New(); //Imagen de la maqueta 3D
  //create3DNet(Net_image,400,400,400);
  getPoints();
  cutHand();
  /*
  mitk::Image::Pointer Net_MITK_image = mitk::Image::New();
  CastToMitkImage(Net_image, Net_MITK_image);
  mitk::DataNode::Pointer net_node = mitk::DataNode::New();
  net_node->SetName("Arbol");

  net_node->SetData(Net_MITK_image);

  GetDataStorage()->Add(net_node);
  */
  return 0;
}


int Ferula::create3DNet(ImageType::Pointer image, unsigned long input_size_0, unsigned long input_size_1,  unsigned long input_size_2){

  constexpr long Pendiente = 1;
  constexpr long Saltos = 50;
  ImageType::RegionType region;

  ImageType::IndexType start;
  start.Fill( 0 );

  region.SetIndex(start);

  ImageType::SizeType size;
  size[0] = input_size_0;
  size[1] = input_size_1;
  size[2] = input_size_2;
//  long max_size=0
//  for (int i=0; i<3;i++)
//  {
//    if (size[i]>max_size)
//    {
//      max_size=size[i];
//    }
//  }
  region.SetSize(size);

  //ImageType::Pointer image = ImageType::New();
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer( itk::NumericTraits< PixelType >::Zero );

  ImageType::IndexType pixelIndex;
  ImageType::IndexValueType y_coord;
  ImageType::IndexValueType x_coord;
  ImageType::IndexValueType z_coord;

  for(z_coord = 0; z_coord <400; z_coord++)
    {
    pixelIndex[2] = z_coord;
    cout<<"Z:"<<endl;
    cout<<pixelIndex.GetElement(2)<<endl;
    for (int i_recta = 0 ; i_recta<8; i_recta++)
      {
      x_coord=i_recta*Saltos;

      for (int j_punto = 0 ; j_punto < 400; j_punto++)
        {
        x_coord++;
        pixelIndex[0]= x_coord;
        //cout<<"X:"<<endl;
        //cout<<pixelIndex.GetElement(0)<<endl;
        //Pendientes positiva-->

        y_coord= Pendiente*j_punto;
        if (y_coord<400)
        {
          pixelIndex[1]=y_coord;
          //cout<<"Y:"<<endl;
          //cout<<pixelIndex.GetElement(1)<<endl;
        }
        else
        {
          break;
        }
        image->SetPixel(pixelIndex, 255);
        //Pendientes negativas -->
        y_coord=400-y_coord;
        if (y_coord>0)
        {
          pixelIndex[1]=y_coord;
        }
        else
        {
          break;
        }
        image->SetPixel(pixelIndex, 255);
        }
      }
    }



  try
      {
      image->Update();
      }
  catch( itk::ExceptionObject & error )
      {
      std::cerr << "Error: " << error << std::endl;
      return EXIT_FAILURE;
      }

    return EXIT_SUCCESS;

}
