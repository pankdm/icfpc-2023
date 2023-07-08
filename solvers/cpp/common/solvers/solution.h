#pragma once

#include <string>

namespace solvers {
class Solution {
 protected:
  std::string id;

 public:
  const std::string& GetId() const { return id; }
  void SetId(const std::string& new_id) { id = new_id; }

  bool Load(const std::string& /* id */, const std::string& /* solver_name */);
  void Save(const std::string& /* solver_name */) const;
};
}  // namespace solvers
