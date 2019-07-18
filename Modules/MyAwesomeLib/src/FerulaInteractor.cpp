#include "FerulaInteractor.h"

#include <mitkInteractionConst.h>
#include <mitkInteractionPositionEvent.h>
#include <mitkSurface.h>
#include <mitkLookupTableProperty.h>
#include <mitkLookupTables.h>
#include <mitkVtkPropRenderer.h>
#include <mitkInteractionConst.h>
#include <mitkPointLocator.h>
#include <string.h>

#include <vtkDataSetMapper.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPlaneSource.h>
#include <vtkInteractorObserver.h>
#include <vtkMath.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkCellLocator.h>
#include <vtkPointLocator.h>
#include <vtkCell.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkSphereSource.h>
#include <vtkAppendPolyData.h>
#include <vtkPlane.h>
#include <vtkLineSource.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkTubeFilter.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkAppendPolyData.h>
#include <vtkImageData.h>
#include <vtkImageLogic.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkImageDataGeometryFilter.h>

#include <mitkInteractionConst.h>
#include <mitkBaseRenderer.h>
#include <mitkRenderWindow.h>
#include <mitkInternalEvent.h>
#include <mitkMouseMoveEvent.h>
#include <mitkPointOperation.h>
#include <mitkVtkPropRenderer.h>
#include <mitkImage.h>

#include <mitkRenderingManager.h>
#include <mitkDataNode.h>
#include <mitkBaseRenderer.h>
#include <mitkVtkRepresentationProperty.h>
#include <vtkImageWriter.h>

#include <vtkCellLocator.h>
#include <vtkPointLocator.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>


#define BASE 220
#define HIGHLIGHTBASE 100
#define MARKING 1
#define FERULA 50
using namespace mitk;

void FerulaInteractor::ConnectActionsAndFunctions()
{
    CONNECT_FUNCTION("hover", hover);
    CONNECT_FUNCTION("newMark", newMark);
    CONNECT_FUNCTION("newErase", newErase);
    CONNECT_FUNCTION("displayMark",displayMark);
    CONNECT_FUNCTION("defineMark",defineMark);
    CONNECT_FUNCTION("displayErase",displayErase);
    CONNECT_FUNCTION("defineErase",defineErase);
    CONNECT_FUNCTION("cancelMark",cancelMark);
    CONNECT_FUNCTION("firstPoint",firstPoint);
    CONNECT_FUNCTION("middlePoint",middlePoint);
    CONNECT_FUNCTION("lastPoint",lastPoint);
    CONNECT_FUNCTION("cancelPointSet",cancelPointSet);
    CONNECT_FUNCTION("cancelErase",cancelErase);

}
//  ---------------------
//  C O N S T R U C T O R
FerulaInteractor::FerulaInteractor()
{
  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  colores = vtkSmartPointer<vtkDoubleArray>::New();
  lut->SetTableRange(0,255);
  lut->Build();

  mitkLut = mitk::LookupTable::New();
  mitkLut->SetVtkLookupTable(lut);

  mOriginalArmNode = mitk::DataNode::New();
  erasingIDs = vtkSmartPointer<vtkIdList>::New();
  markedPoints = vtkSmartPointer<vtkIdList>::New();
  ferula = vtkSmartPointer<vtkAppendPolyData>::New();
  current_tube = vtkSmartPointer<vtkAppendPolyData>::New();
  current_tube_node = mitk::DataNode::New();
  current_tube_node->SetName("Tubo en proceso");
  current_tube_node->SetColor(255.,153.,153.);
  current_tube_node->SetBoolProperty("helper object", false);
  ferula_node = mitk::DataNode::New();
  ferula_node->SetName("Ferula");
  ferula_node->SetColor(255.,0.,0.);



}
//  ---------------------
//  D E S T R U C T O R
FerulaInteractor::~FerulaInteractor()
{


}
//----------------------
//  I N T E R A C C I O N E S
void FerulaInteractor::SetDataNode(mitk::DataNode* dataNode)
{

  DataInteractor::SetDataNode(dataNode);


}
void FerulaInteractor::SetArmNode(mitk::DataNode* dataNode)
{

  mOriginalArmNode = dataNode;

  vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
  mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
  armPolydata = surf->GetVtkPolyData();
  colores->SetNumberOfComponents(1);
  colores->SetNumberOfValues(armPolydata->GetNumberOfPoints());


  long i=0;
  while (i < armPolydata->GetNumberOfPoints())
  {
    colores->SetValue(i,BASE);
      //cellData->InsertTuple(i, base);

     i++;
  }
  cout<<colores->GetNumberOfValues()<<endl;
  armPolydata->GetPointData()->SetScalars(colores);
  mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
  mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
  mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
  mOriginalArmNode->SetBoolProperty("scalar visibility", true);
  mOriginalArmNode->SetBoolProperty("color mode", true);
  radius=0.5;
  lastCellID = 0;

  surf->SetVtkPolyData(armPolydata);
  mOriginalArmNode->SetData(surf);

  cout<<"Se seteo el nodo de brazo"<<endl;

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();


}

void FerulaInteractor::SetRadiusPointer(int int_radius)
{
  cout<<"radius int: "<<int_radius<<endl;
  radius = 0.1 * int_radius +0.5;
}

void FerulaInteractor::SetDataStorage(mitk::DataStorage::Pointer ds)
{
  mDataStorage = ds;
  mDataStorage->Add(current_tube_node);
  mDataStorage->Add(ferula_node);

  //mGhost->SetDataStorage(ds);
}
void FerulaInteractor::firstPoint(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{
 //marcar el primer punto
    markedPoints->Reset();

    mitk::DataNode::Pointer pickedNode = mitk::DataNode::New();

    if (interactionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionEvent" << std::endl;
      return;
    }

    const InteractionPositionEvent *positionEvent = dynamic_cast<const InteractionPositionEvent*>(interactionEvent);

    if (positionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionPositionEvent" << std::endl;
      return;
    }

    // ** 3D Interaction ** //
    if (interactionEvent->GetSender()->GetName() == std::string("stdmulti.widget4"))
    {

      Point2D coordinates = positionEvent->GetPointerPositionOnScreen();
      Point3D intersectionpoint;
      mitk::VtkPropRenderer::Pointer sender = static_cast<mitk::VtkPropRenderer*>(interactionEvent->GetSender());
      pickedNode = sender->PickObject(coordinates, intersectionpoint);

      //cout<<intersectionpoint<<endl;
      // Pick plane using hover in 3D window
      if(pickedNode == mOriginalArmNode)
      {
        vtkIdType cellId;
        double x[3];
        x[0]=intersectionpoint[0];x[1]=intersectionpoint[1];x[2]=intersectionpoint[2];
        vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
        mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
        vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
        armPolydata = surf->GetVtkPolyData();
        cellLocator->SetDataSet(armPolydata);
        cellLocator->BuildLocator();
        cellId= cellLocator->FindCell(x);
        markedPoints->InsertNextId(armPolydata->GetCell(cellId)->GetPointIds()->GetId(0));
        cout<<"Se agrego el punto: "<<endl;
        cout<<intersectionpoint<<endl;
        vtkIdList *touched_points;
        touched_points = armPolydata->GetCell(cellId)->GetPointIds();
        long n = touched_points->GetNumberOfIds();
        if (n==0)
        {
          return;
        }

        for (long i=0; i<n;i++)
        {
           colores->SetValue(touched_points->GetId(i),MARKING);
        }
        lastCellID=cellId;
        armPolydata->GetPointData()->SetScalars(colores);
        mitk::Surface::Pointer new_surf = mitk::Surface::New();
        new_surf->SetVtkPolyData(armPolydata);
        mOriginalArmNode->SetData(new_surf);
        mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
        mOriginalArmNode->SetBoolProperty("scalar visibility", true);
        mOriginalArmNode->SetBoolProperty("color mode", true);
        mOriginalArmNode->Modified();
        mitk::RenderingManager::GetInstance()->RequestUpdateAll();

      }
    }
}
void FerulaInteractor::middlePoint(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{

  mitk::DataNode::Pointer pickedNode = mitk::DataNode::New();

  if (interactionEvent == nullptr)
  {
    MITK_ERROR << "No InteractionEvent" << std::endl;
    return;
  }

  const InteractionPositionEvent *positionEvent = dynamic_cast<const InteractionPositionEvent*>(interactionEvent);

  if (positionEvent == nullptr)
  {
    MITK_ERROR << "No InteractionPositionEvent" << std::endl;
    return;
  }

  // ** 3D Interaction ** //
  if (interactionEvent->GetSender()->GetName() == std::string("stdmulti.widget4"))
  {

    Point2D coordinates = positionEvent->GetPointerPositionOnScreen();
    Point3D intersectionpoint;
    mitk::VtkPropRenderer::Pointer sender = static_cast<mitk::VtkPropRenderer*>(interactionEvent->GetSender());
    pickedNode = sender->PickObject(coordinates, intersectionpoint);

    //cout<<intersectionpoint<<endl;
    // Pick plane using hover in 3D window
    if(pickedNode == mOriginalArmNode)
    {
      double x[3];
      x[0]=intersectionpoint[0];x[1]=intersectionpoint[1];x[2]=intersectionpoint[2];
      vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
      mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
      vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
      armPolydata = surf->GetVtkPolyData();
      vtkIdType cellId;
      cellLocator->SetDataSet(armPolydata);
      cellLocator->BuildLocator();
      cellId= cellLocator->FindCell(x);

      vtkSmartPointer<vtkDijkstraGraphGeodesicPath> dijkstra =
        vtkSmartPointer<vtkDijkstraGraphGeodesicPath>::New();
      dijkstra->SetInputData(armPolydata);
      dijkstra->SetStartVertex(markedPoints->GetId(markedPoints->GetNumberOfIds()-1));
      dijkstra->SetEndVertex(armPolydata->GetCell(cellId)->GetPointIds()->GetId(0));
      markedPoints->InsertNextId(armPolydata->GetCell(cellId)->GetPointIds()->GetId(0));
      dijkstra->Update();

      vtkSmartPointer<vtkTubeFilter> tube = vtkSmartPointer<vtkTubeFilter>::New();
      tube->SetInputData(dijkstra->GetOutput());
      tube->SetRadius(radius);
      tube->SetCapping(1);
      tube->Update();

      mitk::Surface::Pointer tube_surf = mitk::Surface::New();
      current_tube->AddInputData(tube->GetOutput());
      current_tube->Update();

      tube_surf->SetVtkPolyData(current_tube->GetOutput());
      current_tube_node->SetData(tube_surf);
      current_tube->Modified();
      mitk::RenderingManager::GetInstance()->RequestUpdateAll();


      /*
      mitk::PointLocator::Pointer armLocator = mitk::PointLocator::New();
      armLocator->SetPoints(armPolydata);

      double tube_point[3];

      for (vtkIdType i=0; i < dijkstra->GetOutput()->GetNumberOfPoints(); i++)
      {
        dijkstra->GetOutput()->GetPoint(i,tube_point);
        colores->SetValue(armLocator->FindClosestPoint(tube_point),MARKING);

      }


      armPolydata->GetPointData()->SetScalars(colores);
      mitk::Surface::Pointer new_surf = mitk::Surface::New();
      new_surf->SetVtkPolyData(armPolydata);
      mOriginalArmNode->SetData(new_surf);
      mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
      mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
      mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
      mOriginalArmNode->SetBoolProperty("scalar visibility", true);
      mOriginalArmNode->SetBoolProperty("color mode", true);
      mOriginalArmNode->Modified();

      mitk::RenderingManager::GetInstance()->RequestUpdateAll();
      */

    }
  }
}
void FerulaInteractor::cancelMark(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{
  mitk::DataNode::Pointer pickedNode = mitk::DataNode::New();

  if (interactionEvent == nullptr)
  {
    MITK_ERROR << "No InteractionEvent" << std::endl;
    return;
  }

  const InteractionPositionEvent *positionEvent = dynamic_cast<const InteractionPositionEvent*>(interactionEvent);

  if (positionEvent == nullptr)
  {
    MITK_ERROR << "No InteractionPositionEvent" << std::endl;
    return;
  }



    // ** 3D Interaction ** //
  if (interactionEvent->GetSender()->GetName() == std::string("stdmulti.widget4"))
  {

    // Extract the position of the mouse in 2D and 3D
    Point2D coordinates = positionEvent->GetPointerPositionOnScreen();
    //cout<<coordinates<<endl;

    Point3D intersectionpoint;
    mitk::VtkPropRenderer::Pointer sender = static_cast<mitk::VtkPropRenderer*>(interactionEvent->GetSender());
    pickedNode = sender->PickObject(coordinates, intersectionpoint);

    //cout<<intersectionpoint<<endl;
    // Pick plane using hover in 3D window
   if (pickedNode == mOriginalArmNode )
   {
      mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
      if (surf.IsNull())
      {
        cout << "surf es null!!" << endl;
        return;
      }
        //if (!ThickPlane::IsPlane(pickedNode)) VER DE ADAPTARLO PARA CHEQUEAR SI ES STL
         // return;

          //if (pickedNode == mOriginalArmNode)
          //{

           // Locate cell
      vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
      armPolydata = surf->GetVtkPolyData();
      long n = armPolydata->GetNumberOfPoints();
      for (long i=0; i<n;i++)
      {
        if (colores->GetValue(i)== MARKING)
        {
          colores->SetValue(i,BASE);
        }
      }

      armPolydata->GetPointData()->SetScalars(colores);
      mitk::Surface::Pointer new_surf = mitk::Surface::New();
      new_surf->SetVtkPolyData(armPolydata);
      mOriginalArmNode->SetData(new_surf);
      mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
      mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
      mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
      mOriginalArmNode->SetBoolProperty("scalar visibility", true);
      mOriginalArmNode->SetBoolProperty("color mode", true);
      mOriginalArmNode->Modified();
      mitk::RenderingManager::GetInstance()->RequestUpdateAll();

   }
    return;
  }
  return;

}
void FerulaInteractor::lastPoint(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{
  mitk::DataNode::Pointer pickedNode = mitk::DataNode::New();

  if (interactionEvent == nullptr)
  {
    MITK_ERROR << "No InteractionEvent" << std::endl;
    return;
  }

  const InteractionPositionEvent *positionEvent = dynamic_cast<const InteractionPositionEvent*>(interactionEvent);

  if (positionEvent == nullptr)
  {
    MITK_ERROR << "No InteractionPositionEvent" << std::endl;
    return;
  }

  // ** 3D Interaction ** //
  if (interactionEvent->GetSender()->GetName() == std::string("stdmulti.widget4"))
  {

    Point2D coordinates = positionEvent->GetPointerPositionOnScreen();
    Point3D intersectionpoint;
    mitk::VtkPropRenderer::Pointer sender = static_cast<mitk::VtkPropRenderer*>(interactionEvent->GetSender());
    pickedNode = sender->PickObject(coordinates, intersectionpoint);
    if(pickedNode == mOriginalArmNode)
    {
      double x[3];
      x[0]=intersectionpoint[0];x[1]=intersectionpoint[1];x[2]=intersectionpoint[2];
      vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
      mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
      vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
      armPolydata = surf->GetVtkPolyData();
      vtkIdType cellId;
      cellLocator->SetDataSet(armPolydata);
      cellLocator->BuildLocator();
      cellId= cellLocator->FindCell(x);

      vtkSmartPointer<vtkDijkstraGraphGeodesicPath> dijkstra =
        vtkSmartPointer<vtkDijkstraGraphGeodesicPath>::New();
      dijkstra->SetInputData(armPolydata);
      dijkstra->SetStartVertex(markedPoints->GetId(markedPoints->GetNumberOfIds()-1));
      dijkstra->SetEndVertex(armPolydata->GetCell(cellId)->GetPointIds()->GetId(0));
      markedPoints->InsertNextId(armPolydata->GetCell(cellId)->GetPointIds()->GetId(0));
      dijkstra->Update();

      vtkSmartPointer<vtkTubeFilter> tube = vtkSmartPointer<vtkTubeFilter>::New();
      tube->SetInputData(dijkstra->GetOutput());
      tube->SetRadius(radius);
      tube->SetCapping(1);
      tube->Update();

      current_tube->AddInputData(tube->GetOutput());
      current_tube->Update();
      vtkSmartPointer<vtkPolyData> copy = vtkSmartPointer<vtkPolyData>::New();
      copy->DeepCopy(current_tube->GetOutput());

      ferula->AddInputData(copy);
      mitk::Surface::Pointer ferula_surf = mitk::Surface::New();
      ferula_surf->SetVtkPolyData(ferula->GetOutput());
      ferula_node->SetData(ferula_surf);
      ferula_node->Modified();
      mitk::RenderingManager::GetInstance()->RequestUpdateAll();
      //current_tube->RemoveAllInputs();


      /*
      mitk::PointLocator::Pointer armLocator = mitk::PointLocator::New();
      armLocator->SetPoints(armPolydata);

      double tube_point[3];

      for (vtkIdType i=0; i < dijkstra->GetOutput()->GetNumberOfPoints(); i++)
      {
        dijkstra->GetOutput()->GetPoint(i,tube_point);
        colores->SetValue(armLocator->FindClosestPoint(tube_point),FERULA);

      }
      long n = armPolydata->GetNumberOfPoints();
      for (long i=0; i<n;i++)
      {
        if (colores->GetValue(i)== MARKING)
        {
          colores->SetValue(i,FERULA);
        }
      }
      armPolydata->GetPointData()->SetScalars(colores);
      mitk::Surface::Pointer new_surf = mitk::Surface::New();
      new_surf->SetVtkPolyData(armPolydata);
      mOriginalArmNode->SetData(new_surf);
      mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
      mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
      mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
      mOriginalArmNode->SetBoolProperty("scalar visibility", true);
      mOriginalArmNode->SetBoolProperty("color mode", true);
      mOriginalArmNode->Modified();

      mitk::RenderingManager::GetInstance()->RequestUpdateAll();

      */

    }
  }
}
void FerulaInteractor::cancelPointSet(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{

}
void FerulaInteractor::cancelErase(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{

      for (vtkIdType i =0; i<erasingIDs->GetNumberOfIds();i++)
      {
        colores->SetValue(erasingIDs->GetId(i),FERULA);
      }

      for (vtkIdType i =0; i<erasingIDs->GetNumberOfIds();i++)
      {
        erasingIDs->Reset();
      }

      mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
      if (surf.IsNull())
      {
        cout << "surf es null!!" << endl;
        return;
      }
      vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
      armPolydata = surf->GetVtkPolyData();
      armPolydata->GetPointData()->SetScalars(colores);
      mitk::Surface::Pointer new_surf = mitk::Surface::New();
      new_surf->SetVtkPolyData(armPolydata);
      mOriginalArmNode->SetData(new_surf);
      mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
      mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
      mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
      mOriginalArmNode->SetBoolProperty("scalar visibility", true);
      mOriginalArmNode->SetBoolProperty("color mode", true);
      mOriginalArmNode->Modified();
      mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}
void FerulaInteractor::newMark(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{
  mitk::DataNode::Pointer pickedNode = mitk::DataNode::New();
  vtkIdList *marked_points;

  if (interactionEvent == nullptr)
  {
    MITK_ERROR << "No InteractionEvent" << std::endl;
    return;
  }

  const InteractionPositionEvent *positionEvent = dynamic_cast<const InteractionPositionEvent*>(interactionEvent);

  if (positionEvent == nullptr)
  {
    MITK_ERROR << "No InteractionPositionEvent" << std::endl;
    return;
  }



    // ** 3D Interaction ** //
  if (interactionEvent->GetSender()->GetName() == std::string("stdmulti.widget4"))
  {

    // Extract the position of the mouse in 2D and 3D
    Point2D coordinates = positionEvent->GetPointerPositionOnScreen();
    //cout<<coordinates<<endl;

    Point3D intersectionpoint;
    mitk::VtkPropRenderer::Pointer sender = static_cast<mitk::VtkPropRenderer*>(interactionEvent->GetSender());
    pickedNode = sender->PickObject(coordinates, intersectionpoint);

    //cout<<intersectionpoint<<endl;
    // Pick plane using hover in 3D window
   if (pickedNode == mOriginalArmNode)
   {
      mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
      if (surf.IsNull())
      {
        cout << "surf es null!!" << endl;
        return;
      }

      vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
      vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
      armPolydata = surf->GetVtkPolyData();

      if(lastCellID!=0)
      {
        marked_points = armPolydata->GetCell(lastCellID)->GetPointIds();
        long r = marked_points->GetNumberOfIds();
        if (r!=0)
        {
          for (long i=0; i<r;i++)
          {
            colores->SetValue(marked_points->GetId(i),MARKING);

          }
        }
      }
      armPolydata->GetPointData()->SetScalars(colores);
      mitk::Surface::Pointer new_surf = mitk::Surface::New();
      new_surf->SetVtkPolyData(armPolydata);
      mOriginalArmNode->SetData(new_surf);
      mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
      mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
      mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
      mOriginalArmNode->SetBoolProperty("scalar visibility", true);
      mOriginalArmNode->SetBoolProperty("color mode", true);
      mOriginalArmNode->Modified();
      mitk::RenderingManager::GetInstance()->RequestUpdateAll();

   }
    return;
  }
  return;
}
void FerulaInteractor::newErase(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{

  flag_interaction=true;
  vtkIdType cellId;
  vtkIdList *touched_points;
  vtkIdList *restoring_points;

  if (flag_interaction)
    {


    mitk::DataNode::Pointer pickedNode = mitk::DataNode::New();

    if (interactionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionEvent" << std::endl;
      return;
    }

    const InteractionPositionEvent *positionEvent = dynamic_cast<const InteractionPositionEvent*>(interactionEvent);

    if (positionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionPositionEvent" << std::endl;
      return;
    }



      // ** 3D Interaction ** //
    if (interactionEvent->GetSender()->GetName() == std::string("stdmulti.widget4"))
    {

      // Extract the position of the mouse in 2D and 3D
      Point2D coordinates = positionEvent->GetPointerPositionOnScreen();
      //cout<<coordinates<<endl;

      Point3D intersectionpoint;
      mitk::VtkPropRenderer::Pointer sender = static_cast<mitk::VtkPropRenderer*>(interactionEvent->GetSender());
      pickedNode = sender->PickObject(coordinates, intersectionpoint);

      //cout<<intersectionpoint<<endl;
      // Pick plane using hover in 3D window
     if (pickedNode == mOriginalArmNode)
     {
        mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(pickedNode->GetData());
        if (surf.IsNull())
        {
          cout << "surf es null!!" << endl;
          return;
        }
          //if (!ThickPlane::IsPlane(pickedNode)) VER DE ADAPTARLO PARA CHEQUEAR SI ES STL
           // return;

            //if (pickedNode == mOriginalArmNode)
            //{

             // Locate cell
        vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
        vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
        armPolydata = surf->GetVtkPolyData();

        cellLocator->SetDataSet(armPolydata);
        cellLocator->BuildLocator();
        double x[3];
        x[0]=intersectionpoint[0];x[1]=intersectionpoint[1];x[2]=intersectionpoint[2];
        cellId= cellLocator->FindCell(x);

        touched_points = armPolydata->GetCell(cellId)->GetPointIds();
        long n = touched_points->GetNumberOfIds();
        if (n==0)
        {
          return;
        }

        for (long i=0; i<n;i++)
        {

              colores->SetValue(touched_points->GetId(i),BASE);
              erasingIDs->InsertNextId(touched_points->GetId(i));

        }
        if(lastCellID!=0)
        {
          restoring_points = armPolydata->GetCell(lastCellID)->GetPointIds();
          long r = restoring_points->GetNumberOfIds();
          if (r!=0)
          {
            for (long i=0; i<r;i++)
            {

                colores->SetValue(restoring_points->GetId(i),BASE);
                erasingIDs->InsertNextId(restoring_points->GetId(i));

            }
          }
        }
        lastCellID = cellId;
        armPolydata->GetPointData()->SetScalars(colores);
        mitk::Surface::Pointer new_surf = mitk::Surface::New();
        new_surf->SetVtkPolyData(armPolydata);
        mOriginalArmNode->SetData(new_surf);
        mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
        mOriginalArmNode->SetBoolProperty("scalar visibility", true);
        mOriginalArmNode->SetBoolProperty("color mode", true);
        mOriginalArmNode->Modified();
        mitk::RenderingManager::GetInstance()->RequestUpdateAll();

     }
      return;
    }
    return;
  }
}
void FerulaInteractor::displayMark(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{
  flag_interaction=true;
  vtkIdType cellId;
  vtkIdList *touched_points;

  if (flag_interaction)
    {


    mitk::DataNode::Pointer pickedNode = mitk::DataNode::New();

    if (interactionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionEvent" << std::endl;
      return;
    }

    const InteractionPositionEvent *positionEvent = dynamic_cast<const InteractionPositionEvent*>(interactionEvent);

    if (positionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionPositionEvent" << std::endl;
      return;
    }



      // ** 3D Interaction ** //
    if (interactionEvent->GetSender()->GetName() == std::string("stdmulti.widget4"))
    {

      // Extract the position of the mouse in 2D and 3D
      Point2D coordinates = positionEvent->GetPointerPositionOnScreen();
      Point3D intersectionpoint;
      mitk::VtkPropRenderer::Pointer sender = static_cast<mitk::VtkPropRenderer*>(interactionEvent->GetSender());
      pickedNode = sender->PickObject(coordinates, intersectionpoint);

      // Pick plane using hover in 3D window
     if (pickedNode == mOriginalArmNode)
     {
        mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
        if (surf.IsNull())
        {
          cout << "surf es null!!" << endl;
          return;
        }

        vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
        vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
        armPolydata = surf->GetVtkPolyData();

        cellLocator->SetDataSet(armPolydata);
        cellLocator->BuildLocator();
        double x[3];
        x[0]=intersectionpoint[0];x[1]=intersectionpoint[1];x[2]=intersectionpoint[2];
        cellId= cellLocator->FindCell(x);

        touched_points = armPolydata->GetCell(cellId)->GetPointIds();
        long n = touched_points->GetNumberOfIds();
        if (n==0)
        {
          return;
        }

        for (long i=0; i<n;i++)
        {
          colores->SetValue(touched_points->GetId(i),MARKING);

        }

        armPolydata->GetPointData()->SetScalars(colores);
        mitk::Surface::Pointer new_surf = mitk::Surface::New();
        new_surf->SetVtkPolyData(armPolydata);
        mOriginalArmNode->SetData(new_surf);
        mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
        mOriginalArmNode->SetBoolProperty("scalar visibility", true);
        mOriginalArmNode->SetBoolProperty("color mode", true);
        mOriginalArmNode->Modified();
        }
        mitk::RenderingManager::GetInstance()->RequestUpdateAll();

     }
      return;
    }
    return;
}
void FerulaInteractor::defineMark(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{
    mitk::DataNode::Pointer pickedNode = mitk::DataNode::New();

    if (interactionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionEvent" << std::endl;
      return;
    }

    const InteractionPositionEvent *positionEvent = dynamic_cast<const InteractionPositionEvent*>(interactionEvent);

    if (positionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionPositionEvent" << std::endl;
      return;
    }



      // ** 3D Interaction ** //
    if (interactionEvent->GetSender()->GetName() == std::string("stdmulti.widget4"))
    {

      // Extract the position of the mouse in 2D and 3D
      Point2D coordinates = positionEvent->GetPointerPositionOnScreen();
      //cout<<coordinates<<endl;

      Point3D intersectionpoint;
      mitk::VtkPropRenderer::Pointer sender = static_cast<mitk::VtkPropRenderer*>(interactionEvent->GetSender());
      pickedNode = sender->PickObject(coordinates, intersectionpoint);

      //cout<<intersectionpoint<<endl;
      // Pick plane using hover in 3D window
     if (pickedNode == mOriginalArmNode)
     {

        mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
        if (surf.IsNull())
        {
          cout << "surf es null!!" << endl;
          return;
        }

        vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
        armPolydata = surf->GetVtkPolyData();
        long n = armPolydata->GetNumberOfPoints();
        for (long i=0; i<n;i++)
        {
          if (colores->GetValue(i)== MARKING)
          {
            colores->SetValue(i,FERULA);
          }
        }

        armPolydata->GetPointData()->SetScalars(colores);
        mitk::Surface::Pointer new_surf = mitk::Surface::New();
        new_surf->SetVtkPolyData(armPolydata);
        mOriginalArmNode->SetData(new_surf);
        mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
        mOriginalArmNode->SetBoolProperty("scalar visibility", true);
        mOriginalArmNode->SetBoolProperty("color mode", true);
        mOriginalArmNode->Modified();
        mitk::RenderingManager::GetInstance()->RequestUpdateAll();

     }
      return;
    }
    return;


}
void FerulaInteractor::displayErase(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{
  flag_interaction=true;
  vtkIdType cellId;
  vtkIdList *touched_points;

  if (flag_interaction)
    {


    mitk::DataNode::Pointer pickedNode = mitk::DataNode::New();

    if (interactionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionEvent" << std::endl;
      return;
    }

    const InteractionPositionEvent *positionEvent = dynamic_cast<const InteractionPositionEvent*>(interactionEvent);

    if (positionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionPositionEvent" << std::endl;
      return;
    }



      // ** 3D Interaction ** //
    if (interactionEvent->GetSender()->GetName() == std::string("stdmulti.widget4"))
    {

      // Extract the position of the mouse in 2D and 3D
      Point2D coordinates = positionEvent->GetPointerPositionOnScreen();
      //cout<<coordinates<<endl;

      Point3D intersectionpoint;
      mitk::VtkPropRenderer::Pointer sender = static_cast<mitk::VtkPropRenderer*>(interactionEvent->GetSender());
      pickedNode = sender->PickObject(coordinates, intersectionpoint);

      //cout<<intersectionpoint<<endl;
      // Pick plane using hover in 3D window
     if (pickedNode == mOriginalArmNode)
     {
        mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(pickedNode->GetData());
        if (surf.IsNull())
        {
          cout << "surf es null!!" << endl;
          return;
        }
          //if (!ThickPlane::IsPlane(pickedNode)) VER DE ADAPTARLO PARA CHEQUEAR SI ES STL
           // return;

            //if (pickedNode == mOriginalArmNode)
            //{

             // Locate cell
        vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
        vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
        armPolydata = surf->GetVtkPolyData();

        cellLocator->SetDataSet(armPolydata);
        cellLocator->BuildLocator();
        double x[3];
        x[0]=intersectionpoint[0];x[1]=intersectionpoint[1];x[2]=intersectionpoint[2];
        cellId= cellLocator->FindCell(x);

        touched_points = armPolydata->GetCell(cellId)->GetPointIds();
        long n = touched_points->GetNumberOfIds();
        if (n==0)
        {
          return;
        }

        for (long i=0; i<n;i++)
        {
          if (colores->GetValue(touched_points->GetId(i)) == FERULA)
          {
              colores->SetValue(touched_points->GetId(i),BASE);
              erasingIDs->InsertNextId(touched_points->GetId(i));
          }
        }
        lastCellID = cellId;
        armPolydata->GetPointData()->SetScalars(colores);
        mitk::Surface::Pointer new_surf = mitk::Surface::New();
        new_surf->SetVtkPolyData(armPolydata);
        mOriginalArmNode->SetData(new_surf);
        mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
        mOriginalArmNode->SetBoolProperty("scalar visibility", true);
        mOriginalArmNode->SetBoolProperty("color mode", true);
        mOriginalArmNode->Modified();
        mitk::RenderingManager::GetInstance()->RequestUpdateAll();

     }
      return;
    }
    return;
  }

}
void FerulaInteractor::defineErase(mitk::StateMachineAction*, mitk::InteractionEvent* interactionEvent)
{
    erasingIDs->Reset();
}
void FerulaInteractor::hover(StateMachineAction*, InteractionEvent* interactionEvent)
{
  flag_interaction=true;
  vtkIdType cellId;
  vtkIdList *touched_points;
  vtkIdList *restoring_points;
  if (flag_interaction)
    {


    mitk::DataNode::Pointer pickedNode = mitk::DataNode::New();

    if (interactionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionEvent" << std::endl;
      return;
    }

    const InteractionPositionEvent *positionEvent = dynamic_cast<const InteractionPositionEvent*>(interactionEvent);

    if (positionEvent == nullptr)
    {
      MITK_ERROR << "No InteractionPositionEvent" << std::endl;
      return;
    }



      // ** 3D Interaction ** //
    if (interactionEvent->GetSender()->GetName() == std::string("stdmulti.widget4"))
    {

      // Extract the position of the mouse in 2D and 3D
      Point2D coordinates = positionEvent->GetPointerPositionOnScreen();
      //cout<<coordinates<<endl;

      Point3D intersectionpoint;
      mitk::VtkPropRenderer::Pointer sender = static_cast<mitk::VtkPropRenderer*>(interactionEvent->GetSender());
      pickedNode = sender->PickObject(coordinates, intersectionpoint);

      //cout<<intersectionpoint<<endl;
      // Pick plane using hover in 3D window
      if(pickedNode == mOriginalArmNode)
      {
        mitk::Surface::Pointer surf0 = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
        if (surf0.IsNull())
        {
          cout << "surf es null!!" << endl;
          return;
        }
          //if (!ThickPlane::IsPlane(pickedNode)) VER DE ADAPTARLO PARA CHEQUEAR SI ES STL
           // return;

            //if (pickedNode == mOriginalArmNode)
            //{

             // Locate cell
        vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
        mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
        vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
        armPolydata = surf->GetVtkPolyData();

        cellLocator->SetDataSet(armPolydata);
        cellLocator->BuildLocator();
        double x[3];
        x[0]=intersectionpoint[0];x[1]=intersectionpoint[1];x[2]=intersectionpoint[2];
        cellId= cellLocator->FindCell(x);

        if(lastCellID!=0)
        {
          restoring_points = armPolydata->GetCell(lastCellID)->GetPointIds();
          long r = restoring_points->GetNumberOfIds();
          if (r!=0)
          {
            for (long i=0; i<r;i++)
            {
              if (colores->GetValue(restoring_points->GetId(i)) != FERULA)
              {
                colores->SetValue(restoring_points->GetId(i),BASE);
              }
            }
          }
        }

        touched_points = armPolydata->GetCell(cellId)->GetPointIds();
        long n = touched_points->GetNumberOfIds();
        if (n==0)
        {
          return;
        }

        for (long i=0; i<n;i++)
        {
          if (colores->GetValue(touched_points->GetId(i)) == BASE)
          {
            colores->SetValue(touched_points->GetId(i),HIGHLIGHTBASE);
          }

        }
        lastCellID=cellId;
        armPolydata->GetPointData()->SetScalars(colores);
        mitk::Surface::Pointer new_surf = mitk::Surface::New();
        new_surf->SetVtkPolyData(armPolydata);
        mOriginalArmNode->SetData(new_surf);
        mOriginalArmNode->SetProperty("LookupTable", mitk::LookupTableProperty::New(mitkLut));
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMinimum", 0);
        mOriginalArmNode->SetFloatProperty("ScalarsRangeMaximum", 255);
        mOriginalArmNode->SetBoolProperty("scalar visibility", true);
        mOriginalArmNode->SetBoolProperty("color mode", true);
        mOriginalArmNode->Modified();
        mitk::RenderingManager::GetInstance()->RequestUpdateAll();

     }
      return;
    }
    return;
  }
}
//--------------------------
// T R I D I M E N S I O N A R
void FerulaInteractor::toImageData(vtkSmartPointer<vtkPolyData> pd, vtkSmartPointer<vtkImageData> img)
{

  vtkSmartPointer<vtkImageData> whiteImage =
    vtkSmartPointer<vtkImageData>::New();
  double bounds[6];
  pd->GetBounds(bounds);
  double spacing[3]; // desired volume spacing
  spacing[0] = 0.5;
  spacing[1] = 0.5;
  spacing[2] = 0.5;
  whiteImage->SetSpacing(spacing);

  // compute dimensions
  int dim[3];
  for (int i = 0; i < 3; i++)
    {
    dim[i] = static_cast<int>(ceil((bounds[i * 2 + 1] - bounds[i * 2]) / spacing[i]));
    }
  whiteImage->SetDimensions(dim);
  whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

  double origin[3];
  origin[0] = bounds[0] + spacing[0] / 2;
  origin[1] = bounds[2] + spacing[1] / 2;
  origin[2] = bounds[4] + spacing[2] / 2;
  whiteImage->SetOrigin(origin);

  whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR,1);

  // fill the image with foreground voxels:
  unsigned char inval = 255;
  unsigned char outval = 0;
  vtkIdType count = whiteImage->GetNumberOfPoints();
  for (vtkIdType i = 0; i < count; ++i)
    {
    whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
    }

  // polygonal data --> image stencil:
  vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc =
    vtkSmartPointer<vtkPolyDataToImageStencil>::New();

  pol2stenc->SetInputData(pd);
  pol2stenc->SetOutputOrigin(origin);
  pol2stenc->SetOutputSpacing(spacing);
  pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
  pol2stenc->Update();

  // cut the corresponding white image and set the background:
  vtkSmartPointer<vtkImageStencil> imgstenc =
    vtkSmartPointer<vtkImageStencil>::New();

  imgstenc->SetInputData(whiteImage);
  imgstenc->SetStencilConnection(pol2stenc->GetOutputPort());
  imgstenc->ReverseStencilOff();
  imgstenc->SetBackgroundValue(outval);
  imgstenc->Update();
  img = imgstenc->GetOutput();


  vtkSmartPointer<vtkDataSetMapper> mapper =
    vtkSmartPointer<vtkDataSetMapper>::New();

  mapper->SetInputData(imgstenc->GetOutput());

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();

  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();

  // Add both renderers to the window
  renderWindow->AddRenderer(renderer);

  // Add a sphere to the left and a cube to the right
  renderer->AddActor(actor);

  renderer->ResetCamera();

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Start();

}

void FerulaInteractor::getArmShell(vtkPolyData *arm)
{

  vtkSmartPointer<vtkPolyData> big_arm = vtkSmartPointer<vtkPolyData>::New();
  big_arm->DeepCopy(arm);

  mitk::Surface::Pointer surf = mitk::Surface::New();
  surf->SetVtkPolyData(big_arm);
  mitk::DataNode::Pointer new_node = mitk::DataNode::New();
  new_node->SetBoolProperty("helper object",false);
  new_node->SetName("Big arm");
  new_node->SetData(surf);


  mDataStorage->Add(new_node);
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();


}
void FerulaInteractor::adjustToArm()
{

  vtkSmartPointer<vtkImageData> arm_img = vtkSmartPointer<vtkImageData>::New();
  mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
  vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
  armPolydata = surf->GetVtkPolyData();
  toImageData(armPolydata,arm_img);

  mitk::DataNode::Pointer arm_img_node = mitk::DataNode::New();
  mitk::DataNode::Pointer ferula_img_node = mitk::DataNode::New();

  vtkSmartPointer<vtkImageData> ferula_img = vtkSmartPointer<vtkImageData>::New();
  toImageData(current_tube->GetOutput(),ferula_img);
  //toImageData(armPolydata,ferula_img);


  vtkSmartPointer<vtkImageLogic> boolean = vtkSmartPointer<vtkImageLogic>::New();
  boolean->SetInput1Data(ferula_img);
  boolean->SetInput2Data(arm_img);
  boolean->SetOperationToAnd();
  boolean->Update();

  vtkSmartPointer<vtkDataSetMapper> mapper =
    vtkSmartPointer<vtkDataSetMapper>::New();

  mapper->SetInputData(boolean->GetOutput());

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();

  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();

  // Add both renderers to the window
  renderWindow->AddRenderer(renderer);

  // Add a sphere to the left and a cube to the right
  renderer->AddActor(actor);

  renderer->ResetCamera();

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->Render();
  renderWindowInteractor->Start();


  vtkSmartPointer<vtkImageDataGeometryFilter> imageDataGeometryFilter =
    vtkSmartPointer<vtkImageDataGeometryFilter>::New();
  imageDataGeometryFilter->SetInputData(boolean->GetOutput());
  imageDataGeometryFilter->Update();
  vtkSmartPointer<vtkPolyData> boolean_pd = vtkSmartPointer<vtkPolyData>::New();
  boolean_pd = imageDataGeometryFilter->GetOutput();
  cout<<"Number of image points: "<<boolean_pd->GetNumberOfPoints()<<endl;

  mitk::Surface::Pointer new_surf = mitk::Surface::New();
  new_surf->SetVtkPolyData(imageDataGeometryFilter->GetOutput());
  current_tube_node->SetData(new_surf);
  current_tube_node->SetColor(0.,255.,0.);
  current_tube_node->Modified();
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();




}


void FerulaInteractor::createTubes(mitk::DataNode::Pointer tube_node)
{
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  mitk::Surface::Pointer surf = dynamic_cast<mitk::Surface*>(mOriginalArmNode->GetData());
  vtkSmartPointer <vtkPolyData> armPolydata = vtkSmartPointer <vtkPolyData> ::New();
  armPolydata = surf->GetVtkPolyData();
  long n = armPolydata->GetNumberOfPoints();
  vtkIdType j=0;
  double point[3];
  points->SetNumberOfPoints(n);
  for (long i=0; i<n;i++)
  {
    if (colores->GetValue(i)==FERULA)
    {

      armPolydata->GetPoint(i,point);
      points->SetPoint(j,point[0],point[1],point[2]);
      j++;
    }
  }
  cout<<j<<endl;

  vtkSmartPointer<vtkParametricSpline> spline = vtkSmartPointer<vtkParametricSpline>::New();
  spline->SetPoints(points);
  vtkSmartPointer<vtkParametricFunctionSource> functionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
  functionSource->SetParametricFunction(spline);
  functionSource->SetUResolution(1 * int(j));
  functionSource->Update();


  vtkSmartPointer<vtkTubeFilter> tube = vtkSmartPointer<vtkTubeFilter>::New();
  tube->SetInputData(functionSource->GetOutput());
  tube->SetCapping(1);
  tube->Update();
  mitk::Surface::Pointer tube_surf = mitk::Surface::New();
  tube_surf->SetVtkPolyData(tube->GetOutput());
  tube_node->SetData(tube_surf);

}





