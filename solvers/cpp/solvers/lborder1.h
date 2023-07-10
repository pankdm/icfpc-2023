#pragma once

#include "adjusters/assignment.h"
#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"
#include "solvers/base.h"

#include "common/geometry/d2/base.h"
#include "common/geometry/d2/point.h"
#include "common/solvers/solver.h"
#include "common/timer.h"

#include <algorithm>
#include <random>
#include <vector>

class LBorder1 : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  LBorder1() : BaseSolver() {}
  explicit LBorder1(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<LBorder1>(*this); }

  std::string Name() const override { return "loks_border1"; }

  // bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  Solution Solve(const TProblem& p) override {
    std::cout << "LBorder1:\t" << p.Id() << " started" << std::endl;
    Timer t;
    TProblem pfake = p;
    pfake.total_instruments += 1;
    for (auto& a : pfake.attendees) a.tastes.push_back(0);
    pfake.musicians.push_back({});
    unsigned max_candidates = std::max<unsigned>(2000u, p.instruments.size());

    int x1 = int(p.stage.p1.x + 0.5), x2 = int(p.stage.p2.x + 0.5),
        dx = x2 - x1;
    int y1 = int(p.stage.p1.y + 0.5), y2 = int(p.stage.p2.y + 0.5),
        dy = y2 - y1;
    double h = musician_collision_radius / 2.0 * sqrt(3.0) + 1e-6;
    double d2 = musician_collision_radius * musician_collision_radius;

    bool skip_x1 = true, skip_x2 = true, skip_y1 = true, skip_y2 = true;
    for (auto& a : p.attendees) {
      if (a.position.x < x1) skip_x1 = false;
      if (a.position.x > x2) skip_x2 = false;
      if (a.position.y < y1) skip_y1 = false;
      if (a.position.y > y2) skip_y2 = false;
    }
    if (dx < 10) skip_x1 = skip_x2 = false;
    if (dy < 10) skip_y1 = skip_y2 = false;

    std::vector<unsigned> vtask;
    unsigned mtask = 16;
    for (unsigned i = 0; i < mtask * 10000; ++i) {
      vtask.push_back(i);
    }
    auto rng = std::default_random_engine{};
    std::uniform_real_distribution<> urdx(p.stage.p1.x, p.stage.p2.x);
    std::uniform_real_distribution<> urdy(p.stage.p1.y, p.stage.p2.y);
    std::shuffle(vtask.begin(), vtask.end(), rng);

    int64_t best_iscore = 0;
    Solution sbest;
    unsigned itask = 0;
    for (; itask < vtask.size(); ++itask) {
      if (t.GetSeconds() > max_time_in_seconds) break;
      auto task = vtask[itask], jtask = itask;

      auto TIF = [&]() {
        bool b = (jtask & 1);
        jtask /= 2;
        return b;
      };

      std::vector<unsigned> vs;
      for (unsigned i = 0; i < 4; ++i) {
        vs.push_back(task % 10);
        task /= 10;
      }
      std::vector<std::vector<D2Point>> vvp1(4);
      std::vector<std::vector<D2Point>> vvp2(4);
      for (int k = vs[0]; k <= dx; k += 10)
        vvp1[0].push_back(D2Point(x1 + k, y1));
      for (int k = vs[0] + 5; x1 + k < vvp1[0].back().x; k += 10)
        vvp2[0].push_back(D2Point(x1 + k, y1 + h));
      for (int k = vs[1]; k <= dx; k += 10)
        vvp1[1].push_back(D2Point(x1 + k, y2));
      for (int k = vs[1] + 5; x1 + k < vvp1[1].back().x; k += 10)
        vvp2[1].push_back(D2Point(x1 + k, y2 - h));
      for (int k = vs[2]; k <= dy; k += 10)
        vvp1[2].push_back(D2Point(x1, y1 + k));
      for (int k = vs[2] + 5; y1 + k < vvp1[2].back().y; k += 10)
        vvp2[2].push_back(D2Point(x1 + h, y1 + k));
      for (int k = vs[3]; k <= dy; k += 10)
        vvp1[3].push_back(D2Point(x2, y1 + k));
      for (int k = vs[2] + 5; y1 + k < vvp1[3].back().y; k += 10)
        vvp2[3].push_back(D2Point(x2 - h, y1 + k));

      if (x1 == x2) {
        vvp1[0].clear();
        vvp1[1].clear();
        vvp1[2].clear();
        vvp2[0].clear();
        vvp2[1].clear();
        vvp2[2].clear();
        vvp2[3].clear();
      } else if (y1 == y2) {
        vvp1[1].clear();
        vvp1[2].clear();
        vvp1[3].clear();
        vvp2[0].clear();
        vvp2[1].clear();
        vvp2[2].clear();
        vvp2[3].clear();
      } else if (dx < 10) {
        vvp1[TIF() ? 3 : 2].clear();
        vvp2[0].clear();
        vvp2[1].clear();
        vvp2[2].clear();
        vvp2[3].clear();
      } else if (dy < 10) {
        vvp1[TIF() ? 1 : 0].clear();
        vvp2[0].clear();
        vvp2[1].clear();
        vvp2[2].clear();
        vvp2[3].clear();
      } else if (dx < 18) {
        vvp2[2].clear();
        vvp2[3].clear();
      } else if (dy < 18) {
        vvp2[0].clear();
        vvp2[1].clear();
      }

      if (skip_y1) {
        vvp1[0].clear();
        vvp2[0].clear();
      }
      if (skip_y2) {
        vvp1[1].clear();
        vvp2[1].clear();
      }
      if (skip_x1) {
        vvp1[2].clear();
        vvp2[2].clear();
      }
      if (skip_x2) {
        vvp1[3].clear();
        vvp2[3].clear();
      }

      bool b2xok = true;
      for (unsigned k0 = 0; k0 <= std::min<unsigned>(2u, vvp2[0].size());
           ++k0) {
        for (unsigned k1 = 0; k1 <= std::min<unsigned>(2u, vvp2[1].size());
             ++k1) {
          if (SquaredDistanceL2(vvp2[0][k0], vvp2[1][k1]) < d2) b2xok = false;
        }
      }
      if (!b2xok) vvp2[TIF() ? 1 : 0].clear();
      bool b2yok = true;
      for (unsigned k2 = 0; k2 <= std::min<unsigned>(2u, vvp2[2].size());
           ++k2) {
        for (unsigned k3 = 0; k3 <= std::min<unsigned>(2u, vvp2[3].size());
             ++k3) {
          if (SquaredDistanceL2(vvp2[2][k2], vvp2[3][k3]) < d2) b2yok = false;
        }
      }
      if (!b2yok) vvp2[TIF() ? 3 : 2].clear();

      std::vector<D2Point> vp1, vp2;
      for (unsigned i = 0; i < 4; ++i) {
        vp1.insert(vp1.end(), vvp1[i].begin(), vvp1[i].end());
        vp2.insert(vp2.end(), vvp2[i].begin(), vvp2[i].end());
      }
      // Drop overlapping musicians
      std::shuffle(vp1.begin(), vp1.end(), rng);
      for (unsigned i = 0; i < vp1.size(); ++i) {
        for (unsigned j = i + 1; j < vp1.size(); ++j) {
          if (SquaredDistanceL2(vp1[i], vp1[j]) < d2 - 1e-6) {
            vp1[j--] = vp1.back();
            vp1.pop_back();
          }
        }
        for (unsigned j = 0; j < vp2.size(); ++j) {
          if (SquaredDistanceL2(vp1[i], vp2[j]) < d2 - 1e-6) {
            vp2[j--] = vp2.back();
            vp2.pop_back();
          }
        }
      }
      for (unsigned i = 0; i < vp2.size(); ++i) {
        for (unsigned j = i + 1; j < vp2.size(); ++j) {
          if (SquaredDistanceL2(vp2[i], vp2[j]) < d2 - 1e-6) {
            vp2[j--] = vp2.back();
            vp2.pop_back();
          }
        }
      }
      vp1.insert(vp1.end(), vp2.begin(), vp2.end());
      for (; vp1.size() < p.instruments.size();) {
        D2Point temp(urdx(rng), urdy(rng));
        bool ok = true;
        for (auto& pp : vp1) {
          if (SquaredDistanceL2(pp, temp) < d2) {
            ok = false;
            break;
          }
        }
        if (ok) vp1.push_back(temp);
      }
      pfake.instruments.resize(p.instruments.size());
      pfake.musicians.back().clear();
      if (vp1.size() > max_candidates) vp1.resize(max_candidates);
      if (vp1.size() > pfake.instruments.size()) {
        for (unsigned k = pfake.instruments.size(); k < vp1.size(); ++k) {
          pfake.musicians.back().push_back(k);
          pfake.instruments.push_back(pfake.total_instruments - 1);
        }
      }
      Solution stemp;
      stemp.positions = vp1;
      stemp.SetMaxVolume();
      AdjusterAssignment::Check(pfake, stemp, true);
      if (stemp.positions.size() > p.instruments.size()) {
        stemp.positions.resize(p.instruments.size());
        stemp.volume.resize(p.instruments.size());
        AdjusterAssignment::Check(p, stemp);
      }
      auto iscore = Evaluator::Apply(p, stemp).score;
      if (best_iscore < iscore) {
        best_iscore = iscore;
        sbest = stemp;
      }
    }

    std::cout << "LBorder1:\t" << p.Id() << "\t" << t.GetSeconds() << "\t"
              << itask << "\t" << best_iscore << std::endl;
    sbest.SetId(p.Id());
    return sbest;
  }
};
