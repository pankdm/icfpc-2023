#pragma once

#include "common/files/json.h"
#include "common/geometry/d2/point.h"
#include "common/solvers/solution.h"

#include <vector>

class Solution : public solvers::Solution {
 public:
  std::vector<D2Point> positions;

  bool Empty() const { return positions.empty(); }

  void Clear() { positions.clear(); }

  static std::string FileName(const std::string& id,
                              const std::string& solver_name) {
    return "../../solutions/" + solver_name + "/" + id + ".json";
  }

  bool Load(const std::string& id, const std::string& solver_name) {
    SetId(id);
    auto filename = FileName(GetId(), solver_name);
    files::JSON json;
    if (!json.Load(filename)) return false;
    auto& json_placement = json.GetValue("placements");
    positions.resize(json_placement.Size());
    for (unsigned i = 0; i < positions.size(); ++i) {
      auto& json_pi = json_placement.GetValue(i);
      positions[i].x = json_pi.GetFloating("x");
      positions[i].y = json_pi.GetFloating("y");
    }
    return true;
  }

  void Save(const std::string& solver_name) const {
    auto filename = FileName(GetId(), solver_name);
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
