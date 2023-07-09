#pragma once

#include "common/geometry/d2/angle.h"

namespace geometry {
namespace d2 {
template <class T>
class DAngle : public Angle<T, DAngle<T>> {
 public:
  using TSelf = DAngle<T>;
  using TBase = Angle<T, TSelf>;

  DAngle() {}
  DAngle(const T& _dx, const T& _dy) : TBase(_dx, _dy) { Normalize(); }
  explicit DAngle(const Vector<T>& v) : TBase(v) { Normalize(); }

  void Normalize() {
    auto l = TBase::ToVector().Length();
    assert(l > 0);
    TBase::dx /= l;
    TBase::dy /= l;
  }
};
}  // namespace d2
}  // namespace geometry

using D2Angle = geometry::d2::DAngle<double>;
