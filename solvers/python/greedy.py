from src.utils import load_problem
import math

BORDER_RADIUS = 10


def compute_score(x, y, px, py, taste):
    d2 = (x - px) ** 2 + (y - py) ** 2
    return int(math.ceil(10**6 * taste / d2))


class GreedySolver:
    def __init__(self, spec):
        self.spec = spec

    def _within_sector(self, x, y, px, py):
        dx = px - x
        dy = py - y
        sin_a = dy / math.sqrt(dx**2 + dy**2)
        return sin_a >= 0.5

    def _compute_score(self, x, y, channel):
        total_score = 0
        for p in self.spec["attendees"]:
            px = p["x"]
            py = p["y"]
            if self._within_sector(x, y, px, py):
                total_score += compute_score(x, y, px, py, p["tastes"][channel])
        return total_score

    def _precompute(self, channel):
        y = (
            self.spec["stage_bottom_left"][1]
            + self.spec["stage_height"]
            - BORDER_RADIUS
        )
        x_stage_start = self.spec["stage_bottom_left"][0]
        x_stage_end = x_stage_start + self.spec["stage_width"]
        step = 10
        for x in range(
            int(x_stage_start + BORDER_RADIUS),
            int(x_stage_end - BORDER_RADIUS + 1),
            step,
        ):
            score = self._compute_score(x=float(x), y=y, channel=channel)
            print(f"{x:3} {score}")

    def solve(self):
        n_channels = len(self.spec["attendees"][0]["tastes"])
        for channel in range(n_channels):
            self._precompute(channel)


def solve(i):
    spec = load_problem(i)
    solver = GreedySolver(spec)
    solver.solve()


if __name__ == "__main__":
    solve(23)
