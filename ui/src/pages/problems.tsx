import { Box, Center, Group, Stack } from '@mantine/core'
import TrafficLight from '../components/TrafficLight'
import { useState } from 'react'

export default function Problems() {
  const [color, setColor] = useState('red')
  const toggleColor = () => {
    setColor({ red: 'yellow', yellow: 'green', green: 'red' }[color] as string)
  }
  return (
    <Group h="100%" pos='relative'>
      <Stack
        h="100%"
        p="md"
        bg="gray.3"
        w={300}
        sx={{ overflow: 'auto', flexShrink: 0 }}
      >
        <Box>Item 1</Box>
        <Box>Item 2</Box>
        <Box>Item 3</Box>
      </Stack>
      <Box sx={{ flex: 1 }}>
        <TrafficLight pos='absolute' top={0} right={0} size='10rem' {...{ [color]: true }} onClick={toggleColor} />
        <Center>
          <Center bg="gray.2" w={'60vmin'} h={'60vmin'}>
            Visualizer
          </Center>
        </Center>
      </Box>
    </Group>
  )
}
