import { Box, Center, Group, Stack } from '@mantine/core'

export default function Problems() {
  return (
    <Group h="100%">
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
        <Center>
          <Center bg='gray.2' w={'60vmin'} h={'60vmin'}>Visualizer</Center>
        </Center>
      </Box>
    </Group>
  )
}
