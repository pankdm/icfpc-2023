import { Circle, CircleProps } from './primitives.3d'

export const Attendee = ({
  radius = 5,
  color = 'white',
  ...props
}: CircleProps) => <Circle radius={radius} color={color} {...props} />

export const Musician = ({
  radius = 5,
  color = 'gold',
  ...props
}: CircleProps) => <Circle radius={radius} color={color} {...props} />
