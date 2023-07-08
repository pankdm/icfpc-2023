#pragma once

#include "base/attendee.h"
#include "base/constants.h"

#include "common/files/json.h"
#include "common/geometry/d2/axis/rectangle.h"
#include "common/geometry/d2/circle.h"
#include "common/geometry/d2/point.h"
#include "common/solvers/problem.h"

#include <algorithm>
#include <vector>

class Problem : public solvers::Problem {
 public:
  D2ARectangle room;
  D2ARectangle stage;  // Already adjusted by musician_collision_radius
  unsigned total_instruments;
  std::vector<unsigned> instruments;
  std::vector<Attendee> attendees;
  std::vector<D2Circle> pillars;

 public:
  bool Load(const std::string& _id) {
    return Load(_id, "../../problems/" + _id + ".json");
  }

  bool Load(const std::string& _id, const std::string& filename) {
    id = _id;
    files::JSON json;
    if (!json.Load(filename)) return false;
    room = D2ARectangle({0., 0.}, {json.GetFloating("room_width"),
                                   json.GetFloating("room_height")});
    {
      auto& json_stage = json.GetValue("stage_bottom_left");
      double x0 = json_stage.GetFloating(0) + musician_collision_radius;
      double y0 = json_stage.GetFloating(1) + musician_collision_radius;
      double x1 =
          json.GetFloating("stage_width") + x0 - 2 * musician_collision_radius;
      double y1 =
          json.GetFloating("stage_height") + y0 - 2 * musician_collision_radius;
      if (x1 < x0) x0 = x1 = (x0 + x1) / 2.;
      if (y1 < y0) y0 = y1 = (y0 + y1) / 2.;
      stage = D2ARectangle({x0, y0}, {x1, y1});
    }
    auto& json_musicians = json.GetValue("musicians");
    instruments.resize(json_musicians.Size());
    total_instruments = 0;
    for (unsigned i = 0; i < instruments.size(); ++i) {
      instruments[i] = json_musicians.GetInteger(i);
      total_instruments = std::max(total_instruments, instruments[i] + 1);
    }
    auto& json_attendees = json.GetValue("attendees");
    attendees.resize(json_attendees.Size());
    for (unsigned i = 0; i < attendees.size(); ++i) {
      auto& ai = attendees[i];
      auto& json_ai = json_attendees.GetValue(i);
      ai.position = D2Point(json_ai.GetFloating("x"), json_ai.GetFloating("y"));
      auto& json_atastes = json_ai.GetValue("tastes");
      if (json_atastes.Size() != total_instruments) return false;
      ai.tastes.resize(total_instruments);
      for (unsigned j = 0; j < total_instruments; ++j)
        ai.tastes[j] = json_atastes.GetFloating(j);
    }
    auto& json_pillars = json.GetValue("pillars");
    pillars.resize(json_pillars.Size());
    for (unsigned i = 0; i < pillars.size(); ++i) {
      auto& pi = pillars[i];
      auto& json_pi = json_pillars.GetValue(i);
      auto& json_xy = json_pi.GetValue("center");
      pi.c.x = json_xy.GetFloating(0);
      pi.c.y = json_xy.GetFloating(1);
      pi.r = json_pi.GetFloating("radius");
    }
    return true;
  }
};
