import { Box, MantineSize, Paper, PaperProps } from '@mantine/core'
import { Problem, ProblemStats, Solution } from '../../api/types'
import { Rect } from './primitives'
import { Attendee, Musician, Pillar } from './elements'
import { getFullViewBox, getZoomedViewBox } from '../../utils/camera'

export default function Visualizer({
  size,
  problemId,
  problem,
  solution,
  problemStats,
  zoomMode,
  ...props
}: {
  size: MantineSize
  problemId: string | number
  problem: Problem
  problemStats?: ProblemStats | null
  solution?: Solution | null
  zoomMode: String
} & PaperProps) {
  const {
    room_width,
    room_height,
    stage_width,
    stage_height,
    stage_bottom_left: [stage_x, stage_y],
  } = problem
  const rmin = Math.min(room_width, room_height)
  const zoomViewBox =
    problemStats && zoomMode === 'Zoom'
      ? getZoomedViewBox(problemStats)
      : getFullViewBox(problem)
  return (
    <Paper
      w={size}
      h={size}
      shadow="md"
      withBorder
      pos="relative"
      radius={0}
      {...props}
    >
      <Box
        component="svg"
        w="100%"
        h="100%"
        viewBox={zoomViewBox}
        transform="scale(1,-1)"
      >
        {/* stage */}
        <Rect
          x={0}
          y={0}
          width={room_width}
          height={room_height}
          color="#453f3f"
        />
        <Rect
          x={stage_x}
          y={stage_y}
          width={stage_width}
          height={stage_height}
          color="blue.9"
        />

        {/* pillars */}
        {problem.pillars.map((p, idx) => (
          <Pillar key={idx} x={p.center[0]} y={p.center[1]} radius={p.radius} />
        ))}

        {/* attendees */}
        {problem.attendees.map((att, idx) => (
          <Attendee key={idx} x={att.x} y={att.y} radius={rmin * 0.0025} />
        ))}

        {/* musicians */}
        {solution?.placements?.map((p, idx) => (
          <Musician key={idx} x={p.x} y={p.y} />
        ))}
      </Box>
    </Paper>
  )
}
