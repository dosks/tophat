/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2013 The XCSoar Project
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

#include "SymbolsConfigPanel.hpp"
#include "Profile/ProfileKeys.hpp"
#include "Form/DataField/Enum.hpp"
#include "Form/DataField/Base.hpp"
#include "Form/DataField/Listener.hpp"
#include "Interface.hpp"
#include "Language/Language.hpp"
#include "Form/Form.hpp"
#include "Widget/RowFormWidget.hpp"
#include "Dialogs/CallBackTable.hpp"
#include "UIGlobals.hpp"
#include "MapSettings.hpp"

enum ControlIndex {
  REACH_DISPLAY,
  ENABLE_FLARM_MAP,
  TRAIL_LENGTH,
  TRAIL_DRIFT,
  TRAIL_TYPE,
  TRAIL_WIDTH,
};

class SymbolsConfigPanel final
  : public RowFormWidget, DataFieldListener {
public:
  SymbolsConfigPanel()
    :RowFormWidget(UIGlobals::GetDialogLook()) {}

public:
  void ShowTrailControls(bool show);

  /* methods from Widget */
  virtual void Prepare(ContainerWindow &parent, const PixelRect &rc) override;
  virtual bool Save(bool &changed) override;

private:
  /* methods from DataFieldListener */
  virtual void OnModified(DataField &df) override;
};

void
SymbolsConfigPanel::ShowTrailControls(bool show)
{
  SetRowVisible(TRAIL_DRIFT, show);
  SetRowVisible(TRAIL_TYPE, show);
  SetRowVisible(TRAIL_WIDTH, show);
}

void
SymbolsConfigPanel::OnModified(DataField &df)
{
  if (IsDataField(TRAIL_LENGTH, df)) {
    const DataFieldEnum &dfe = (const DataFieldEnum &)df;
    TrailSettings::Length trail_length = (TrailSettings::Length)dfe.GetValue();
    ShowTrailControls(trail_length != TrailSettings::Length::OFF);
  }
}

static constexpr StaticEnumChoice ground_track_mode_list[] = {
  { (unsigned)DisplayGroundTrack::OFF, N_("Off"), N_("Disable display of ground track line.") },
  { (unsigned)DisplayGroundTrack::ON, N_("On"), N_("Always display ground track line.") },
  { (unsigned)DisplayGroundTrack::AUTO, N_("Auto"), N_("Display ground track line if there is a significant difference to plane heading.") },
  { 0 }
};

static constexpr StaticEnumChoice trail_length_list[] = {
  { (unsigned)TrailSettings::Length::OFF, N_("Off") },
  { (unsigned)TrailSettings::Length::LONG, N_("Long") },
  { (unsigned)TrailSettings::Length::SHORT, N_("Short") },
  { (unsigned)TrailSettings::Length::FULL, N_("Full") },
  { 0 }
};

static constexpr StaticEnumChoice trail_type_list[] = {
  { (unsigned)TrailSettings::Type::VARIO_1, N_("Vario #1"), N_("Within lift areas "
    "lines get displayed green and thicker, while sinking lines are shown brown and thin. "
    "Zero lift is presented as a grey line.") },
  { (unsigned)TrailSettings::Type::VARIO_1_DOTS, N_("Vario #1 (with dots)"), N_("The same "
    "colour scheme as the previous, but with dotted lines while sinking.") },
  { (unsigned)TrailSettings::Type::VARIO_2, N_("Vario #2"), N_("The climb colour "
    "for this scheme is orange to red, sinking is displayed as light blue to dark blue. "
    "Zero lift is presented as a yellow line.") },
  { (unsigned)TrailSettings::Type::VARIO_2_DOTS, N_("Vario #2 (with dots)"), N_("The same "
    "colour scheme as the previous, but with dotted lines while sinking.") },
  { (unsigned)TrailSettings::Type::VARIO_DOTS_AND_LINES,
    N_("Vario-scaled dots and lines"),
    N_("Vario-scaled dots with lines. "
       "Orange to red = climb. Light blue to dark blue = sink. "
       "Zero lift is presented as a yellow line.") },
  { (unsigned)TrailSettings::Type::ALTITUDE, N_("Altitude"), N_("The colour scheme corresponds to the height.") },
  { 0 }
};

void
SymbolsConfigPanel::Prepare(ContainerWindow &parent, const PixelRect &rc)
{
  const MapSettings &settings_map = CommonInterface::GetMapSettings();
  const ComputerSettings &settings_computer = CommonInterface::GetComputerSettings();
  const RoutePlannerConfig &route_planner = settings_computer.task.route_planner;

  static constexpr StaticEnumChoice turning_reach_list[] = {
    { (unsigned)RoutePlannerConfig::ReachMode::OFF, N_("Off"),
      N_("Reachable terrain outline not displayed.") },
    { (unsigned)RoutePlannerConfig::ReachMode::STRAIGHT, N_("On"),
      N_("An outline is displayed over the contours of the terrain indicating how far the glider can reach in each direction.") },
    { 0 }
  };

  AddEnum(_("Glidable terrain outline"),
          _("Whether an outline is displayed on the terrain showing how far the glider can reach."),
          turning_reach_list, (unsigned)route_planner.reach_calc_mode,
          this);

  AddBoolean(_("FLARM traffic"), _("This enables the display of FLARM traffic on the map window."),
             settings_map.show_flarm_on_map);

  AddEnum(_("Trail length"),
          _("Determines whether and how long a snail trail is drawn behind the glider."),
          trail_length_list,
          (unsigned)settings_map.trail.length, this);
  SetExpertRow(TRAIL_LENGTH);

  AddBoolean(_("Trail drift"),
             _("Determines whether the snail trail is drifted with the wind when displayed in "
               "circling mode. Switched Off, "
               "the snail trail stays uncompensated for wind drift."),
             settings_map.trail.wind_drift_enabled);
  SetExpertRow(TRAIL_DRIFT);

  AddEnum(_("Trail type"),
          _("Sets the type of the snail trail display."), trail_type_list, (int)settings_map.trail.type);
  SetExpertRow(TRAIL_TYPE);

  AddBoolean(_("Trail scaled"),
             _("If set to ON the snail trail width is scaled according to the vario signal."),
             settings_map.trail.scaling_enabled);
  SetExpertRow(TRAIL_WIDTH);

  ShowTrailControls(settings_map.trail.length != TrailSettings::Length::OFF);
}

bool
SymbolsConfigPanel::Save(bool &_changed)
{
  bool changed = false;

  MapSettings &settings_map = CommonInterface::SetMapSettings();
  ComputerSettings &settings_computer = CommonInterface::SetComputerSettings();
  RoutePlannerConfig &route_planner = settings_computer.task.route_planner;

  changed |= SaveValueEnum(REACH_DISPLAY, ProfileKeys::TurningReach,
                           route_planner.reach_calc_mode);

  changed |= SaveValue(ENABLE_FLARM_MAP, ProfileKeys::EnableFLARMMap,
                       settings_map.show_flarm_on_map);

  changed |= SaveValueEnum(TRAIL_LENGTH, ProfileKeys::SnailTrail, settings_map.trail.length);

  changed |= SaveValue(TRAIL_DRIFT, ProfileKeys::TrailDrift, settings_map.trail.wind_drift_enabled);

  changed |= SaveValueEnum(TRAIL_TYPE, ProfileKeys::SnailType, settings_map.trail.type);

  changed |= SaveValue(TRAIL_WIDTH, ProfileKeys::SnailWidthScale,
                       settings_map.trail.scaling_enabled);

  _changed |= changed;

  return true;
}

Widget *
CreateSymbolsConfigPanel()
{
  return new SymbolsConfigPanel();
}
