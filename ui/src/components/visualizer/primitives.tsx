import _ from 'lodash'
import {
  Box as MantineBox,
  MantineColor,
  MantineSize,
  PaperProps,
  useMantineTheme,
} from '@mantine/core'

export type RectProps = {
  x: number
  y: number
  width: number
  height: number
  color?: MantineColor
  borderRadius?: MantineSize
} & PaperProps
export const Rect = ({
  x,
  y,
  width,
  height,
  color = 'white',
  borderRadius,
  ...props
}: RectProps) => {
  const theme = useMantineTheme()
  return (
    <MantineBox
      component="rect"
      x={x}
      y={y}
      width={width}
      height={height}
      fill={(_.get(theme.colors, color) || color) as any}
      rx={(borderRadius && theme.radius[borderRadius]) || borderRadius}
      {...props}
    />
  )
}

export type CircleProps = {
  x?: number
  y?: number
  radius?: number
  color?: MantineColor
} & PaperProps
export const Circle = ({
  x,
  y,
  radius,
  color = 'white',
  ...props
}: CircleProps) => {
  const theme = useMantineTheme()
  return (
    <MantineBox
      component="circle"
      cx={x}
      cy={y}
      r={radius}
      fill={(_.get(theme.colors, color) || color) as any}
      {...props}
    />
  )
}
