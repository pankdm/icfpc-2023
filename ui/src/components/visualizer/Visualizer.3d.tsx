import { MantineSize, Paper, PaperProps } from '@mantine/core'
import { Problem } from '../../api/types'
import { Canvas, useFrame, useThree } from '@react-three/fiber'
import { Rect } from './primitives.3d'
import { Attendee } from './elements.3d'
import { useState } from 'react'

const CameraPositioner = ({
  roomWidth,
  roomHeight,
}: {
  roomWidth: number
  roomHeight: number
}) => {
  const { camera, viewport } = useThree()
  const rmax = Math.max(roomWidth, roomHeight)
  useFrame(() => {
    const canvas = document.querySelector('#visualizer')
    camera.position.x = roomWidth / 2
    camera.position.y = roomHeight / 2
    if (canvas) {
      const correctZoom =
        viewport.aspect >= 1
          ? canvas.clientWidth / rmax
          : canvas.clientHeight / rmax
      if (correctZoom !== camera.zoom) {
        camera.zoom = correctZoom
        camera.updateProjectionMatrix()
      }
    }
  })
  return null
}

export default function Visualizer3D({
  size,
  problem,
  ...props
}: { size: MantineSize; problem: Problem } & PaperProps) {
  const {
    room_width,
    room_height,
    stage_width,
    stage_height,
    stage_bottom_left: [stage_x, stage_y],
  } = problem
  const rmin = Math.min(room_width, room_height)
  const [stageHovered, setStageHovered] = useState(false)
  const [stageHeld, setStageHeld] = useState(false)
  return (
    <Paper
      w={size}
      h={size}
      shadow="md"
      withBorder
      pos="relative"
      radius={0}
      onResize={console.log}
      {...props}
    >
      <Canvas
        id="visualizer"
        orthographic
        camera={{
          position: [0, 0, 100],
          left: -50,
          right: 50,
          top: 50,
          bottom: -50,
          near: 1,
          far: 100000,
        }}
      >
        <CameraPositioner roomWidth={room_width} roomHeight={room_height} />
        <ambientLight intensity={1} color={'#fff'} />
        {/* room */}
        <Rect
          x={0}
          y={0}
          z={0}
          width={room_width}
          height={room_height}
          color="#444"
        />
        {/* stage */}
        <Rect
          onPointerEnter={() => setStageHovered(true)}
          onPointerLeave={() => setStageHovered(false)}
          onPointerDown={() => setStageHeld(true)}
          onPointerUp={() => setStageHeld(false)}
          x={stage_x}
          y={stage_y}
          z={1}
          width={stage_width}
          height={stage_height}
          color={
            (stageHeld && '#274672') || (stageHovered && '#284c7e') || '#3163aa'
          }
        />

        {/* attendees */}
        {problem.attendees.map((att, idx) => (
          <Attendee
            key={idx}
            x={att.x}
            y={att.y}
            radius={rmin * 0.0025}
            z={2}
          />
        ))}
      </Canvas>
    </Paper>
  )
}
