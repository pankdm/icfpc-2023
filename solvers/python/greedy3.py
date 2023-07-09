# From the authors of Greedy and Greedy2
import math
import sys
from src.utils import *
import vectormath as vm

pi_2 = 2 * math.pi


class GreedySolver3:
    def __init__(self, i):
        self.i = i
        self.spec = ProblemX(i)
        print(f'Assigning {self.spec.size} musicians')

    def drop_deaf(self):
        self.spec.attendees = [at for at in self.spec.attendees if self._sees_scene(at)]

    def dump_clean_spec(self):
        cleaned = load_problem(self.i)
        cleaned_attendees = []
        before = len(cleaned['attendees'])
        for attendee in cleaned['attendees']:
            point = vm.Vector2(attendee['x'], attendee['y'])
            if self._sees_scene((point, [])):
                cleaned_attendees.append(attendee)
        cleaned['attendees'] = cleaned_attendees
        after = len(cleaned['attendees'])
        print(f'Left {after} from {before} attendees')
        save_problem(self.i, '_clean', cleaned)

    def _sees_scene(self, attendee):
        origin, _ = attendee
        back = origin - self.spec.stage_center

        def norm_angle(base_angle):
            return (base_angle - back.theta + pi_2) % pi_2

        seen_corners = 0
        seen_pillars = 0
        pillar_ends = []
        for (p, d) in self.spec.pillars:
            vec = p - origin
            center_angle = math.asin(d / vec.length)
            angles = sorted([norm_angle(vec.theta - center_angle), norm_angle(vec.theta + center_angle)])
            # special case for the pillar behind us
            if not math.isclose(angles[1] - angles[0], 2 * math.fabs(center_angle)):
                seen_pillars += 1
            pillar_ends.append((angles[0], 1))
            pillar_ends.append((angles[1], -1))

        corner_angles = [(norm_angle((c - origin).theta), 0) for c in self.spec.stage_corners]
        events = sorted(corner_angles + pillar_ends, key=lambda x: (x[0], x[1]))

        for (a, pillar_end) in events:
            seen_pillars += pillar_end
            if not pillar_end:
                seen_corners += 1
            if seen_corners == 4:
                return False
            if seen_corners and not seen_pillars:
                return True
        return False

    def solve(self):
        # filter out "deaf" attendees
        self.dump_clean_spec()
        # Compute initial points for border placement
        # Compute scores per channel
        # Iterate over scores/channels for local optimum(s)
        # Add assignment to the map of assignments
        pass


def solve_all(problems: list[any]):
    for i in sorted(int(s) for s in set(problems)):
        try:
            solve(i)
        except Exception as ex:
            print(ex)
            raise ex


def solve(problem: int):
    print()
    print(f"Solving problem {problem}")
    solver = GreedySolver3(problem)
    solver.solve()


if __name__ == "__main__":
    if len(sys.argv) > 2:
        solve_all(sys.argv[1:])
    elif len(sys.argv) == 2:
        ids = sys.argv[1].split('-')
        start, end = int(ids[0]), int(ids[-1])
        solve_all(list(range(start, end + 1)))
    else:
        print('''Usage:
        python greedy3.py PROBLEM_ID ANOTHER_PROBLEM_ID...
        python greedy3.py PROBLEM_RANGE_START_ID-PROBLEM_RANGE_END_ID
        ''')
        exit(1)
