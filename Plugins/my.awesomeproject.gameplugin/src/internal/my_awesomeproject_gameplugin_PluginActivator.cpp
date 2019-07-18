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

#include "my_awesomeproject_gameplugin_PluginActivator.h"
#include "Game_Estructuras.h"

void my_awesomeproject_gameplugin_PluginActivator::start(ctkPluginContext* context)
{
  BERRY_REGISTER_EXTENSION_CLASS(Game_Estructuras, context)
}

void my_awesomeproject_gameplugin_PluginActivator::stop(ctkPluginContext*)
{
}
