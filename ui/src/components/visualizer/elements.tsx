import { Circle, CircleProps } from './primitives'

export const Pillar = ({
  radius = 5,
  color = '#e72626',
  ...props
}: CircleProps) => <Circle radius={radius} color={color} {...props} />

export const Attendee = ({
  radius = 5,
  color = '#fff',
  ...props
}: CircleProps) => <Circle radius={radius} color={color} {...props} />

export const Musician = ({
  radius = 5,
  color = '#ffd700',
  ...props
}: CircleProps) => <Circle radius={radius} color={color} {...props} />
