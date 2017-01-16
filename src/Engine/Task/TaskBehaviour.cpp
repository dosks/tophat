/* Copyright_License {

  Top Hat Soaring Glide Computer - http://www.tophatsoaring.org/
  Copyright (C) 2000-2016 The Top Hat Soaring Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
 */

#include "TaskBehaviour.hpp"

void
SectorDefaults::SetDefaults()
{
  start_type = TaskPointFactoryType::START_CYLINDER;
  start_radius = fixed(1000);
  turnpoint_type = TaskPointFactoryType::AST_CYLINDER;
  turnpoint_radius = fixed(500);
  finish_type = TaskPointFactoryType::FINISH_CYLINDER;
  finish_radius = fixed(1000);
}

void
TaskStartMargins::SetDefaults()
{
  max_speed_margin = fixed(0);
  max_height_margin = 0u;
}

void
TaskBehaviour::SetDefaults()
{
  glide.SetDefaults();

  optimise_targets_range = true;
  optimise_targets_bearing = true;
  optimise_targets_margin = 300;
  auto_mc = false;
  auto_mc_mode = AutoMCMode::CLIMBAVERAGE;
  calc_cruise_efficiency = true;
  calc_effective_mc = true;
  calc_glide_required = true;
  goto_nonlandable = true;
  risk_gamma = fixed(0);
  safety_mc = fixed(1.7);
  task_planning_speed_mode = TaskPlanningSpeedMode::PastPerformanceSpeed;
  task_planning_speed_override = fixed(25);
  safety_height_arrival = fixed(0);
  safety_height_arrival_gr = fixed(0);
  task_type_default = TaskFactoryType::RACING;
  start_margins.SetDefaults();
  sector_defaults.SetDefaults();
  ordered_defaults.SetDefaults();
  route_planner.SetDefaults();
  abort_task_mode = AbortTaskMode::SIMPLE;
  abort_task_airfield_only = false;
}

void
TaskBehaviour::DisableAll()
{
  optimise_targets_range = false;
  optimise_targets_bearing = false;
  auto_mc = false;
  calc_cruise_efficiency = false;
  calc_glide_required = false;
  route_planner.mode = RoutePlannerConfig::Mode::NONE;
}
