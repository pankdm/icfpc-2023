#pragma once

#include "base/attendee.h"
#include "base/constants.h"

#include "common/geometry/d2/axis/rectangle.h"
#include "common/geometry/d2/point.h"
#include "common/solvers/problem.h"

#include <vector>

class Problem : public solvers::Problem {
 public:
  D2ARectangle room;
  D2ARectangle stage;  // Already adjusted by musician_collision_radius
  std::vector<unsigned> instruments;
  std::vector<Attendee> attendees;
};
