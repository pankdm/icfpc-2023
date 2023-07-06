import { Center, Stack, Text, Title } from '@mantine/core'
import { Link } from 'react-router-dom'

export default function Root() {
  return (
    <Center h="100%">
      <Stack align='center'>
        <Title order={2}>Welcome to ICFPC-2023!</Title>
        <Text>Team: Snakes, Monkeys and Two Smoking Lambdas</Text>
        <Link to="/problems">
          <Text>Go solve some problems</Text>
        </Link>
      </Stack>
    </Center>
  )
}
