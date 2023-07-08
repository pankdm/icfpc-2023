#pragma once

#include "common/files/json.h"
#include "common/geometry/d2/point.h"
#include "common/solvers/solution.h"

#include <vector>

class Solution : public solvers::Solution {
 public:
  std::vector<D2Point> positions;

  bool Load(const std::string& /* id */, const std::string& /* filename */) {
    return false;
  }

  void Save(const std::string& filename) const {
    files::JSON json_placements;
    json_placements.SetArray();
    for (auto& p : positions) {
      files::JSON json_p;
      json_p.SetDictionary();
      json_p.Add("x", files::JSON(p.x));
      json_p.Add("y", files::JSON(p.y));
      json_placements.Add(json_p);
    }
    files::JSON json;
    json.SetDictionary();
    json.Add("placements", json_placements);
    json.Save(filename);
  }
};
