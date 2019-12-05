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

#include "GamePlay.h"
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCubeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkMitkRectangleProp.h>

#include "usGetModuleContext.h"
#include "usModuleContext.h"

#include <mitkDataNode.h>
#include <mitkImage.h>
#include <mitkDataStorage.h>
#include <mitkStandaloneDataStorage.h>
#include <mitkPointSetDataInteractor.h>
#include <mitkRenderWindow.h>
#include <mitkBaseRenderer.h>
#include <mitkDisplayInteractor.h>

#include <QGridLayout>
#include <QLabel>
#include <QVTKOpenGLWidget.h>
//#include <QVTKWidget.h>
#include <QmitkRenderWindow.h>
#include <QMediaPlayer>
#include <QmitkSliceWidget.h>
#include<mitkNodePredicateDataType.h>
#include <string>


void GamePlay::playClicked(){
  music->play();
  music_loaded=true;
}
GamePlay::GamePlay( QWidget * parent) : QDialog(parent)
{
  setupUi(this);

  music = new QMediaPlayer();
  const QUrl url = QUrl("file://home/cande/work/MyMITKProjects/Plugins/my.awesomeproject.gameplugin/resources/gamemusic.mp3");
  music->setMedia(url);
  connect(music, &QMediaPlayer::mediaStatusChanged,
          this, [&](QMediaPlayer::MediaStatus status){
      if(status == QMediaPlayer::LoadedMedia) SLOT(playClicked());
  });
  cout<<music->mediaStatus()<<endl;
  cout<<QMediaPlayer::MediaStatus::LoadedMedia<<endl;
    /*
  while(music->mediaStatus() != QMediaPlayer::MediaStatus::LoadedMedia)
  {
    cout<<"loaded: "<<music->mediaStatus()<<endl;
    cout<<"error: "<<music->error()<<endl;
    sleep(10);
  }
    */
  connect(pbStart, SIGNAL (clicked()),this, SLOT(onStart()));
  pbItemEnd->hide();
  lblResultado->hide();
  teScore->hide();
  labelScore->hide();
  lblResultado->setWordWrap(true);
  connect(pbItemEnd,SIGNAL (clicked()),this, SLOT(onNextItem()));
}

void clearWidgets(QLayout * layout) {
   if (! layout)
      return;
   while (auto item = layout->takeAt(0)) {
      delete item->widget();
      clearWidgets(item->layout());
   }
}
void GamePlay::onClickOption()
{
  QObject* obj = sender();
  QPushButton* w = qobject_cast<QPushButton*>(sender());

  for (int i = 0; i < options_layout->count(); ++i)
  {
    QPushButton *widget = qobject_cast<QPushButton*>(options_layout->itemAt(i)->widget());
    if (widget != NULL)
    {
      widget->setFlat(true);
    }
  }
  w->setFlat(false);
}
void GamePlay::exit()
{
  this->close();
}
void GamePlay::onItemEnd()
{
    QObject* button = QObject::sender();
    if(button==m_correct_answer)
    {
     current_score = current_score + current_item_score;
     std::string s =std::to_string(current_item_score);
     std::string result = "¡Correcto! Sumás " + s;
     lblResultado->setText(QString(result.c_str()));
     lblResultado->setAlignment(Qt::AlignCenter);
    }
    else
    {
     lblResultado->setText("Incorrecto");
     lblResultado->setAlignment(Qt::AlignCenter);
    }

  QString score(std::to_string(current_score).c_str());

  teScore->setText(score);
  teScore->setAlignment(Qt::AlignCenter);

  pbItemEnd->setEnabled(true);
  pbItemEnd->show();

  lblResultado->show();
  questionsWidget->hide();
  optionsWidget->hide();
  scoreWidget->setGeometry(400,400,400,200);

}

void GamePlay::onNextItem()
{
  if (current_item<N_items)
  {
    pbItemEnd->hide();
    pbItemEnd->hide();
    lblResultado->hide();
    scoreWidget->setGeometry(10,820,400,200);
    current_item++;
    questionsWidget->show();
    optionsWidget->show();
    PlayItem(current_item);
  }
  else
  {
    clearWidgets(question_layout);
    clearWidgets(options_layout);
    labelScore->setText("FINAL SCORE");
    labelScore->setGeometry(400,340,211,31);
    teScore->setGeometry(505,380,104,70);
    lblResultado->hide();
    pbItemEnd->setText("Fin");
    pbItemEnd->setGeometry(411,500,131,51);
    connect(pbItemEnd, SIGNAL (clicked()),this, SLOT(exit()));
    current_score =0;
  }

}

void GamePlay::SetDataStorage(mitk::DataStorage::Pointer ds)
{
    m_dataStorage = ds;
}

void GamePlay::SetNumberOfItems(unsigned long N)
{
  N_items = N;
}

void GamePlay::SetItemWidgetInLayout(mitk::DataNode::Pointer itemNode, QLayout *layout)
{

  bool isquestion = false;
  itemNode->GetBoolProperty("medicas.gaming.isQuestion", isquestion);

  int n_nodes;
  itemNode->GetIntProperty("medicas.gaming.NumberOfNodes", n_nodes);

  std::string text;
  itemNode->GetStringProperty("medicas.gaming.text",text);
  if(isquestion)
  {

    //lbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    lbl->setWordWrap(true);
    lbl->setText(text.c_str());
    QFont questionfont;
    questionfont.setPointSize(12);
    lbl->setFont(questionfont);
    //lbl->setParent(questionsWidget);
    //lbl->setGeometry(10,350,200,200);
    //layout->setSizeConstraint(QLayout::SetMinimumSize);
    questionTitle_layout->update();
  }

  if (n_nodes>0)
  {
    cout<<"n nodes"<<n_nodes<<endl;
      mitk::StandaloneDataStorage::Pointer ds = mitk::StandaloneDataStorage::New();
    int view;
    itemNode->GetIntProperty("medicas.gaming.view",view);
    QmitkRenderWindow *widget3D = new QmitkRenderWindow(this->parentWidget());
    QmitkSliceWidget *widget_slice = new QmitkSliceWidget(this->parentWidget());
    QmitkRenderWindow *mitkWidget1 = new QmitkRenderWindow(this->parentWidget());
    //ADD ALL NODES TO NEW DATA STORAGE
    for (int i = 0; i<n_nodes; i++)
    {
      std::string prop_name = "medicas.gaming.nodo" + std::to_string(i);
      std::string prop_name_color_R = "medicas.gaming.color_nodo_R" + std::to_string(i);
      std::string prop_name_color_G = "medicas.gaming.color_nodo_G" + std::to_string(i);
      std::string prop_name_color_B = "medicas.gaming.color_nodo_B" + std::to_string(i);
      std::string prop_opacity = "medicas.gaming.opacity" + std::to_string(i);

      std::string node_name;
      itemNode->GetStringProperty(prop_name.c_str(),node_name);
      float col[3];
      itemNode->GetFloatProperty(prop_name_color_R.c_str(),col[0]);
      itemNode->GetFloatProperty(prop_name_color_G.c_str(),col[1]);
      itemNode->GetFloatProperty(prop_name_color_B.c_str(),col[2]);
      mitk::Color colmitk;
      colmitk.SetRed(col[0]);
      colmitk.SetGreen(col[1]);
      colmitk.SetBlue(col[2]);
      mitk::DataNode::Pointer node = mitk::DataNode::New();
      mitk::DataNode *to_copy =m_dataStorage->GetNamedNode(node_name);
      if (to_copy)
      {
        node->SetData(to_copy->GetData());
        node->SetProperty("opacity",to_copy->GetProperty(prop_opacity.c_str()));
        node->SetColor(colmitk);
        node->SetVisibility(true, widget3D->GetRenderer());
        node->SetVisibility(true, widget_slice->GetRenderer());
        node->SetVisibility(true, mitkWidget1->GetRenderer());
        ds->Add(node);
      }
    }


    //ADD DATA STORAGE TO A RENDER QT MITK WINDOW
    if (view==0)
    {
      widget3D->GetRenderer()->SetDataStorage(ds);
      widget3D->GetRenderer()->SetMapperID(mitk::BaseRenderer::Standard3D);
      widget3D->GetRenderer()->GetVtkRenderer()->ResetCamera();
      layout->addWidget(widget3D);
    }
    else
    {
      mitkWidget1->GetRenderer()->PrepareRender();
      mitkWidget1->GetRenderer()->SetDataStorage(ds);
      vtkSmartPointer<vtkMitkRectangleProp> m_RectangleRendering1;
      mitk::DataNode::Pointer m_PlaneNode1;
      if(view==1)
        mitkWidget1->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Sagittal);
      if(view==2)
        mitkWidget1->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Axial);
      if(view==3)
        mitkWidget1->GetSliceNavigationController()->SetDefaultViewDirection(mitk::SliceNavigationController::Frontal);
      mitk::SliceNavigationController *m_TimeNavigationController = nullptr;
      m_TimeNavigationController = mitk::RenderingManager::GetInstance()->GetTimeNavigationController();
      m_TimeNavigationController->ConnectGeometryTimeEvent(mitkWidget1->GetSliceNavigationController(), false);
      mitkWidget1->GetSliceNavigationController()->ConnectGeometryTimeEvent(m_TimeNavigationController, false);
      m_RectangleRendering1 = vtkSmartPointer<vtkMitkRectangleProp>::New();
      m_RectangleRendering1->SetColor(1.0, 0.0, 0.0);
      mitkWidget1->GetRenderer()->GetVtkRenderer()->AddViewProp(m_RectangleRendering1);
      vtkRenderer *vtkrenderer;


      //mitk::BaseRenderer::GetInstance(mitkWidget1->GetVtkRenderWindow())->GetCameraController()->Fit();
      int w = vtkObject::GetGlobalWarningDisplay();
      vtkObject::GlobalWarningDisplayOff();
      vtkrenderer = mitk::BaseRenderer::GetInstance(mitkWidget1->GetVtkRenderWindow())->GetVtkRenderer();

      if (vtkrenderer != nullptr)
          vtkrenderer->ResetCamera();

      // Tell the renderwindow which (part of) the datastorage to render
      cout<<"display interactor on"<<endl;
      mitk::DisplayInteractor::Pointer m_DisplayInteractor;
      m_DisplayInteractor = mitk::DisplayInteractor::New();
      m_DisplayInteractor->LoadStateMachine("DisplayInteraction.xml");
      m_DisplayInteractor->SetEventConfig("DisplayConfigMITK.xml");
      // Register as listener via micro services

      //us::ModuleContext *context = us::GetModuleContext();
      //context->RegisterService<mitk::InteractionEventObserver>(m_DisplayInteractor.GetPointer());
      cout<<"mapper id on"<<endl;

      mitkWidget1->GetRenderer()->SetMapperID(mitk::BaseRenderer::Standard2D);
      auto geo = ds->ComputeBoundingGeometry3D(ds->GetAll());
      mitk::RenderingManager::GetInstance()->InitializeViews(geo);

      ds->Print(std::cout);
      mitk::RenderingManager::GetInstance()->RequestUpdateAll();
      layout->addWidget(mitkWidget1);
    }
  }

  if (!isquestion)
 {
      QPushButton *pb = new QPushButton;
      QPalette pal = pb->palette();
      pal.setColor(QPalette::Button, QColor(Qt::gray));
      pb->setAutoFillBackground(true);
      pb->setPalette(pal);
      pb->update();
      connect(pb, SIGNAL (clicked()),this, SLOT(onItemEnd()));
      pb->setText(text.c_str());
      layout->addWidget(pb);
      connect(pb, SIGNAL (clicked()),this, SLOT(onClickOption()));
      bool iscorrect=false;
      itemNode->GetBoolProperty("medicas.gaming.isCorrect",iscorrect);
      if(iscorrect)
      {
        m_correct_answer = pb;
      }
  }

}





void GamePlay::PlayItem(int i)
{

  //questionsWidget->setVisible(true);
  //optionsWidget->setVisible(true);

  clearWidgets(question_layout);
  clearWidgets(options_layout);

  std::string question_name = "Pregunta" + std::to_string(i);
  mitk::DataNode::Pointer questionNode = m_dataStorage->GetNamedNode(question_name);
  if (questionNode)
  {
    std::string title = "Pregunta " + std::to_string(i);
    //lbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    labeltitle->setWordWrap(true);
    labeltitle->setText(title.c_str());
    QFont questiontitle;
    questiontitle.setPointSize(18);
    questiontitle.setBold(true);
    labeltitle->setFont(questiontitle);
    //labeltitle->setParent(questionsWidget);
    //labeltitle->setGeometry(10,10,200,100);
    //layout->setSizeConstraint(QLayout::SetMinimumSize);
    questionTitle_layout->update();
    questionNode->GetIntProperty("medicas.gaming.score",current_item_score);
    SetItemWidgetInLayout(questionNode, question_layout);



    bool stillMoreOptions = true;
    int j = 1;
    while (stillMoreOptions)
    {
      std::string option_name = "Respuesta" + std::to_string(i) + "-Opcion" + std::to_string(j);
      mitk::DataNode::Pointer optionNode = m_dataStorage->GetNamedNode(option_name);

      if (optionNode)
      {
        SetItemWidgetInLayout(optionNode,options_layout);
      }
      else
      {
        stillMoreOptions = false;
      }
      j++;
    }
  }
  else
  {
    return;
  }



}


void GamePlay::onStart()
{
  pbItemEnd->setEnabled(false);
  pbItemEnd->hide();
  teScore->show();
  labelScore->show();
  pbStart->hide();
  pbStart->setEnabled(false);
  QFont scorefont;
  scorefont.setPointSize(50);
  scorefont.setBold(true);

  teScore->setText("0");
  teScore->setStyleSheet("QLabel { background-color : black; color : white; }");
  teScore->setAlignment(Qt::AlignCenter);
  teScore->setWordWrap(true);

  current_score = 0;
  current_item =1 ;



  PlayItem(current_item);

  /*
  QVTKWidget *opcion1 = new QVTKWidget(this);
  options_widgets.push_back(opcion1);

  vtkSmartPointer<vtkSphereSource> sphereSource =
    vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->Update();
  vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkSmartPointer<vtkActor> sphereActor =
    vtkSmartPointer<vtkActor>::New();
  sphereActor->SetMapper(sphereMapper);


  QVTKWidget *opcion2 = new QVTKWidget(this);
  options_widgets.push_back(opcion2);

  vtkSmartPointer<vtkCubeSource> cubeSource =
    vtkSmartPointer<vtkCubeSource>::New();
  cubeSource->Update();
  vtkSmartPointer<vtkPolyDataMapper> cubeMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
  vtkSmartPointer<vtkActor> cubeActor =
    vtkSmartPointer<vtkActor>::New();
  cubeActor->SetMapper(cubeMapper);

  // VTK Renderer
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(sphereActor);

  vtkSmartPointer<vtkRenderer> renderercube =
    vtkSmartPointer<vtkRenderer>::New();
  renderercube->AddActor(cubeActor);

  // VTK/Qt wedded
  opcion1->GetRenderWindow()->AddRenderer(renderer);
  opcion2->GetRenderWindow()->AddRenderer(renderercube);
  options_layout->addWidget(opcion1);
  options_layout->addWidget(opcion2);

  QLabel *lbl = new QLabel;
  lbl->setText("Yo soy la pregunta");
  question_layout->addWidget(lbl);
  */
}
