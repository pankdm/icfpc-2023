#pragma once

#include "common/base.h"
#include "common/files/json.h"
#include "common/geometry/d2/point.h"
#include "common/solvers/solution.h"

#include <vector>

class Solution : public solvers::Solution {
 public:
  std::vector<D2Point> positions;
  std::vector<double> volume;

 public:
  bool Empty() const { return positions.empty(); }

  void Clear() {
    positions.clear();
    volume.clear();
  }

  double Volume(unsigned i) const { return volume.empty() ? 1.0 : volume[i]; }

  void SetMaxVolume() {
    assert(!Empty());
    volume.clear();
    volume.resize(positions.size(), max_volume);
  }

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
    if (json.HasKey("volumes")) {
      auto& json_volume = json.GetValue("volumes");
      volume.resize(json_volume.Size());
      for (unsigned i = 0; i < volume.size(); ++i)
        volume[i] = json_volume.GetFloating(i);
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
    if (!volume.empty()) {
      files::JSON json_volume;
      json_volume.SetArray();
      for (auto d : volume) json_volume.Add(files::JSON(d));
      json.Add("volumes", json_volume);
    }
    json.Save(filename);
  }
};
