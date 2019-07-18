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

#ifndef GAME_PLAY_H_
#define GAME_PLAY_H_
#include <QObject>
#include <ui_GamePlay.h>
#include <QDialog>
#include<bits/stdc++.h>
#include <vector>
#include <QVTKWidget.h>
#include <QMediaPlayer>
#include <mitkDataNode.h>
#include <mitkDataStorage.h>
#include <mitkPointSet.h>
#include <mitkPointSetDataInteractor.h>

class GamePlay : public QDialog, public Ui_GamePlay
{
  Q_OBJECT


public:
  GamePlay( QWidget * parent = 0);
  void SetDataStorage(mitk::DataStorage::Pointer ds);
  void SetItemWidgetInLayout(mitk::DataNode::Pointer questionNode, QLayout *layout);
  void SetNumberOfItems(unsigned long N);
  void PlayItem(int i);
private slots:
  void onStart();
  void onClickOption();
  void onItemEnd();
  void exit();
  void onNextItem();
  void playClicked();

private:
  std::vector<QVTKWidget*> options_widgets;
  mitk::DataStorage::Pointer m_dataStorage;
  int N_items;
  QObject *m_correct_answer;
  int current_score;
  int current_item;
  int current_item_score;
  QMediaPlayer *music;
  bool music_loaded=false;
  //mitk::PointSet::Pointer m_PointsInOptionWindow;
  //mitk::PointSetDataInteractor::Pointer m_interactor;
};
#endif
