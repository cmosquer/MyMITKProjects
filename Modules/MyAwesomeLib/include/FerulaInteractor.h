#ifndef FERULAINTERACTOR_H
#define FERULAINTERACTOR_H

#include <QObject>
#include <string.h>

#include <vtkSmartPointer.h>
#include <vtkCellLocator.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkAppendPolyData.h>
#include <vtkImageData.h>

#include <itkObject.h>
#include <itkSmartPointer.h>
#include <itkObjectFactory.h>

#include <mitkCommon.h>
#include <mitkDataNode.h>
#include <mitkDataInteractor.h>
#include <mitkLookupTable.h>
#include <QmitkRenderWindow.h>

#include <QmitkRenderWindow.h>

#include "MyAwesomeLibExports.h"

class MyAwesomeLib_EXPORT FerulaInteractor: public QObject, public mitk::DataInteractor
{

  Q_OBJECT

public:
    mitkClassMacro(FerulaInteractor, DataInteractor)
    itkNewMacro(FerulaInteractor)



   void SetDataNode(mitk::DataNode* dataNode);
   void SetArmNode(mitk::DataNode* dataNode);
   void SetDataStorage(mitk::DataStorage::Pointer ds);
   void createTubes(mitk::DataNode::Pointer tube_node);
   void SetRadiusPointer(int radius);
   void toImageData(vtkSmartPointer<vtkPolyData> pd, vtkSmartPointer<vtkImageData> img);
   void adjustToArm();


public slots:


signals:


protected:
   FerulaInteractor();
   virtual ~FerulaInteractor();

    // Here actions strings from the loaded state machine pattern are mapped to functions of
    // the DataInteractor. These functions are called when an action from the state machine pattern is executed.

    virtual void ConnectActionsAndFunctions();



    // This function is called when a DataNode has been set/changed.
    // It is used to initialize the DataNode, e.g. if no PointSet exists yet it is created
    // and added to the DataNode.

    void hover(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void newMark(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void newErase(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void displayMark(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void defineMark(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void displayErase(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void defineErase(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void firstPoint(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void middlePoint(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void lastPoint(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void cancelMark(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void cancelPointSet(mitk::StateMachineAction*, mitk::InteractionEvent*);
    void cancelErase(mitk::StateMachineAction*, mitk::InteractionEvent*);

    void getArmShell(vtkPolyData *arm);



private:
    mitk::DataStorage::Pointer              mDataStorage;
    mitk::DataNode::Pointer                 mGhostNode;
    mitk::DataNode::Pointer                 mOriginalArmNode;
    mitk::DataNode::Pointer                 mFerulaNode;
    vtkIdType                              lastCellID;
    vtkSmartPointer<vtkDoubleArray>         colores;
    mitk::LookupTable::Pointer              mitkLut;
    vtkSmartPointer<vtkIdList>  markedPoints;
    vtkSmartPointer<vtkIdList> erasingIDs;
    bool flag_interaction;
    double last_color;
    mitk::DataNode::Pointer ferula_node;
    mitk::DataNode::Pointer current_tube_node;
    vtkSmartPointer<vtkAppendPolyData> ferula;
    vtkSmartPointer<vtkAppendPolyData> current_tube;
    double radius;


};

#endif // FERULAINTERACTOR_H
