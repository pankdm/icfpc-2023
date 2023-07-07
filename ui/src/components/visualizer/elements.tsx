import { Circle, CircleProps } from './primitives'

export const Attendee = ({
  radius = 5,
  color = 'white',
  ...props
}: CircleProps) => <Circle radius={5} color={color} {...props} />
