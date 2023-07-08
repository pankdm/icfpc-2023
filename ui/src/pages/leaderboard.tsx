import { Box, Container } from '@mantine/core'
import { MantineReactTable as Table } from 'mantine-react-table'
import API, { useAPIData } from '../api'
import { Scoreboard } from '../api/types'
import { formatNumber } from '../utils/numbers'

export default function Leaderboard() {
  const { data, isLoading } = useAPIData<Scoreboard>({
    fetch: () => API.getScoreboard(),
  })
  return (
    <Box w="100%" h="100%" sx={{ overflow: 'auto' }}>
      <Container maw={1200} p={0} h="100%">
        <Table
          enableRowNumbers
          mantinePaperProps={{
            radius: 0,
            mah: '100%',
          }}
          columns={[
            {
              accessorKey: 'username',
              header: 'Name',
            },
            {
              accessorKey: 'score',
              header: 'Score',
              accessorFn: (row) => formatNumber(row.score),
            },
            {
              accessorFn: (row) => row.score?.toExponential(3),
              id: 'score-exp',
              header: 'Score Exp',
            },
          ]}
          enablePagination={false}
          enableHiding={false}
          enableFullScreenToggle={false}
          enableColumnActions={false}
          enableStickyHeader
          enableDensityToggle={false}
          state={{ isLoading, density: 'xs' }}
          data={data?.scoreboard || []}
        />
      </Container>
    </Box>
  )
}
