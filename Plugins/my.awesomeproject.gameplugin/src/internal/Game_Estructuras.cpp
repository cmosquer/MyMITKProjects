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


#include "Game_Estructuras.h"

//#include "HighlightStructureInteractor.h"

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
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCubeSource.h>

#include <QMessageBox>
#include <QString>
#include <QSettings>
#include <QFileDialog>
#include <QTableWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QPlainTextEdit>

#include <mitkImageCast.h>
#include <mitkLookupTables.h>
#include <mitkNodePredicateDataType.h>
#include <mitkGeometryData.h>
#include <mitkIOUtil.h>

#define INICIO 0
#define CARGADO 1
#define ESCRIBIR_PREG 2
#define INICIO_RESPUESTA 3
#define ESCRIBIR_OPC 4
#define RESUMEN 5


using namespace std;

Game_Estructuras::Game_Estructuras()
{

}
Game_Estructuras::~Game_Estructuras()
{


}

const std::string Game_Estructuras::VIEW_ID = "my.awesomeproject.views.Game";


void Game_Estructuras::changeScreen(int scr)
{
  current_screen=scr;
  if (scr==INICIO)
  {
    m_Controls.cbEscenario->setChecked(false);
    m_Controls.labelTitulo->setText("Seleccione");
    QFont questiontitle;
    questiontitle.setPointSize(18);
    questiontitle.setBold(true);
    m_Controls.rbSI->setVisible(false);
    m_Controls.rbNO->setVisible(false);
    m_Controls.labelTitulo->setFont(questiontitle);

    m_Controls.pbCrear->setVisible(true);
    m_Controls.pbCargarJuego->setVisible(true);
    m_Controls.pbAtras->setVisible(false);
    m_Controls.pbConfirmar->setVisible(false);
    m_Controls.pbOtraOpcion->setVisible(false);
    m_Controls.pbEliminar->setVisible(false);
    m_Controls.pbMostrar->setVisible(false);
    m_Controls.labelScore->setVisible(false);
    m_Controls.indicacion->setVisible(false);
    m_Controls.lePreg->setVisible(false);
    m_Controls.leScore->setVisible(false);
    m_Controls.cbOpcion->setVisible(false);
    m_Controls.cbItem->setVisible(false);
    m_Controls.textResumen->setVisible(false);
    m_Controls.pregunta->setVisible(false);
    m_Controls.cbGenerico->setVisible(false);
    m_Controls.cbEscenario->setVisible(false);
    m_Controls.cbVistasPreg->setVisible(false);
    //m_Controls.aclaracion->setVisible(false);
    m_Controls.aclaracion->setText("             ");
    m_Controls.labelItems->setVisible(false);
    m_Controls.labelResumen->setVisible(false);


  }
  if(scr==CARGADO)
  {
    m_Controls.labelTitulo->setText("");

    m_Controls.pbCrear->setVisible(false);
    m_Controls.pbCargarJuego->setVisible(false);
    m_Controls.pbAtras->setVisible(true);
    m_Controls.pbAtras->setText("Atrás");
    m_Controls.pbConfirmar->setText("Jugar");
    m_Controls.pbConfirmar->setEnabled(true);
    m_Controls.pbConfirmar->setVisible(true);
    m_Controls.pbOtraOpcion->setVisible(false);
    m_Controls.labelScore->setVisible(false);
    m_Controls.indicacion->setVisible(false);
    m_Controls.lePreg->setVisible(false);
    m_Controls.leScore->setVisible(false);
    m_Controls.cbOpcion->setVisible(false);
    m_Controls.cbItem->setVisible(false);
    m_Controls.textResumen->setVisible(false);
    m_Controls.cbGenerico->setVisible(false);
    m_Controls.pbEliminar->setVisible(false);
    m_Controls.pbMostrar->setVisible(false);
    m_Controls.rbSI->setVisible(false);
    m_Controls.rbNO->setVisible(false);
    m_Controls.labelItems->setVisible(false);
    m_Controls.labelResumen->setVisible(false);
  }

  if(scr==ESCRIBIR_PREG)
  {
    std::string tit = "Pregunta "+ std::to_string(N_items);
    m_Controls.labelTitulo->setText(tit.c_str());    
    m_Controls.pbCrear->setVisible(false);
    m_Controls.pbCargarJuego->setVisible(false);
    m_Controls.pbEliminar->setVisible(false);
    m_Controls.pbMostrar->setVisible(false);
    m_Controls.indicacion->setVisible(true);
    m_Controls.indicacion->setText("Escriba la pregunta");
    m_Controls.lePreg->setVisible(true);
    m_Controls.lePreg->setEnabled(true);
    m_Controls.lePreg->setText("");
    m_Controls.aclaracion->setText("                ");

    m_Controls.cbGenerico->setVisible(false);
    m_Controls.cbGenerico->setChecked(false);

    m_Controls.pbAtras->setVisible(true);
    m_Controls.pbAtras->setText("Atrás");
    m_Controls.pbConfirmar->setText("Listo!");
    m_Controls.pbConfirmar->setEnabled(true);
    m_Controls.pbConfirmar->setVisible(true);
    m_Controls.pbOtraOpcion->setVisible(false);

    m_Controls.labelScore->setVisible(true);
    m_Controls.leScore->setVisible(true);
    m_Controls.leScore->setValue(100);
    m_Controls.labelScore->setText("Indique cuántos puntos se otorgan por responder bien esta pregunta.");

    m_Controls.rbSI->setVisible(false);
    m_Controls.rbNO->setVisible(false);

    m_Controls.cbOpcion->setVisible(false);
    m_Controls.cbItem->setVisible(false);
    m_Controls.textResumen->setVisible(false);
    m_Controls.pregunta->setVisible(false);
    m_Controls.cbEscenario->setChecked(false);
    m_Controls.cbEscenario->setVisible(true);
    m_Controls.cbVistasPreg->setVisible(true);
    m_Controls.cbVistasPreg->setEnabled(false);
    m_Controls.labelItems->setVisible(false);
    m_Controls.labelResumen->setVisible(false);
  }
  if(scr==INICIO_RESPUESTA)
  {
    std::string tit = "Respuesta "+ std::to_string(N_items);
    m_Controls.labelTitulo->setText(tit.c_str());

    m_Controls.pbCrear->setVisible(false);
    m_Controls.pbCargarJuego->setVisible(false);
    m_Controls.pbAtras->setVisible(true);
    m_Controls.pbAtras->setText("Atrás");
    m_Controls.pbConfirmar->setText("Crear primera opción de respuesta");
    m_Controls.pbConfirmar->setVisible(true);
    m_Controls.pbOtraOpcion->setVisible(false);
    m_Controls.pbEliminar->setVisible(false);
    m_Controls.pbMostrar->setVisible(false);

    m_Controls.labelScore->setVisible(false);
    m_Controls.indicacion->setVisible(true);
    m_Controls.indicacion->setText("A continuación, vamos a crear una por una las opciones \nmultiple-choice para esta pregunta.");
    m_Controls.lePreg->setVisible(false);
    m_Controls.leScore->setVisible(false);
    m_Controls.aclaracion->setText("                ");

    m_Controls.cbOpcion->setVisible(false);
    m_Controls.cbItem->setVisible(false);
    m_Controls.textResumen->setVisible(false);
    m_Controls.cbGenerico->setVisible(false);
    m_Controls.cbEscenario->setVisible(false);
    m_Controls.cbVistasPreg->setVisible(false);
    m_Controls.labelItems->setVisible(false);
    m_Controls.labelResumen->setVisible(false);

  }
  if(scr==ESCRIBIR_OPC)
  {
    m_Controls.cbGenerico->setChecked(false);

    std::string tit = "Respuesta "+ std::to_string(N_items) + "\nOpción " +  std::to_string(N_options[N_items-1]+1);
    m_Controls.labelTitulo->setText(tit.c_str());
    m_Controls.aclaracion->setText("                ");

    m_Controls.pbCrear->setVisible(false);
    m_Controls.pbCargarJuego->setVisible(false);
    m_Controls.pbAtras->setVisible(true);
    m_Controls.pbAtras->setText("Cancelar pregunta");
    m_Controls.pbConfirmar->setText("Listo, es la última opción");
    m_Controls.pbConfirmar->setVisible(true);
    m_Controls.pbOtraOpcion->setVisible(true);
    m_Controls.pbEliminar->setVisible(false);
    m_Controls.pbMostrar->setVisible(false);

    m_Controls.pregunta->setVisible(true);
    m_Controls.lePreg->setVisible(true);
    m_Controls.lePreg->setEnabled(true);
    m_Controls.cbEscenario->setVisible(true);
    m_Controls.cbVistasPreg->setVisible(true);
    m_Controls.cbVistasPreg->setEnabled(false);

    m_Controls.indicacion->setVisible(true);
    m_Controls.indicacion->setText("Escriba el texto para esta opción.");

    m_Controls.lePreg->setText("");
    m_Controls.cbGenerico->setEnabled(true);
    m_Controls.cbGenerico->setVisible(true);
    m_Controls.cbGenerico->setChecked(false);
    m_Controls.labelItems->setVisible(false);
    m_Controls.labelResumen->setVisible(false);
    m_Controls.cbEscenario->setChecked(false);
    m_Controls.cbEscenario->setEnabled(true);

    m_Controls.labelScore->setVisible(true);
    m_Controls.labelScore->setText("¿Esta es la opción correcta?");
    m_Controls.leScore->setVisible(false);
    m_Controls.rbNO->setVisible(true);
    m_Controls.rbNO->setAutoExclusive(false);
    m_Controls.rbNO->setChecked(false);
    m_Controls.rbNO->setAutoExclusive(true);
    m_Controls.rbSI->setVisible(true);
    m_Controls.rbSI->setAutoExclusive(false);
    m_Controls.rbSI->setChecked(false);
    m_Controls.rbSI->setAutoExclusive(true);
    m_Controls.cbOpcion->setVisible(false);
    m_Controls.cbItem->setVisible(false);
    m_Controls.textResumen->setVisible(false);

  }

  if(scr==RESUMEN)
  {

    std::string tit = "RESUMEN DE JUEGO";
    m_Controls.labelTitulo->setText(tit.c_str());
    m_Controls.cbEscenario->setChecked(false);
    m_Controls.cbEscenario->setVisible(false);
    m_Controls.cbVistasPreg->setVisible(false);
    m_Controls.pbCrear->setVisible(false);
    m_Controls.pbCargarJuego->setVisible(false);
    m_Controls.pbAtras->setVisible(true);
    m_Controls.pbAtras->setText("Agregar otra pregunta");
    m_Controls.pbConfirmar->setText("Jugar demo");
    m_Controls.pbConfirmar->setVisible(true);
    m_Controls.pbOtraOpcion->setVisible(false);
    m_Controls.pbEliminar->setVisible(true);
    m_Controls.pbMostrar->setVisible(true);
    m_Controls.cbOpcion->setVisible(true);
    m_Controls.labelScore->setVisible(false);
    m_Controls.indicacion->setVisible(false);
    m_Controls.pregunta->setVisible(false);
    m_Controls.lePreg->setVisible(false);
    m_Controls.leScore->setVisible(false);
    m_Controls.cbItem->setVisible(true);
    m_Controls.textResumen->setVisible(true);
    m_Controls.cbGenerico->setVisible(false);
    m_Controls.labelItems->setVisible(true);
    m_Controls.labelResumen->setVisible(true);
   // m_Controls.aclaracion->setVisible(false);
    m_Controls.aclaracion->setText("             ");

    m_Controls.rbSI->setVisible(false);
    m_Controls.rbNO->setVisible(false);
    onResumen();
  }
}


void Game_Estructuras::CreateQtPartControl(QWidget* parent)
{
  // Setting up the UI is a true pleasure when using .ui files, isn't it?
  m_Controls.setupUi(parent);

  connect(m_Controls.pbCrear, SIGNAL (clicked()),this, SLOT(onCrearJuego())); //Crear el juego
  connect(m_Controls.pbCargarJuego, SIGNAL (clicked()),this, SLOT(onCargarJuego()));
  //connect(m_Controls.pbAgregarItem, SIGNAL (clicked()),this, SLOT(onAgregarItem())); //Agregar una nueva pregunta
  connect(m_Controls.pbConfirmar, SIGNAL (clicked()),this, SLOT(onConfirmar())); //Confirmar la pregunta con los datos introducidos
  connect(m_Controls.pbOtraOpcion, SIGNAL (clicked()),this, SLOT(onAgregarOpcion())); //Si esta el RB de respuesta con opciones, agrega opcion segun datos introducidos
  connect(m_Controls.pbAtras, SIGNAL (clicked()),this, SLOT(onAtras()));
  connect(m_Controls.cbItem, SIGNAL(currentIndexChanged(int)), this, SLOT(onResumen()));
  connect(m_Controls.cbEscenario, SIGNAL(clicked()),this,SLOT(onToggle()));
  connect(m_Controls.cbGenerico,SIGNAL(clicked()),this,SLOT(onGenerico()));
  connect(m_Controls.pbMostrar,SIGNAL(clicked()),this,SLOT(onMostrar()));
  connect(m_Controls.pbEliminar,SIGNAL(clicked()),this,SLOT(onEliminarItem()));

  changeScreen(INICIO);

  nodesView = new QLineEdit;
  itemsView = new QHBoxLayout;

}
void Game_Estructuras::onGenerico()
{
    if (m_Controls.cbGenerico->isChecked())
    {
        m_standard_options_flag = true;
        m_Controls.lePreg->setEnabled(false);
    }
    else
    {
        m_standard_options_flag = false;
        m_Controls.lePreg->setEnabled(true);
    }

}

void Game_Estructuras::onToggle()
{
    if (m_Controls.cbEscenario->isChecked()||(!m_Controls.cbGenerico->isChecked()))
    {
        m_Controls.pbConfirmar->setEnabled(true);
    }
    if (m_Controls.cbEscenario->isChecked())
    {
        QFont aclaracion;
        aclaracion.setItalic(true);
        m_Controls.aclaracion->setFont(aclaracion);
        m_Controls.aclaracion->setText("Marque como visibles aquellos objetos que desea incluir en el escenario ");
        //m_Controls.aclaracion->setVisible(true);
        m_Controls.cbVistasPreg->setEnabled(true);
    }
    else
    {
        //m_Controls.aclaracion->setVisible(false);
        m_Controls.aclaracion->setText("                ");

        m_Controls.cbVistasPreg->setEnabled(false);

    }
}

void Game_Estructuras::onCargarJuego()
{


  QString fileName = QFileDialog::getOpenFileName(NULL, "Abrir archivo .mitk del juego");
  if (fileName.isNull())
    return;

  mitk::IOUtil::Load(fileName.toStdString(), *GetDataStorage());

  changeScreen(CARGADO);
  //Search for N_items
  mitk::DataStorage::SetOfObjects::ConstPointer allNodes = GetDataStorage()->GetAll();
  int max_item=0;
  for (mitk::DataStorage::SetOfObjects::ConstIterator it = allNodes->Begin(); it != allNodes->End(); ++it)
  {

    std::string name = it->Value()->GetName();
    if(name.find("Pregunta") != std::string::npos)
    {
      //Veo que numero de item es
      int item;
      it->Value()->GetIntProperty("medicas.gaming.itemNumber",item);
      if (item>max_item)
      {
        max_item=item;
      }
    }
  }
  N_items = max_item;

}

void Game_Estructuras::onCrearJuego()
{
    N_items=0;
    N_options.clear();
    game_name = "juego_prueba";    

    standard_options_names.push_back("A");
    standard_options_names.push_back("B");
    standard_options_names.push_back("C");
    standard_options_names.push_back("D");
    standard_options_names.push_back("E");
    m_standard_options_flag = false;

    MAX_OPCS=5;
    onAgregarItem();
    changeScreen(ESCRIBIR_PREG); //Screen de nueva pregunta
}
/*
void Game_Estructuras::widgetToNodesView()
{

  m_Controls.verticalLayout->addWidget(nodesView);


}

void Game_Estructuras::widgetToItemsView()
{


}
*/
void Game_Estructuras::onConfirmar()
{
  cout<<"CONFIRMAR PRESIONADO: "<<current_screen<<" . Nitems: "<<N_items<<endl;

  if (current_screen==CARGADO)
  {
    onJugarDemo();
    return;
  }

  if (current_screen==ESCRIBIR_PREG) //Pantalla de nueva pregunta
  {

    int state = onSimpleConfirmarPreg();
    if (state==0)
        changeScreen(INICIO_RESPUESTA);

    return;
  }
  if(current_screen==INICIO_RESPUESTA) //Pantalla de nueva opción
  {

    changeScreen(ESCRIBIR_OPC);
    return;
  }
  if(current_screen==ESCRIBIR_OPC) //Pantalla de nueva opción
  {
    int status = simpleAgregarOpcion();
    if (status==0)
    {
        onConfirmarItem();
        changeScreen(RESUMEN);
    }
    return;


  }
  if(current_screen==RESUMEN) //Pantalla resumen juego
  {
    onJugarDemo();
  }
}
void Game_Estructuras::onAtras()
{
  cout<<"ATRAS PRESIONADO: "<<current_screen<<" . Nitems: "<<N_items<<endl;
  if(current_screen==CARGADO)
  {
    changeScreen(INICIO);


  }
  if (current_screen==ESCRIBIR_PREG)
  {
    if(N_items==1)
    {
      changeScreen(INICIO);
      return;
    }
    else
    {
      onCancelarItem();
      m_Controls.pbConfirmar->setEnabled(true);
      changeScreen(RESUMEN);
      return;
    }
  }
  if(current_screen==INICIO_RESPUESTA)
  {
    changeScreen(ESCRIBIR_PREG);
    m_Controls.pbConfirmar->setEnabled(true);

    return;
  }
  if(current_screen==ESCRIBIR_OPC)
  {
    onCancelarItem();
    m_Controls.pbConfirmar->setEnabled(true);
    changeScreen(ESCRIBIR_PREG);
    return;
  }

  if(current_screen==RESUMEN)
  {
    onAgregarItem();
    changeScreen(ESCRIBIR_PREG);
    m_Controls.pbConfirmar->setEnabled(true);
    return;
  }
}

void Game_Estructuras::deleteItem(int item)
{
    cout<<"item "<<item<<endl;
    std::string name = "Pregunta"+std::to_string(item+1);
    if(GetDataStorage()->GetNamedNode(name))
    {
      GetDataStorage()->Remove(GetDataStorage()->GetNamedNode(name));
    }
    //m_Controls.cbOpcion->removeItem(m_Controls.cbOpcion->findText(name.c_str()));
    //std::string itemErasedStr = name.substr(name.find("Pregunta")+9,name.length());
    //int itemErased = std::atoi(itemErasedStr.c_str());
    for (unsigned long k = 0 ; k<N_options[item]; k++)
    {
      name = "Respuesta" + std::to_string(item+1) +"-Opcion" + std::to_string(k+1);
      if(GetDataStorage()->GetNamedNode(name))
      {
        GetDataStorage()->Remove(GetDataStorage()->GetNamedNode(name));
      }
      //m_Controls.cbOpcion->removeItem(m_Controls.cbOpcion->findText(name.c_str()));

    }
    for (int i=item+2; i<=int(N_items); i++) //Actualizo los numeros de los items posteriores
    {
      string name="Pregunta" + std::to_string(i);
      std::string new_name = "Pregunta" + std::to_string(i-1);
      cout<<"old name: "<<name<<". new name: "<<new_name<<endl;

      //m_Controls.cbOpcion->setItemText(m_Controls.cbOpcion->findText(name.c_str()),new_name.c_str());
      if(GetDataStorage()->GetNamedNode(name))
      {
        GetDataStorage()->GetNamedNode(name)->SetName(new_name);
      }

      for (unsigned long j = 0 ; j<N_options[i-1]; j++)
      {
        name = "Respuesta" + std::to_string(i) +"-Opcion" + std::to_string(j+1);
        string new_name="Respuesta" + std::to_string(i-1) +"-Opcion" + std::to_string(j+1);
        cout<<"old name"<<name<<"new name"<<new_name<<endl;

        if(GetDataStorage()->GetNamedNode(name))
        {
          GetDataStorage()->GetNamedNode(name)->SetName(new_name);
        }
        //m_Controls.cbOpcion->setItemText(m_Controls.cbOpcion->findText(name.c_str()),new_name.c_str());

      }
      m_Controls.cbItem->setItemText(i-1,new_name.c_str());

    }
    cout<<"Pasando a borrar opciones"<<endl;
    N_options.erase(N_options.begin()+item);
    //items_summaries.erase(items_summaries.begin()+item);
    if (N_items!=0)
    {
       N_items--;
    }

    m_Controls.cbItem->removeItem(item);
}

void Game_Estructuras::deleteOption(int pregunta, int item)
{
    string name = "Respuesta" + std::to_string(pregunta+1) + "-Opcion"+ std::to_string(item+1);
    bool is_correct;

    if(GetDataStorage()->GetNamedNode(name))
    {
        GetDataStorage()->GetNamedNode(name)->GetBoolProperty("medicas.gaming.isCorrect",is_correct);


        GetDataStorage()->Remove(GetDataStorage()->GetNamedNode(name));
    }
    for (unsigned long j = item+1 ; j<N_options[pregunta]; j++)
    {
        name = "Respuesta" + std::to_string(pregunta+1) +"-Opcion" + std::to_string(j+1);
        string new_name="Respuesta" + std::to_string(pregunta+1) +"-Opcion" + std::to_string(j);

        if(GetDataStorage()->GetNamedNode(name))
        {
            cout<<"Changing old name "<<name<<" to "<<new_name<<endl;
            GetDataStorage()->GetNamedNode(name)->SetName(new_name);

        }
    }


    if (N_options[pregunta]!=0)
    {
      N_options[pregunta]--;
    }

    if (is_correct)
    {
        QMessageBox msgBox;
        std::string message = "Esta era la respuesta correcta así que deberá marcar otra";
        msgBox.setText(message.c_str());
        msgBox.setInformativeText("Seleccione la nueva respuesta correcta");
        mitk::DataNode *nodoopcion;
        std::string name;
        std::string buttonName;
        /*std::vector<QMessageBox::ButtonRole> roles;
        roles[0] = QMessageBox::YesRole;
        roles[1]= QMessageBox::NoRole;
        roles[2] = QMessageBox::AcceptRole;
        roles[3] = QMessageBox::RejectRole;
        roles[4] = QMessageBox::ResetRole;*/
        std::vector<QAbstractButton*> buttons;

        for (unsigned long i =0;i<N_options[pregunta]; i++)
        {
          name = "Respuesta" + std::to_string(pregunta+1) +"-Opcion" + std::to_string(i+1);
          nodoopcion = GetDataStorage()->GetNamedNode(name);
          if(nodoopcion)
          {
            nodoopcion->GetStringProperty("medicas.gaming.text",buttonName);
          }
          QAbstractButton* button = msgBox.addButton(QString(buttonName.c_str()), QMessageBox::YesRole);
          buttons.push_back(button);
        }

        //msgBox.setStandardButtons(QMessageBox::Cancel|QMessageBox::Yes);
        msgBox.exec();
        QAbstractButton* selectedbutton = msgBox.clickedButton();
        cout<<buttons[0]<<endl;
        cout<<selectedbutton<<endl;
        ptrdiff_t index = buttons.at(0)-selectedbutton;
        std::string correctName = "Respuesta" + std::to_string(pregunta+1) +"-Opcion" + std::to_string(index+1);
        nodoopcion = GetDataStorage()->GetNamedNode(correctName);
        cout<<correctName<<endl;
        nodoopcion->SetBoolProperty("medicas.gaming.isCorrect",true);
        m_flag_hay_correcta = true;
    }


}

void Game_Estructuras::updateSummaryBasedOnDataStorage()
{
    std::string name;
    mitk::DataNode::Pointer itemNode;
    std::string updateSummary;

    items_summaries.clear();
    for (unsigned long i=0;i<N_items;i++)
    {



        name = "Pregunta" + std::to_string(i+1);

        itemNode = GetDataStorage()->GetNamedNode(name);
        int n_nodes;
        itemNode->GetIntProperty("medicas.gaming.NumberOfNodes", n_nodes);
        std::string questionText;
        itemNode->GetStringProperty("medicas.gaming.text", questionText);

        updateSummary = "\n PREGUNTA: " + questionText + "\n";
        for (int j = 0; j<n_nodes ; j++)
        {

          std::string prop_name = "medicas.gaming.nodo" + std::to_string(j);
          std::string node_name;
          itemNode->GetStringProperty(prop_name.c_str(),node_name);
          updateSummary = updateSummary  + node_name + "\n";
        }

        for (unsigned long opcion=0;opcion<N_options[i];opcion++)
        {
            name = "Respuesta"+ std::to_string(i+1) + "-Opcion" + std::to_string(opcion+1);
            itemNode = GetDataStorage()->GetNamedNode(name);
            itemNode->GetIntProperty("medicas.gaming.NumberOfNodes", n_nodes);
            itemNode->GetStringProperty("medicas.gaming.text", questionText);
            updateSummary = updateSummary + "\n Opcion " + std::to_string(opcion+1) +": " +  questionText + "\n";
            for (int j = 0; j<n_nodes ; j++)
            {
              std::string prop_name = "medicas.gaming.nodo" + std::to_string(j);
              std::string node_name;
              itemNode->GetStringProperty(prop_name.c_str(),node_name);
              updateSummary = updateSummary +  node_name + "\n";
            }
        }

        items_summaries.push_back(updateSummary);
    }
}

void Game_Estructuras::onCancelarItem()
{

  string name="Pregunta" + std::to_string(N_items);
  if(GetDataStorage()->GetNamedNode(name))
  {
    GetDataStorage()->Remove(GetDataStorage()->GetNamedNode(name));
  }
  for (unsigned long i = 0 ; i<N_options[N_items-1]; i++)
  {
    name = "Respuesta" + std::to_string(N_items) +"-Opcion" + std::to_string(i+1);
    if(GetDataStorage()->GetNamedNode(name))
    {
      GetDataStorage()->Remove(GetDataStorage()->GetNamedNode(name));
    }
  }
  N_options.pop_back();
  if (N_items!=0)
  {
    N_items--;
  }
}

void Game_Estructuras::onEliminarItem()
{
  int item = int(m_Controls.cbItem->currentIndex());
  QMessageBox msgBox;
  int ret;
  bool completeItem = false;
  //if (m_Controls.cbOpcion->currentText().toStdString().find("Pregunta") != string::npos)
  if (m_Controls.cbOpcion->currentIndex()==0)
  {
      std::string message = "Seleccionó eliminar la pregunta " + std::to_string(item+1) +".\n Esto eliminará todo el item correspondiente, incluyendo las respuestas.";
      msgBox.setText(message.c_str());
      msgBox.setInformativeText("¿Desea confirmarlo?");
      msgBox.setStandardButtons(QMessageBox::Cancel|QMessageBox::Yes);
      ret = msgBox.exec();
      completeItem = true;
  }
  else
  {
      std::string message = "Seleccionó eliminar la " + m_Controls.cbOpcion->currentText().toStdString() + ".\n La Pregunta " + std::to_string(item+1) + " seguirá existiendo, sólo se eliminará esta opción de entre las respuesta";
      msgBox.setText(message.c_str());
      msgBox.setInformativeText("¿Desea confirmarlo?");
      msgBox.setStandardButtons(QMessageBox::Cancel|QMessageBox::Yes);
      ret = msgBox.exec();
  }

  switch (ret)
  {
    case QMessageBox::Cancel:
        // Save was clicked
        return;
    case QMessageBox::Yes:
        // Don't Save was clicked
      if (completeItem)
      {
          deleteItem(m_Controls.cbItem->currentIndex());
      }
      else
      {
          deleteOption(m_Controls.cbItem->currentIndex(),m_Controls.cbOpcion->currentIndex()-1);
      }

   }
    onResumen();

}

void Game_Estructuras::updateItemList()
{
    int item = m_Controls.cbItem->currentIndex();
    m_Controls.cbOpcion->clear();
    m_Controls.cbOpcion->addItem(QString("Pregunta"));
    for (unsigned int i=0; i<N_options[item]; i++)
    {
        std::string itemName = "Opcion " + std::to_string(i+1);
        m_Controls.cbOpcion->addItem(QString(itemName.c_str()));
    }
}
void Game_Estructuras::onResumen()
{
    updateSummaryBasedOnDataStorage();
    int item = m_Controls.cbItem->currentIndex();
    QString currentitems(items_summaries[item].c_str());
    m_Controls.textResumen->setText(currentitems);
    updateItemList();

}


void Game_Estructuras::onMostrar()
{
    mostrarItem(m_Controls.cbItem->currentIndex(),m_Controls.cbOpcion->currentIndex());
}
void Game_Estructuras::mostrarItem(int numeroPregunta, int numeroItem)
{

    std::string itemCompleteName;
    //Chequear si esta queriendo mostrar toda una pregunta o una opcion
    if (m_Controls.cbOpcion->currentIndex()==0)
    {
        itemCompleteName = "Pregunta" + std::to_string(numeroPregunta+1);
    }
    else
    {
        itemCompleteName = "Respuesta" + std::to_string(numeroPregunta+1) + "-Opcion" + std::to_string(numeroItem);
    }

    cout<<"A mostrar: "<<itemCompleteName<<endl;
    mitk::DataNode *nodopreg = GetDataStorage()->GetNamedNode(itemCompleteName);
    if(nodopreg)
    {
      mitk::DataStorage::SetOfObjects::ConstPointer allNodes = GetDataStorage()->GetAll();
      for (mitk::DataStorage::SetOfObjects::ConstIterator it = allNodes->Begin(); it != allNodes->End(); ++it)
      {
        cout<<"it: "<<it.Value()<<endl;
        cout<<"it: "<<it->Value()->GetName()<<endl;
        if(it->Value().IsNotNull())
        {
          GetDataStorage()->GetNamedNode(it->Value()->GetName())->SetVisibility(false);

        }
      }
      int nodos;
      nodopreg->GetIntProperty("medicas.gaming.NumberOfNodes",nodos);
      cout<<"Nodos a mostrar: "<<nodos<<endl;
      if (nodos>0)
      {
        for(int i =0; i<nodos; i++)
        {

          std::string prop_name = "medicas.gaming.nodo" + std::to_string(i);
          std::string prop_name_color_R = "medicas.gaming.color_nodo_R" + std::to_string(i);
          std::string prop_name_color_G = "medicas.gaming.color_nodo_G" + std::to_string(i);
          std::string prop_name_color_B = "medicas.gaming.color_nodo_B" + std::to_string(i);
          std::string prop_opacity = "medicas.gaming.opacity" + std::to_string(i);
          std::string node_name;
          nodopreg->GetStringProperty(prop_name.c_str(),node_name);
          cout<<node_name<<endl;
          float col[3];
          nodopreg->GetFloatProperty(prop_name_color_R.c_str(),col[0]);
          nodopreg->GetFloatProperty(prop_name_color_G.c_str(),col[1]);
          nodopreg->GetFloatProperty(prop_name_color_B.c_str(),col[2]);
          float opac;
          nodopreg->GetFloatProperty(prop_opacity.c_str(),opac);
          mitk::Color colmitk;
          colmitk.SetRed(col[0]);
          colmitk.SetGreen(col[1]);
          colmitk.SetBlue(col[2]);


          if(GetDataStorage()->GetNamedNode(node_name))
          {
            GetDataStorage()->GetNamedNode(node_name)->SetVisibility(true);
            GetDataStorage()->GetNamedNode(node_name)->SetFloatProperty("opacity",opac);
            GetDataStorage()->GetNamedNode(node_name)->SetColor(colmitk);


          }
        }

      }
    }
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();

}
void Game_Estructuras::onCancelarJuego()
{

}

int Game_Estructuras::onSimpleConfirmarPreg()
{
    string name = "Pregunta" + std::to_string(N_items);
    mitk::DataNode::Pointer nuevapregunta = GetDataStorage()->GetNamedNode(name);
    std::string text;
    QString str = m_Controls.lePreg->toPlainText();
    if(str.isEmpty())
    {
      QMessageBox::warning(NULL, "Pregunta inválida", "Por favor escriba un texto.");
      changeScreen(ESCRIBIR_PREG);
      return 1;
    }
    if (!nuevapregunta)
    {
        nuevapregunta = mitk::DataNode::New();
        mitk::Surface::Pointer gd = mitk::Surface::New();
        vtkSmartPointer<vtkPoints> points =
          vtkSmartPointer<vtkPoints>::New();
        const float p[3] = {1.0, 2.0, 3.0};

        // Create the topology of the point (a vertex)
        vtkSmartPointer<vtkCellArray> vertices =
          vtkSmartPointer<vtkCellArray>::New();
        vtkIdType pid[1];
        pid[0] = points->InsertNextPoint(p);
        vertices->InsertNextCell(1,pid);

        // Create a polydata object
        vtkSmartPointer<vtkPolyData> point =
          vtkSmartPointer<vtkPolyData>::New();

        // Set the points and vertices we created as the geometry and topology of the polydata
        point->SetPoints(points);
        point->SetVerts(vertices);

        gd->SetVtkPolyData(point);
        nuevapregunta->SetData(gd);
        nuevapregunta->SetVisibility(false);
        gd->SetVtkPolyData(point);
        nuevapregunta->SetData(gd);
        nuevapregunta->SetBoolProperty("medicas.gaming.isQuestion", true);
        //nuevapregunta->SetProperty("helper object", mitk::BoolProperty::New(true));
        GetDataStorage()->Add(nuevapregunta);

    }


    text = str.toStdString();

    nuevapregunta->SetStringProperty("medicas.gaming.text",text.c_str());
    nuevapregunta->SetIntProperty("medicas.gaming.itemNumber", int(N_items));
    nuevapregunta->SetName(name);
    if (!m_Controls.cbEscenario->isChecked())
    {
        nuevapregunta->SetIntProperty("medicas.gaming.NumberOfNodes",0);
    }

    else
    {


        vector<std::string> list_of_node_names;
        vector<mitk::Color> list_of_node_colors;

        mitk::DataStorage::SetOfObjects::ConstPointer allNodes = GetDataStorage()->GetAll();
        for (mitk::DataStorage::SetOfObjects::ConstIterator it = allNodes->Begin(); it != allNodes->End(); ++it)
        {
            mitk::DataNode *current_node = it->Value();
            bool selected_flag=false;
            current_node->GetBoolProperty("visible",selected_flag);
            bool is_good=true;
            if (current_node->GetName().find("std") != std::string::npos) {
                is_good=false;
            }
            if (current_node->GetName().find("Pregunta") != std::string::npos) {
                is_good=false;
            }
            if (current_node->GetName().find("Respuesta") != std::string::npos) {
                is_good=false;
            }
            if (selected_flag && is_good)
            {
              list_of_node_names.push_back(current_node->GetName());
              mitk::ColorProperty *colorProperty;
              colorProperty=dynamic_cast<mitk::ColorProperty*>(current_node->GetProperty("color"));
              list_of_node_colors.push_back(colorProperty->GetColor());
            }
        }

        if (list_of_node_names.empty())
        {
            nuevapregunta->SetIntProperty("medicas.gaming.NumberOfNodes",0);
        }
        else
        {
            nuevapregunta->SetIntProperty("medicas.gaming.NumberOfNodes",list_of_node_names.size());
            for(std::vector<int>::size_type i = 0; i != list_of_node_names.size(); i++)
            {
              GetDataStorage()->GetNamedNode(list_of_node_names[i])->SetVisibility(false);
              GetDataStorage()->GetNamedNode(list_of_node_names[i])->Modified();
              string prop_name = "medicas.gaming.nodo" + std::to_string(i);
              string prop_name_color_R = "medicas.gaming.color_nodo_R" + std::to_string(i);
              string prop_name_color_G = "medicas.gaming.color_nodo_G" + std::to_string(i);
              string prop_name_color_B = "medicas.gaming.color_nodo_B" + std::to_string(i);
              string prop_opacity = "medicas.gaming.opacity" + std::to_string(i);
              mitk::FloatProperty *opacityProperty = dynamic_cast<mitk::FloatProperty*>(GetDataStorage()->GetNamedNode(list_of_node_names[i])->GetProperty("opacity"));
              float col[3];
              col[0] = list_of_node_colors[i].GetRed();
              col[1] =list_of_node_colors[i].GetGreen();
              col[2] =list_of_node_colors[i].GetBlue();              
              nuevapregunta->SetFloatProperty(prop_opacity.c_str(),opacityProperty->GetValue());
              nuevapregunta->SetStringProperty(prop_name.c_str(),list_of_node_names[i].c_str());
              nuevapregunta->SetFloatProperty(prop_name_color_R.c_str(),col[0]);
              nuevapregunta->SetFloatProperty(prop_name_color_G.c_str(),col[1]);
              nuevapregunta->SetFloatProperty(prop_name_color_B.c_str(),col[2]);
            }
        }

        nuevapregunta->SetIntProperty("medicas.gaming.view",m_Controls.cbVistasPreg->currentIndex());
    }


    nuevapregunta->Modified();

    mitk::RenderingManager::GetInstance()->RequestUpdateAll();

    return 0;
}



void Game_Estructuras::onAgregarItem()
{

  m_flag_hay_correcta = false;
  N_items++;
  unsigned long n_opts= 0;
  N_options.push_back(n_opts);


}

void Game_Estructuras::deleteCorrectAnswer()
{
    string name;
    for (unsigned long j = 0 ; j<N_options[N_items-1]; j++)
    {
      name = "Respuesta" + std::to_string(N_items) +"-Opcion" + std::to_string(j+1);
      if(GetDataStorage()->GetNamedNode(name))
      {
        GetDataStorage()->GetNamedNode(name)->SetBoolProperty("medicas.gaming.isCorrect",false);
      }

    }
}


int Game_Estructuras::simpleAgregarOpcion()
{
    string name = "Respuesta" + std::to_string(N_items) +"-Opcion" + std::to_string(N_options[N_items-1]+1);
    std::string text;
    mitk::DataNode::Pointer nuevarespuesta = GetDataStorage()->GetNamedNode(name);
    if (!nuevarespuesta)
    {
       nuevarespuesta = mitk::DataNode::New();
       mitk::Surface::Pointer gd = mitk::Surface::New();
       vtkSmartPointer<vtkPoints> points =
         vtkSmartPointer<vtkPoints>::New();
       const float p[3] = {1.0, 2.0, 3.0};

       // Create the topology of the point (a vertex)
       vtkSmartPointer<vtkCellArray> vertices =
         vtkSmartPointer<vtkCellArray>::New();
       vtkIdType pid[1];
       pid[0] = points->InsertNextPoint(p);
       vertices->InsertNextCell(1,pid);

       // Create a polydata object
       vtkSmartPointer<vtkPolyData> point =
         vtkSmartPointer<vtkPolyData>::New();

       // Set the points and vertices we created as the geometry and topology of the polydata
       point->SetPoints(points);
       point->SetVerts(vertices);

       gd->SetVtkPolyData(point);
       nuevarespuesta->SetData(gd);
       nuevarespuesta->SetVisibility(false);
       nuevarespuesta->SetBoolProperty("medicas.gaming.isQuestion", false);

       nuevarespuesta->SetIntProperty("medicas.gaming.itemNumber", N_items);
       nuevarespuesta->SetIntProperty("medicas.gaming.optionNumber",N_options[N_items-1]);
       //nuevarespuesta->SetProperty("helper object", mitk::BoolProperty::New(true));
       nuevarespuesta->SetName(name);
       GetDataStorage()->Add(nuevarespuesta);
    }
    if(m_Controls.rbSI->isChecked())
     {
       if (m_flag_hay_correcta)
       {
           std::string message = "Ya existía una respuesta correcta.";

           QMessageBox msgBox;
           msgBox.setText(message.c_str());
           msgBox.setInformativeText("¿Desea cambiarla por esta?");
           msgBox.setStandardButtons(QMessageBox::No|QMessageBox::Yes);
           int ret = msgBox.exec();
           switch (ret)
           {
             case QMessageBox::No:
               nuevarespuesta->SetBoolProperty("medicas.gaming.isCorrect",false);
               break;
             case QMessageBox::Yes:
               deleteCorrectAnswer();
               nuevarespuesta->SetBoolProperty("medicas.gaming.isCorrect",true);
               break;
             }
       }
       else
       {
       m_flag_hay_correcta = true;
       nuevarespuesta->SetBoolProperty("medicas.gaming.isCorrect",true);
       }

     }
     else
     {
       if(m_Controls.rbNO->isChecked())
       {
         nuevarespuesta->SetBoolProperty("medicas.gaming.isCorrect",false);
       }
       else
       {

           QMessageBox::warning(NULL, "Indicar si es correcta", "Por favor seleccione si esta respuesta es correcta o no.");
           changeScreen(ESCRIBIR_OPC);
           return 1;
       }
    }



    QString str = m_Controls.lePreg->toPlainText();
    text = str.toStdString();
    if (text.empty()|| m_standard_options_flag)
    {
      text = standard_options_names[N_options[N_items-1]];
    }


    nuevarespuesta->SetStringProperty("medicas.gaming.text",text.c_str());

    if (!m_Controls.cbEscenario->isChecked())
    {
        nuevarespuesta->SetIntProperty("medicas.gaming.NumberOfNodes",0);
    }
    else
    {
        vector<std::string> list_of_node_names;
        vector<mitk::Color> list_of_node_colors;

        mitk::DataStorage::SetOfObjects::ConstPointer allNodes = GetDataStorage()->GetAll();
        for (mitk::DataStorage::SetOfObjects::ConstIterator it = allNodes->Begin(); it != allNodes->End(); ++it)
        {
          mitk::DataNode *current_node = it->Value();
          bool selected_flag=false;
          current_node->GetBoolProperty("visible",selected_flag);
          bool is_good=true;
          if (current_node->GetName().find("std") != std::string::npos) {
              is_good=false;
          }
         if (selected_flag && is_good)
          {
            list_of_node_names.push_back(current_node->GetName());
            mitk::ColorProperty *colorProperty;
            colorProperty=dynamic_cast<mitk::ColorProperty*>(current_node->GetProperty("color"));
            list_of_node_colors.push_back(colorProperty->GetColor());


          }
        }
        if (!list_of_node_names.empty())
        {

          nuevarespuesta->SetIntProperty("medicas.gaming.NumberOfNodes",list_of_node_names.size());
          for(std::vector<int>::size_type i = 0; i != list_of_node_names.size(); i++)
          {
            GetDataStorage()->GetNamedNode(list_of_node_names[i])->SetVisibility(false);
            GetDataStorage()->GetNamedNode(list_of_node_names[i])->Modified();
            string prop_name = "medicas.gaming.nodo" + std::to_string(i);
            string prop_name_color_R = "medicas.gaming.color_nodo_R" + std::to_string(i);
            string prop_name_color_G = "medicas.gaming.color_nodo_G" + std::to_string(i);
            string prop_name_color_B = "medicas.gaming.color_nodo_B" + std::to_string(i);
            string prop_opacity = "medicas.gaming.opacity" + std::to_string(i);
            mitk::FloatProperty *opacityProperty = dynamic_cast<mitk::FloatProperty*>(GetDataStorage()->GetNamedNode(list_of_node_names[i])->GetProperty("opacity"));

            float col[3];
            col[0] = list_of_node_colors[i].GetRed();
            col[1] =list_of_node_colors[i].GetGreen();
            col[2] =list_of_node_colors[i].GetBlue();
            nuevarespuesta->SetFloatProperty(prop_opacity.c_str(),opacityProperty->GetValue());

            nuevarespuesta->SetStringProperty(prop_name.c_str(),list_of_node_names[i].c_str());
            nuevarespuesta->SetFloatProperty(prop_name_color_R.c_str(),col[0]);
            nuevarespuesta->SetFloatProperty(prop_name_color_G.c_str(),col[1]);
            nuevarespuesta->SetFloatProperty(prop_name_color_B.c_str(),col[2]);
          }
        }
        else
        {
            nuevarespuesta->SetIntProperty("medicas.gaming.NumberOfNodes",0);
        }

        nuevarespuesta->SetIntProperty("medicas.gaming.view",m_Controls.cbVistasPreg->currentIndex());
    }

    N_options[N_items-1]=N_options[N_items-1]+1;
    cout<<N_options[N_items-1]<<endl;
    if (N_options[N_items-1]==MAX_OPCS)
    {
      m_Controls.pbOtraOpcion->setEnabled(false);
    }

    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
    return 0;
}


void Game_Estructuras::onAgregarOpcion()
{
  //agregarOpcion(2);
  int status = simpleAgregarOpcion();
  if (status==0)
  {
    changeScreen(ESCRIBIR_OPC);
    return;
  }
}

void Game_Estructuras::onConfirmarItem()

{

  //EL ITEM NO ES VALIDO:
  if (!m_flag_hay_correcta)
  {

    QMessageBox msgBox;
    std::string message = "No ha marcado ninguna respuesta correcta.";
    msgBox.setText(message.c_str());
    msgBox.setInformativeText("Seleccione la correcta");
    mitk::DataNode *nodoopcion;
    std::string name;
    std::string buttonName;
    /*std::vector<QMessageBox::ButtonRole> roles;
    roles[0] = QMessageBox::YesRole;
    roles[1]= QMessageBox::NoRole;
    roles[2] = QMessageBox::AcceptRole;
    roles[3] = QMessageBox::RejectRole;
    roles[4] = QMessageBox::ResetRole;*/
    std::vector<QAbstractButton*> buttons;

    for (unsigned long i =0;i<N_options[N_items-1]; i++)
    {
      name = "Respuesta" + std::to_string(N_items) +"-Opcion" + std::to_string(i+1);
      nodoopcion = GetDataStorage()->GetNamedNode(name);
      if(nodoopcion)
      {
        nodoopcion->GetStringProperty("medicas.gaming.text",buttonName);
        //buttonName = "Opcion" + std::to_string(i+1) + ": "+ buttonName;
      }
      QAbstractButton* button = msgBox.addButton(QString(buttonName.c_str()), QMessageBox::YesRole);
      buttons.push_back(button);
    }

    //msgBox.setStandardButtons(QMessageBox::Cancel|QMessageBox::Yes);
    msgBox.exec();
    QAbstractButton* selectedbutton = msgBox.clickedButton();
    ptrdiff_t index = buttons[0]- selectedbutton;
    std::string correctName = "Respuesta" + std::to_string(N_items) +"-Opcion" + std::to_string(index+1);
    nodoopcion = GetDataStorage()->GetNamedNode(correctName);
    nodoopcion->SetBoolProperty("medicas.gaming.isCorrect",true);
    m_flag_hay_correcta = true;

  }

  //EL ITEM SÍ ES VÁLIDO:
  cout<<"el item es valido"<<endl;
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  string name="Pregunta" + std::to_string(N_items);
  mitk::DataNode *nodopreg = GetDataStorage()->GetNamedNode(name);
  if(nodopreg)
  {
    nodopreg->SetIntProperty("medicas.gaming.score",m_Controls.leScore->value());
  }

  m_Controls.lePreg->setText(""); 
  m_Controls.cbItem->addItem(std::to_string(N_items).c_str());
}

void Game_Estructuras::onJugarDemo()
{
  //m_Play.setWindowModality(Qt::ApplicationModal);
  //Qt::WindowFlags flags = m_Play.windowFlags();
  //flags = flags | Qt::WindowStaysOnTopHint;// | Qt::X11BypassWindowManagerHint;
  //m_Play.setWindowFlags(flags);
  m_Play = new GamePlay;
  m_Play->SetDataStorage(GetDataStorage());
  m_Play->SetNumberOfItems(N_items);
  m_Play->show();

  /*playWidget = new QWidget;
  //titulo = new QLabel(resultsWidget);
  //tableWidget = new QTableWidget(resultsWidget);
  pbStart = new QPushButton(playWidget);

  playWidget->setSizeIncrement(4, 5);
  playWidget->setMinimumSize(1000, 600);

  //tableWidget->verticalHeader()->hide();
  //tableWidget->horizontalHeader()->hide();
  //tableWidget->setEnabled(false);

  //titulo->setGeometry(300, 0, 700, 90);
  //pbExport->setText("Exportar resultados");
  pbStart->setGeometry(100, 300, 200, 40);
  //pbStart->setStyleSheet("QPushButton {background-color: black; color: white;}");
  pbStart->setText("¡Comenzar!");
  connect(pbStart, SIGNAL(clicked()), this, SLOT(onStart()));
  playWidget->show();
  */
}

void Game_Estructuras::onStart()
{
  QVTKOpenGLWidget *new_vtkwindow = new QVTKOpenGLWidget(playWidget);
  new_vtkwindow->setGeometry(100,100,400,400);
  vtkSmartPointer<vtkCubeSource> cubeSource =
      vtkSmartPointer<vtkCubeSource>::New();
  cubeSource->Update();
  vtkSmartPointer<vtkPolyDataMapper> cubeMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
  vtkSmartPointer<vtkActor> cubeActor =
      vtkSmartPointer<vtkActor>::New();
  cubeActor->SetMapper(cubeMapper);

  vtkSmartPointer<vtkRenderer> rightRenderer =
      vtkSmartPointer<vtkRenderer>::New();

  // Add Actor to renderer
  rightRenderer->AddActor(cubeActor);

  new_vtkwindow->GetRenderWindow()->AddRenderer(rightRenderer);
  playWidget->update();

}
void Game_Estructuras::SetFocus()
{
}
/*
void Game_Estructuras::createInteractor()
{
  us::Module* module = us::GetModuleContext()->GetModule("MyAwesomeLib");
  mitk::DataNode::Pointer dataNode = mitk::DataNode::New();
  mitk::GeometryData::Pointer gd = mitk::GeometryData::New();
  dataNode->SetData(gd);
  dataNode->SetName("Interactor");
  dataNode->SetVisibility(true);
  GetDataStorage()->Add(dataNode);

  m_CurrentInteractor = HighlightStructureInteractor::New();

  m_CurrentInteractor->SetDataStorage(GetDataStorage());
  m_CurrentInteractor->LoadStateMachine("Ferula.xml", module);
  m_CurrentInteractor->SetEventConfig("FerulaConfig.xml", module);
  m_CurrentInteractor->SetDataNode(dataNode);
  //m_CurrentInteractor->SetGameObjects(game_n_structures[i_current_game], structures_masks[i_current_game], structures_names[i_current_game], game_images[i_current_game]);


  mitk::TNodePredicateDataType<mitk::LabelSetImage>::Pointer predicateAllLabelSetImages =
    mitk::TNodePredicateDataType<mitk::LabelSetImage>::New();
  mitk::DataStorage::SetOfObjects::ConstPointer so = GetDataStorage()->GetSubset(predicateAllLabelSetImages);

  for (mitk::DataStorage::SetOfObjects::ConstIterator it = so->Begin(); it != so->End(); ++it)
  {

    mitk::DataNode *current_node = it->Value();
    if(!current_node)
    {
      QMessageBox::warning(NULL,"NO HAY ESTRUCTURAS.", "Por favor marque al menos una estructura. ");
      return;
    }
    current_node->SetBoolProperty("visible",false);
  }



}
*/

