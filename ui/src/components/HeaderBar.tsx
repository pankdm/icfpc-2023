import { Group, GroupProps, Text, Title } from '@mantine/core'
import { NavLink } from 'react-router-dom'

export default function HeaderBar({ children, ...props }: GroupProps) {
  return (
    <Group
      position="apart"
      c="white"
      bg="orange.8"
      p="sm"
      sx={{
        '& a': {
          color: 'white',
          textDecoration: 'none',
        },
      }}
      {...props}
    >
      <NavLink to="/">
        <Title order={2} w={150}>
          ICFPC 2023
        </Title>
      </NavLink>
      <Group>
        <NavLink to="/problems">
          {({ isActive }) => (
            <Text fw="bolder" td={isActive ? 'underline' : 'none'}>
              Problems
            </Text>
          )}
        </NavLink>
        <NavLink to="/leaderboard">
          {({ isActive }) => (
            <Text fw="bolder" td={isActive ? 'underline' : 'none'}>
              Leaderboard
            </Text>
          )}
        </NavLink>
      </Group>
      <Group w={150} position='right'>
        <Text fw="bolder">To the moon! 🚀</Text>
      </Group>
    </Group>
  )
}
