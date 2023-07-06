import { Box, Container } from '@mantine/core'
import { MantineReactTable as Table } from 'mantine-react-table'

export default function Leaderboard() {
  return (
    <Box w="100%" h="100%">
      <Container>
        <Table
          columns={[
            {
              accessorKey: 'name',
              header: 'Name',
            },
            {
              accessorKey: 'score',
              header: 'Score',
            },
          ]}
          enablePagination={false}
          enableHiding={false}
          enableFullScreenToggle={false}
          enableColumnActions={false}
          data={[
            { name: 'Player 1', score: 1337 },
            { name: 'Player 2', score: 560 },
            { name: 'Player 3', score: 1234 },
          ]}
        />
      </Container>
    </Box>
  )
}
