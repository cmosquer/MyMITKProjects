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

#ifndef Ferula_h
#define Ferula_h

#include <QmitkAbstractView.h>
#include <mitkSurface.h>
#include "vtkRANSACPlane.h"
#include <vector>
#include <vtkSmartPointer.h>
#include <mitkLookupTable.h>
#include <mitkPointSet.h>

#include <qtablewidget.h>
#include <QTableWidget>


#include "itkImage.h"
#include "itkImageFileWriter.h"
// There's an item "FerulaControls.ui" in the UI_FILES list in
// files.cmake. The Qt UI Compiler will parse this file and generate a
// header file prefixed with "ui_", which is located in the build directory.
// Use Qt Creator to view and edit .ui files. The generated header file
// provides a class that contains all of the UI widgets.
#include <ui_FerulaControls.h>

#include <FerulaInteractor.h>


// All views in MITK derive from QmitkAbstractView. You have to override
// at least the two methods CreateQtPartControl() and SetFocus().
class Ferula : public QmitkAbstractView
{
	// As QmitkAbstractView derives from QObject and we want to use the Qt
	// signal and slot mechanism, we must not forget the Q_OBJECT macro.
	// This header file also has to be listed in MOC_H_FILES in files.cmake,
	// in order that the Qt Meta-Object Compiler can find and process this
	// class declaration.
	Q_OBJECT

  using PixelType = unsigned char;
  using ImageType = itk::Image< PixelType, 3 >;
  using WriterType = itk::ImageFileWriter< ImageType >;


public:
	// This is a tricky one and will give you some headache later on in
	// your debug sessions if it has been forgotten. Also, don't forget
	// to initialize it in the implementation file.
	static const std::string VIEW_ID;
  Ferula();
  mitk::PointSet::Pointer ps_mano;
  mitk::PointSet::Pointer ps_codo;


  vtkSmartPointer <vtkPolyData> polydata_brazo;
  // In this method we initialize the GUI components and connect the
  // associated signals and slots.
  void CreateQtPartControl(QWidget* parent) override;


private slots:

  void onCreateFerula();
  void onInteractor();
  void onTubes();
  void setRadius(int radius);
  void onAdjust();

private:
  // Typically a one-liner. Set the focus to the default widget.
  void SetFocus() override;
  int create3DNet(ImageType::Pointer output_image, unsigned long input_size_0, unsigned long input_size_1,  unsigned long input_size_2);
  int cutHand();
  int createFerula();
  int getPoints();
  int cutters(vtkSmartPointer<vtkPlane> planeMano, vtkSmartPointer<vtkPlane> planeCodo);
  // This method is conveniently called whenever the selection of Data Manager
  // items changes.
//  void OnSelectionChanged(
 //   berry::IWorkbenchPart::Pointer source,
  //  const QList<mitk::DataNode::Pointer>& dataNodes) override;

  Ui::FerulaControls                    m_Controls;
  FerulaInteractor::Pointer             m_CurrentInteractor;

};

#endif
