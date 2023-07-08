import { Color } from 'three'

export type RectProps = {
  x: number
  y: number
  z: number
  width: number
  height: number
  color?: string | Color
}
export const Rect = ({
  x = 0,
  y = 0,
  z = 0,
  width = 1,
  height = 1,
  color = 'white',
}: RectProps) => {
  return (
    <group position={[x, y, z]} scale={[width, height, 1]}>
      <mesh position={[0.5, 0.5, 0]}>
        <boxGeometry args={[1, 1, 1]} />
        <meshStandardMaterial color={color} />
      </mesh>
    </group>
  )
}

export type CircleProps = {
  x?: number
  y?: number
  z?: number
  radius?: number
  color?: string | Color
}
export const Circle = ({
  x = 0,
  y = 0,
  z = 0,
  radius = 1,
  color = 'white',
}: CircleProps) => {
  return (
    <group position={[x, y, z]} scale={[radius, radius, 1]}>
      <mesh>
        <circleGeometry args={[1, 16]} />
        <meshStandardMaterial color={color} />
      </mesh>
    </group>
  )
}
