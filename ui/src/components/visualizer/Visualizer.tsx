import { Box, MantineSize, Paper, PaperProps } from '@mantine/core'
import { Problem, Solution } from '../../api/types'
import { Rect } from './primitives'
import { Attendee, Musician, Pillar } from './elements'

export default function Visualizer({
  size,
  problemId,
  problem,
  solution,
  ...props
}: {
  size: MantineSize
  problemId: string | number
  problem: Problem
  solution?: Solution | null
} & PaperProps) {
  const {
    room_width,
    room_height,
    stage_width,
    stage_height,
    stage_bottom_left: [stage_x, stage_y],
  } = problem
  const rmin = Math.min(room_width, room_height)
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
        viewBox={`0 0 ${room_width} ${room_height}`}
        // viewBox={`${stage_x - stage_width * 0.1} ${stage_y - stage_height * 0.1} ${stage_width * 1.1} ${stage_height * 1.1}`}
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
