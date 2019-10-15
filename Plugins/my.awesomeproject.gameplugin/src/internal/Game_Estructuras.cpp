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

#include <mitkImageCast.h>
#include <mitkLookupTables.h>
#include <mitkNodePredicateDataType.h>
#include <mitkGeometryData.h>
#include <mitkIOUtil.h>

#define INICIO 0
#define CARGADO 1
#define ESCRIBIR_PREG 2
#define SELECCIONAR_OBJETOS_PREG 22
#define INICIO_RESPUESTA 23
#define ESCRIBIR_OPC 3
#define SELECCIONAR_OBJETOS_OPC 32
#define DEFINIR_CORRECTA_OPC 33
#define DEFINIR_PUNTOS 34
#define RESUMEN 4


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
    m_Controls.rbEscenario->setChecked(false);
    m_Controls.rbSoloTexto->setChecked(false);
    m_Controls.labelTitulo->setText("Seleccione");
    QFont questiontitle;
    questiontitle.setPointSize(18);
    questiontitle.setBold(true);

    m_Controls.labelTitulo->setFont(questiontitle);

    m_Controls.pbCrear->setVisible(true);
    m_Controls.pbCargarJuego->setVisible(true);
    m_Controls.pbAtras->setVisible(false);
    m_Controls.pbConfirmar->setVisible(false);
    m_Controls.pbOtraOpcion->setVisible(false);
    m_Controls.pbFlecha->setVisible(false);
    m_Controls.pbPunto->setVisible(false);
    m_Controls.groupBoxEscenario->setVisible(false);
    m_Controls.labelScore->setVisible(false);
    m_Controls.indicacion->setVisible(false);
    m_Controls.lePreg->setVisible(false);
    m_Controls.leScore->setVisible(false);
    m_Controls.cbItem->setVisible(false);
    m_Controls.teItem->setVisible(false);
    m_Controls.indicacion2->setVisible(false);
    m_Controls.radioButtonsBox->setVisible(false);


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
    m_Controls.pbFlecha->setVisible(false);
    m_Controls.pbPunto->setVisible(false);
    m_Controls.groupBoxEscenario->setVisible(false);
    m_Controls.labelScore->setVisible(false);
    m_Controls.indicacion->setVisible(false);
    m_Controls.lePreg->setVisible(false);
    m_Controls.leScore->setVisible(false);
    m_Controls.cbItem->setVisible(false);
    m_Controls.teItem->setVisible(false);
    m_Controls.radioButtonsBox->setVisible(false);
    m_Controls.labelCorrecta->setVisible(false);

  }

  if(scr==ESCRIBIR_PREG)
  {
    std::string tit = "Pregunta "+ std::to_string(N_items);
    m_Controls.labelTitulo->setText(tit.c_str());
    m_Controls.pbCrear->setVisible(false);
    m_Controls.pbCargarJuego->setVisible(false);
    m_Controls.pbAtras->setVisible(true);
    m_Controls.pbAtras->setText("Atrás");
    m_Controls.pbConfirmar->setText("Listo!");
    m_Controls.pbConfirmar->setVisible(true);
    m_Controls.pbOtraOpcion->setVisible(false);
    /*m_Controls.pbFlecha->setVisible(true);
    m_Controls.pbFlecha->setText("Agregar flecha");
    m_Controls.pbPunto->setVisible(true);*/
    m_Controls.groupBoxEscenario->setVisible(false);
    m_Controls.labelScore->setVisible(false);
    m_Controls.indicacion->setVisible(true);
    m_Controls.indicacion->setText("Escriba la pregunta");
    m_Controls.lePreg->setVisible(true);
    m_Controls.lePreg->setEnabled(true);
    m_Controls.lePreg->setText("");
    m_Controls.leScore->setVisible(false);
    m_Controls.leScore->setValue(100);
    m_Controls.cbItem->setVisible(false);
    m_Controls.teItem->setVisible(false);
    m_Controls.radioButtonsBox->setVisible(false);
    m_Controls.labelCorrecta->setVisible(false);
    m_Controls.indicacion2->setVisible(false);

    //QFont summary_font;
    //summary_font.setItalic(true);
    //m_Controls.label_currentoptions->setFont(summary_font);
    //m_Controls.label_currentquestion->setFont(summary_font);

  }
  if (scr==SELECCIONAR_OBJETOS_PREG)
  {
      m_Controls.radioButtonsBox->setVisible(false);
      m_Controls.rbEscenario->setChecked(false);
      m_Controls.rbSoloTexto->setChecked(false);
      m_Controls.labelCorrecta->setVisible(false);
      m_Controls.pbCrear->setVisible(false);
      m_Controls.pbCargarJuego->setVisible(false);
      m_Controls.pbAtras->setVisible(true);
      m_Controls.pbAtras->setText("Atrás");
      m_Controls.pbConfirmar->setEnabled(false);
      m_Controls.pbConfirmar->setText("Listo!");
      m_Controls.pbConfirmar->setVisible(true);
      m_Controls.pbOtraOpcion->setVisible(false);
      /*m_Controls.pbFlecha->setVisible(true);
      m_Controls.pbFlecha->setText("Agregar flecha");
      m_Controls.pbPunto->setVisible(true);*/
      m_Controls.groupBoxEscenario->setVisible(true);
      m_Controls.labelScore->setVisible(false);
      m_Controls.indicacion->setVisible(false);
      m_Controls.lePreg->setVisible(true);
      m_Controls.lePreg->setEnabled(false);
      m_Controls.leScore->setVisible(false);
      m_Controls.leScore->setValue(100);
      m_Controls.cbItem->setVisible(false);
      m_Controls.teItem->setVisible(false);
      m_Controls.indicacion2->setVisible(false);
      m_Controls.indicacion2->setText("Deje visibles aquellos objetos que sean parte de la pregunta. \n Seleccione qué vista es la adecuada para la pregunta. ");
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
    /*m_Controls.pbFlecha->setVisible(true);
    m_Controls.pbFlecha->setText("Agregar flecha");
    m_Controls.pbPunto->setVisible(true);*/
    m_Controls.groupBoxEscenario->setVisible(false);
    m_Controls.labelScore->setVisible(false);
    m_Controls.indicacion->setVisible(true);
    m_Controls.indicacion->setText("A continuación, vamos a crear una por una las opciones \nmultiple-choice para esta pregunta.");
    m_Controls.lePreg->setVisible(false);
    m_Controls.lePreg->setText("");
    m_Controls.leScore->setVisible(false);
    m_Controls.cbItem->setVisible(false);
    m_Controls.teItem->setVisible(false);
    m_Controls.indicacion2->setVisible(false);
    m_Controls.radioButtonsBox->setVisible(false);
    m_Controls.labelCorrecta->setVisible(false);

  }
  if(scr==ESCRIBIR_OPC)
  {
    std::string tit = "Respuesta "+ std::to_string(N_items) + "\nOpción " +  std::to_string(N_options[N_items-1]+1);
    m_Controls.labelTitulo->setText(tit.c_str());
    m_Controls.indicacion2->setVisible(false);

    m_Controls.pbCrear->setVisible(false);
    m_Controls.pbCargarJuego->setVisible(false);
    m_Controls.pbAtras->setVisible(true);
    m_Controls.pbAtras->setText("Atrás");
    m_Controls.pbConfirmar->setText("Listo!");
    m_Controls.pbConfirmar->setVisible(true);
    m_Controls.pbOtraOpcion->setVisible(false);
    /*m_Controls.pbFlecha->setVisible(true);
    m_Controls.pbFlecha->setText("Agregar flecha");
    m_Controls.pbPunto->setVisible(true);*/
    m_Controls.groupBoxEscenario->setVisible(false);
    m_Controls.labelScore->setVisible(false);
    m_Controls.indicacion->setVisible(true);
    m_Controls.indicacion->setText("Escriba el texto para esta opción.");
    m_Controls.lePreg->setVisible(true);
    m_Controls.lePreg->setEnabled(true);
    m_Controls.leScore->setVisible(false);
    m_Controls.leScore->setValue(100);
    m_Controls.cbItem->setVisible(false);
    m_Controls.teItem->setVisible(false);
    m_Controls.radioButtonsBox->setVisible(false);
    m_Controls.labelCorrecta->setVisible(false);


    //QFont summary_font;
    //summary_font.setItalic(true);
    //m_Controls.label_currentoptions->setFont(summary_font);
    //m_Controls.label_currentquestion->setFont(summary_font);

  }

  if (scr==SELECCIONAR_OBJETOS_OPC)
  {
      m_Controls.rbEscenario->setChecked(false);
      m_Controls.rbSoloTexto->setChecked(false);
      m_Controls.pbCrear->setVisible(false);
      m_Controls.pbCargarJuego->setVisible(false);
      m_Controls.pbAtras->setVisible(true);
      m_Controls.pbAtras->setText("Atrás");
      m_Controls.pbConfirmar->setEnabled(false);
      m_Controls.pbConfirmar->setText("Listo!");
      m_Controls.pbConfirmar->setVisible(true);
      m_Controls.pbOtraOpcion->setVisible(false);
      /*m_Controls.pbFlecha->setVisible(true);
      m_Controls.pbFlecha->setText("Agregar flecha");
      m_Controls.pbPunto->setVisible(true);*/
      m_Controls.groupBoxEscenario->setVisible(true);
      m_Controls.rbEscenario->setChecked(false);
      m_Controls.rbSoloTexto->setChecked(false);
      m_Controls.labelScore->setVisible(false);
      m_Controls.indicacion->setVisible(false);
      m_Controls.lePreg->setVisible(true);
      m_Controls.lePreg->setEnabled(false);
      m_Controls.leScore->setVisible(false);
      m_Controls.leScore->setValue(100);
      m_Controls.cbItem->setVisible(false);
      m_Controls.teItem->setVisible(false);
      m_Controls.radioButtonsBox->setVisible(false);
      m_Controls.labelCorrecta->setVisible(false);

      m_Controls.indicacion2->setVisible(false);
      m_Controls.indicacion2->setText("Deje visibles aquellos objetos que sean \nparte de esta opción de respuesta. \n Seleccione qué vista es la adecuada para la respuesta. ");
  }

  if (scr==DEFINIR_CORRECTA_OPC)
  {
      m_Controls.pbCrear->setVisible(false);
      m_Controls.pbCargarJuego->setVisible(false);
      m_Controls.pbAtras->setVisible(true);
      m_Controls.pbAtras->setText("Atrás");
      m_Controls.pbConfirmar->setText("Fue la última opción, continuar");
      m_Controls.pbConfirmar->setVisible(true);
      m_Controls.pbOtraOpcion->setVisible(true);
      m_Controls.pbOtraOpcion->setText("Agregar siguiente opción");
      /*m_Controls.pbFlecha->setVisible(true);
      m_Controls.pbFlecha->setText("Agregar flecha");
      m_Controls.pbPunto->setVisible(true);*/
      m_Controls.groupBoxEscenario->setVisible(false);
      m_Controls.labelScore->setVisible(false);
      m_Controls.indicacion->setVisible(false);
      m_Controls.lePreg->setVisible(true);
      m_Controls.lePreg->setEnabled(false);
      m_Controls.leScore->setVisible(false);
      m_Controls.leScore->setValue(100);
      m_Controls.cbItem->setVisible(false);
      m_Controls.teItem->setVisible(false);
      if (m_flag_hay_correcta)
      {
          m_Controls.radioButtonsBox->setVisible(false);
          m_Controls.labelCorrecta->setVisible(false);
      }
      else
      {
          m_Controls.radioButtonsBox->setVisible(true);
          m_Controls.labelCorrecta->setVisible(true);
      }


      m_Controls.indicacion2->setVisible(false);
  }
  if (scr==DEFINIR_PUNTOS)
  {
      std::string tit = "Pregunta "+ std::to_string(N_items) + "\nPuntaje";
      m_Controls.labelTitulo->setText(tit.c_str());
      m_Controls.pbCrear->setVisible(false);
      m_Controls.pbCargarJuego->setVisible(false);
      m_Controls.pbAtras->setVisible(true);
      m_Controls.pbAtras->setText("Atrás");
      m_Controls.pbConfirmar->setText("Listo!");
      m_Controls.pbConfirmar->setVisible(true);
      m_Controls.pbOtraOpcion->setVisible(false);
      m_Controls.pbOtraOpcion->setText("");
      /*m_Controls.pbFlecha->setVisible(true);
      m_Controls.pbFlecha->setText("Agregar flecha");
      m_Controls.pbPunto->setVisible(true);*/
      m_Controls.groupBoxEscenario->setVisible(false);
      m_Controls.labelScore->setVisible(true);
      m_Controls.indicacion->setVisible(false);
      m_Controls.lePreg->setVisible(false);
      m_Controls.lePreg->setEnabled(false);
      m_Controls.leScore->setVisible(true);
      m_Controls.leScore->setValue(100);
      m_Controls.cbItem->setVisible(false);
      m_Controls.teItem->setVisible(false);
      m_Controls.radioButtonsBox->setVisible(false);
      m_Controls.labelCorrecta->setVisible(false);

      m_Controls.indicacion2->setVisible(true);
      m_Controls.indicacion2->setText("Indique cuántos puntos se ganarán al responder bien esta pregunta.");
  }
  if(scr==RESUMEN)
  {
    std::string tit = "Pregunta "+ std::to_string(N_items);
    m_Controls.labelTitulo->setText(tit.c_str());
    m_Controls.pbCrear->setVisible(false);
    m_Controls.pbCargarJuego->setVisible(false);
    m_Controls.pbAtras->setVisible(true);
    m_Controls.pbAtras->setText("Agregar otra pregunta");
    m_Controls.pbConfirmar->setText("Jugar demo");
    m_Controls.pbConfirmar->setVisible(true);
    m_Controls.pbOtraOpcion->setVisible(false);
    /*m_Controls.pbFlecha->setVisible(true);
    m_Controls.pbFlecha->setText("Eliminar item seleccionado");
    m_Controls.pbPunto->setVisible(false);*/
    m_Controls.groupBoxEscenario->setVisible(false);
    m_Controls.labelScore->setVisible(false);
    m_Controls.indicacion->setVisible(false);
    m_Controls.indicacion2->setText("RESUMEN DE JUEGO");
    m_Controls.lePreg->setVisible(false);
    m_Controls.leScore->setVisible(false);
    m_Controls.cbItem->setVisible(true);
    m_Controls.teItem->setVisible(true);
    m_Controls.radioButtonsBox->setVisible(false);
    m_Controls.labelCorrecta->setVisible(false);

    prepararResumen();
  }
}
void Game_Estructuras::prepararResumen()
{
  /*ARmar para que muestre pregunta y texto de opcineos de cada item
  QString currentitems(items_summaries[item-1].c_str());
  m_Controls.teObjetos->setText(currentitems);
  */
  for(int i=0;i<N_items;i++)
  {
    //TERMINARRRRRRRR
      cout<<"Sin implementar todavia"<<endl;

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
  connect(m_Controls.pbFlecha, SIGNAL (clicked()),this, SLOT(onFlecha()));
  connect(m_Controls.pbPunto, SIGNAL (clicked()),this, SLOT(onPunto()));
  connect(m_Controls.cbItem, SIGNAL(currentIndexChanged()), this, SLOT(onMostrarItem()));
  connect(m_Controls.rbEscenario, SIGNAL(clicked()),this,SLOT(onToggle()));
  connect(m_Controls.rbSoloTexto, SIGNAL(clicked()),this,SLOT(onToggle()));

  changeScreen(INICIO);

  nodesView = new QLineEdit;
  itemsView = new QHBoxLayout;

}

void Game_Estructuras::onToggle()
{
    if (m_Controls.rbEscenario->isChecked()||m_Controls.rbSoloTexto->isChecked())
    {
        m_Controls.pbConfirmar->setEnabled(true);
    }
    if (m_Controls.rbEscenario->isChecked())
    {
        QFont aclaracion;
        aclaracion.setItalic(true);
        m_Controls.indicacion2->setFont(aclaracion);
        m_Controls.indicacion2->setVisible(true);
    }
    else
    {
        m_Controls.indicacion2->setVisible(false);
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
      it->Value()->GetIntProperty("itemNumber",item);
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
  if (current_screen==CARGADO)
  {
    onJugarDemo();
    return;
  }

  if (current_screen==ESCRIBIR_PREG) //Pantalla de nueva pregunta
  {
    changeScreen(SELECCIONAR_OBJETOS_PREG);
    return;
  }
  if (current_screen==SELECCIONAR_OBJETOS_PREG) //Pantalla de nueva pregunta
  {
    onConfirmarPreg();
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
    changeScreen(SELECCIONAR_OBJETOS_OPC);
    return;
  }
  if(current_screen==SELECCIONAR_OBJETOS_OPC) //Pantalla de nueva opción
  {
    changeScreen(DEFINIR_CORRECTA_OPC);
    return;
  }
  if(current_screen==DEFINIR_CORRECTA_OPC) //Pantalla de nueva opción
  {
    agregarOpcion();
    changeScreen(DEFINIR_PUNTOS);
    return;
  }
  if(current_screen==DEFINIR_PUNTOS) //Pantalla de nueva opción
  {
    onConfirmarItem();
    changeScreen(RESUMEN);
    return;
  }
  if(current_screen==RESUMEN) //Pantalla resumen juego
  {
    onJugarDemo();
  }
}
void Game_Estructuras::onAtras()
{
  if(current_screen==CARGADO)
  {
    changeScreen(INICIO);
  }
  if (current_screen==ESCRIBIR_PREG)
  {
    if(N_items==0)
    {
      changeScreen(INICIO);
      return;
    }
    else
    {
      onCancelarItem();
      changeScreen(RESUMEN);
      return;
    }
  }
  if(current_screen==SELECCIONAR_OBJETOS_PREG)
  {
    changeScreen(ESCRIBIR_PREG);
    return;
  }

  if(current_screen==INICIO_RESPUESTA)
  {
    changeScreen(SELECCIONAR_OBJETOS_PREG);
    return;
  }
  if(current_screen==ESCRIBIR_OPC)
  {
    onCancelarItem();
    changeScreen(INICIO_RESPUESTA);
    return;
  }
  if(current_screen==SELECCIONAR_OBJETOS_OPC)
  {
    changeScreen(ESCRIBIR_OPC);
    return;
  }
  if(current_screen==DEFINIR_CORRECTA_OPC)
  {
    changeScreen(SELECCIONAR_OBJETOS_OPC);
    return;
  }
  if(current_screen==DEFINIR_PUNTOS)
  {
    changeScreen(DEFINIR_CORRECTA_OPC);
    return;
  }
  if(current_screen==RESUMEN)
  {
    onAgregarItem();
    changeScreen(ESCRIBIR_PREG);
  }
}
void Game_Estructuras::onFlecha()
{
  if(current_screen==RESUMEN)
  {
    onEliminarItem();
  }
  else
  {
    //crearFlecha();
  }
}

void Game_Estructuras::onPunto()
{

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
  int item = int(m_Controls.cbItem->currentIndex())+1;
  string name="Pregunta" + std::to_string(item);
  if(GetDataStorage()->GetNamedNode(name))
  {
    GetDataStorage()->Remove(GetDataStorage()->GetNamedNode(name));
  }
  for (unsigned long i = 0 ; i<N_options[item-1]; i++)
  {
    name = "Respuesta" + std::to_string(item) +"-Opcion" + std::to_string(i+1);
    if(GetDataStorage()->GetNamedNode(name))
    {
      GetDataStorage()->Remove(GetDataStorage()->GetNamedNode(name));
    }
  }

  for (int i=item+1; i<=int(N_items); i++)
  {
    string name="Pregunta" + std::to_string(i);
    if(GetDataStorage()->GetNamedNode(name))
    {
      string new_name="Pregunta" + std::to_string(i-1);
      GetDataStorage()->GetNamedNode(name)->SetName(new_name);
    }
    for (unsigned long j = 0 ; j<N_options[i-1]; j++)
    {
      name = "Respuesta" + std::to_string(i) +"-Opcion" + std::to_string(j+1);
      if(GetDataStorage()->GetNamedNode(name))
      {
        string new_name="Respuesta" + std::to_string(i-1) +"-Opcion" + std::to_string(j+1);

        GetDataStorage()->GetNamedNode(name)->SetName(new_name);

      }
    }
    std::string nameItem = "Pregunta " + std::to_string(i-1);
    m_Controls.cbItem->setItemText(i-1,nameItem.c_str());
  }

  items_summaries.erase(items_summaries.begin()+item-1);
  N_options.erase(N_options.begin()+item-1);
  /*
  for (int i=item-1; i<int(N_items); i++)
  {
    m_Controls.cbItem->addItem(std::to_string(i).c_str());
  }
  for (int i = 0; i<(int(N_items)-item);i++)
  {

    m_Controls.cbItem->addItem(std::to_string(item+i).c_str());
  }
  */
  if (N_items!=0)
  {
     N_items--;
  }

  m_Controls.cbItem->removeItem(item-1);

}

void Game_Estructuras::onMostrarItem(int item)
{

    item++;
    string name="Pregunta" + std::to_string(item);
    mitk::DataNode *nodopreg = GetDataStorage()->GetNamedNode(name);
    if(nodopreg)
    {
      mitk::DataStorage::SetOfObjects::ConstPointer allNodes = GetDataStorage()->GetAll();
      for (mitk::DataStorage::SetOfObjects::ConstIterator it = allNodes->Begin(); it != allNodes->End(); ++it)
      {
        if(it->Value().IsNotNull())
        {
          it->Value()->SetVisibility(false);

        }
      }
      int nodos;
      nodopreg->GetIntProperty("NumberOfNodes",nodos);
      if (nodos>0)
      {
        for(int i =0; i<nodos; i++)
        {
          string prop_name = "nodo" + std::to_string(i);
          string n;
          nodopreg->GetStringProperty(prop_name.c_str(),n);
          if(GetDataStorage()->GetNamedNode(n))
          {
            GetDataStorage()->GetNamedNode(n)->SetVisibility(true);
          }
          mitk::RenderingManager::GetInstance()->RequestUpdateAll();
        }

      }
    }
    QString currentitems(items_summaries[item-1].c_str());
    m_Controls.teItem->setText(currentitems);
    cout<<currentitems<<endl;
}
void Game_Estructuras::onCancelarJuego()
{

}



void Game_Estructuras::onConfirmarPreg()
{
  bool found_content = false;
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
      if(!found_content)
      {
        found_content=true;
      }
      list_of_node_names.push_back(current_node->GetName());
      mitk::ColorProperty *colorProperty;
      colorProperty=dynamic_cast<mitk::ColorProperty*>(current_node->GetProperty("color"));
      list_of_node_colors.push_back(colorProperty->GetColor());
    }
  }

  QString str = m_Controls.lePreg->text();
  if(!str.isEmpty())
  {
    found_content=true;

  }
  if (!found_content)
  {
    QMessageBox::warning(NULL, "Pregunta inválida", "Por favor escriba un texto o ponga visible un nodo.");
    return;
  }
  else
  {

    mitk::DataNode::Pointer nuevapregunta = mitk::DataNode::New();
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
    nuevapregunta->SetBoolProperty("isQuestion", true);
    string question_summary="";

    std::string text = str.toStdString();
    if (!text.empty())
    {
      question_summary = question_summary + "Texto: '" + text +"'";

    }

    if (!list_of_node_names.empty())
    {
      nuevapregunta->SetIntProperty("NumberOfNodes",list_of_node_names.size());
      for(std::vector<int>::size_type i = 0; i != list_of_node_names.size(); i++)
      {
        GetDataStorage()->GetNamedNode(list_of_node_names[i])->SetVisibility(false);
        GetDataStorage()->GetNamedNode(list_of_node_names[i])->Modified();
        question_summary=question_summary + '\n' + list_of_node_names[i];
        string prop_name = "nodo" + std::to_string(i);
        string prop_name_color_R = "color_nodo_R" + std::to_string(i);
        string prop_name_color_G = "color_nodo_G" + std::to_string(i);
        string prop_name_color_B = "color_nodo_B" + std::to_string(i);

        float col[3];
        col[0] = list_of_node_colors[i].GetRed();
        col[1] =list_of_node_colors[i].GetGreen();
        col[2] =list_of_node_colors[i].GetBlue();
        nuevapregunta->SetStringProperty(prop_name.c_str(),list_of_node_names[i].c_str());
        nuevapregunta->SetFloatProperty(prop_name_color_R.c_str(),col[0]);
        nuevapregunta->SetFloatProperty(prop_name_color_G.c_str(),col[1]);
        nuevapregunta->SetFloatProperty(prop_name_color_B.c_str(),col[2]);

      }

    }
    else
    {
      nuevapregunta->SetIntProperty("NumberOfNodes",0);
    }

    nuevapregunta->SetStringProperty("text",text.c_str());
    nuevapregunta->SetIntProperty("itemNumber", int(N_items));
    nuevapregunta->SetIntProperty("view",m_Controls.cbVistasPreg->currentIndex());
    string name = "Pregunta" + std::to_string(N_items);
    nuevapregunta->SetName(name);
    nuevapregunta->Modified();

    GetDataStorage()->Add(nuevapregunta);

    m_question_summary = question_summary;

    mitk::RenderingManager::GetInstance()->RequestUpdateAll();

  }
}
void Game_Estructuras::onAgregarItem()
{

  m_flag_hay_correcta = false;
  N_items++;
  unsigned long n_opts= 0;
  N_options.push_back(n_opts);


}

void Game_Estructuras::agregarOpcion()
{
    bool found_content = false;
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
        if(!found_content)
        {
          found_content=true;
        }
        list_of_node_names.push_back(current_node->GetName());
        mitk::ColorProperty *colorProperty;
        colorProperty=dynamic_cast<mitk::ColorProperty*>(current_node->GetProperty("color"));
        list_of_node_colors.push_back(colorProperty->GetColor());


      }
    }

    QString str = m_Controls.lePreg->text();

    if(!str.isEmpty())
    {
      found_content=true;

    }

    if (!found_content)
    {
      QMessageBox::warning(NULL, "Pregunta inválida", "Por favor escriba un texto o ponga visible un nodo.");
      return;
    }
    else
    {

      N_options[N_items-1]=N_options[N_items-1]+1;

      mitk::DataNode::Pointer nuevarespuesta = mitk::DataNode::New();
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
      nuevarespuesta->SetBoolProperty("isQuestion", false);
      string extra="";

      if(m_Controls.rbSI->isChecked())
      {

        m_flag_hay_correcta = true;
        nuevarespuesta->SetBoolProperty("isCorrect",true);
        extra = extra  +"CORRECTA";

      }
      else
      {
        nuevarespuesta->SetBoolProperty("isCorrect",false);
      }
      string option_summary="OPCION " + std::to_string(N_options[N_items-1]) + '\t' + '\t' +extra + '\n';
      std::string text = str.toStdString();
      if (!text.empty())
      {
        option_summary = option_summary + "Texto: '" + text +"'";

      }
      else
      {
        text = standard_options_names[N_options[N_items-1]-1];
      }
      if (!list_of_node_names.empty())
      {
        nuevarespuesta->SetIntProperty("NumberOfNodes",list_of_node_names.size());
        for(std::vector<int>::size_type i = 0; i != list_of_node_names.size(); i++)
        {
          GetDataStorage()->GetNamedNode(list_of_node_names[i])->SetVisibility(false);
          GetDataStorage()->GetNamedNode(list_of_node_names[i])->Modified();
          option_summary=option_summary + '\n' + list_of_node_names[i];
          string prop_name = "nodo" + std::to_string(i);
          string prop_name_color_R = "color_nodo_R" + std::to_string(i);
          string prop_name_color_G = "color_nodo_G" + std::to_string(i);
          string prop_name_color_B = "color_nodo_B" + std::to_string(i);

          float col[3];
          col[0] = list_of_node_colors[i].GetRed();
          col[1] =list_of_node_colors[i].GetGreen();
          col[2] =list_of_node_colors[i].GetBlue();
          nuevarespuesta->SetStringProperty(prop_name.c_str(),list_of_node_names[i].c_str());
          nuevarespuesta->SetFloatProperty(prop_name_color_R.c_str(),col[0]);
          nuevarespuesta->SetFloatProperty(prop_name_color_G.c_str(),col[1]);
          nuevarespuesta->SetFloatProperty(prop_name_color_B.c_str(),col[2]);
        }
        option_summary=option_summary + '\n' + '\n';
      }
      else
      {
          nuevarespuesta->SetIntProperty("NumberOfNodes",0);
      }

      nuevarespuesta->SetStringProperty("text",text.c_str());
      nuevarespuesta->SetIntProperty("itemNumber", N_items);
      nuevarespuesta->SetIntProperty("optionNumber",N_options[N_items-1]);
      nuevarespuesta->SetIntProperty("view",m_Controls.cbVistasPreg->currentIndex());

      string name = "Respuesta" + std::to_string(N_items) +"-Opcion" + std::to_string(N_options[N_items-1]);
      nuevarespuesta->SetName(name);
      nuevarespuesta->Modified();
      GetDataStorage()->Add(nuevarespuesta);
      m_Controls.lePreg->setText("");
      m_option_summary = m_option_summary + '\n'+ option_summary;
      QString currentopts(option_summary.c_str());
      //m_Controls.label_currentoptions->append(currentopts);
      mitk::RenderingManager::GetInstance()->RequestUpdateAll();

      if (N_options[N_items-1]==MAX_OPCS)
      {
        m_Controls.pbOtraOpcion->setEnabled(false);
      }
    }
}
void Game_Estructuras::onAgregarOpcion()
{
  agregarOpcion();
  changeScreen(ESCRIBIR_OPC);
}

void Game_Estructuras::onConfirmarItem()

{

  //EL ITEM NO ES VALIDO:
  if (!m_flag_hay_correcta)
  {
    QMessageBox::warning(NULL,"Item inválido.", "No ha marcado ninguna respuesta correcta. Ingrese todas las respuestas nuevamente.");
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
    return;
  }

  //EL ITEM SÍ ES VÁLIDO:
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  string name="Pregunta" + std::to_string(N_items);
  mitk::DataNode *nodopreg = GetDataStorage()->GetNamedNode(name);
  if(nodopreg)
  {
    nodopreg->SetIntProperty("score",m_Controls.leScore->value());
  }

  m_Controls.lePreg->text()="";


  //AGREGAR UPDATE DE ITEMS_SUMMARY
  string item_summary = "PREGUNTA";
  item_summary = item_summary + '\n' + m_question_summary + '\n' + '\n' + "RESPUESTAS" + m_option_summary + '\n';
  QString currentitems(item_summary.c_str());
  items_summaries.push_back(item_summary);

  m_option_summary ="";

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

