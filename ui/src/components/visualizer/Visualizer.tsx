import { Box, MantineSize, Paper, PaperProps } from '@mantine/core'
import { Problem, ProblemStats, Solution } from '../../api/types'
import { Rect } from './primitives'
import { Attendee, Musician, Pillar } from './elements'
import { getFullViewBox, getZoomedViewBox } from '../../utils/camera'
import { API_URL } from '../../api'
import { $previewInstrumentsMode } from '../../state/renderMode'
import { useStore } from '@nanostores/react'

const getPreviewImageURL = (
  problemId: string | number,
  instrumentId: number,
  mode: 'linear' | 'log'
) => `${API_URL}/problems/${problemId}/instruments/${instrumentId}/lut/${mode}`

export default function Visualizer({
  size,
  problemId,
  previewInstrumentId = 0,
  problem,
  solution,
  problemStats,
  zoomMode,
  ...props
}: {
  size: MantineSize
  problemId: string | number
  problem: Problem
  previewInstrumentId?: number
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
  const previewInstrumentsMode = useStore($previewInstrumentsMode)
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
        viewBox={zoomViewBox.join(' ')}
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
        {previewInstrumentId >= 0 &&
          previewInstrumentsMode !== 'musicians_only' && (
            <image
              x={stage_x}
              y={stage_y}
              width={stage_width}
              height={stage_height}
              transform={`scale(1,-1) translate(0,${-stage_height})`}
              href={getPreviewImageURL(
                problemId,
                previewInstrumentId,
                previewInstrumentsMode
              )}
            />
          )}

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
          <Musician
            key={idx}
            x={p.x}
            y={p.y}
            dimmed={
              previewInstrumentId >= 0 &&
              problem.musicians[idx] !== previewInstrumentId
            }
          />
        ))}
      </Box>
    </Paper>
  )
}
