import _ from 'lodash'
import { Link } from 'react-router-dom'
import { Container } from '@mantine/core'
import { MantineReactTable as Table, MRT_ColumnDef } from 'mantine-react-table'
import API, { useAPIData } from '../../api'
import { ProblemStats } from '../../api/types'

type ProblemStatsTableColumns =
  | 'id'
  | 'stageSize'
  | 'instruments'
  | 'musicians'
  | 'attendees'
  | 'score'

const columns = {
  id: {
    id: 'id',
    accessorFn: (row) =>
      row.id && <Link to={`/problems/${row.id}`}># {row.id}</Link>,
    header: 'Problem',
  },
  ...Object.fromEntries(
    [
      ['instruments', 'Instruments'],
      ['musicians', 'Musicians'],
      ['attendees', 'Attendees'],
      ['score', 'Score'],
    ].map(([accessorKey, header]) => [
      accessorKey,
      {
        accessorKey,
        header,
      },
    ]) as any
  ),
  stageSize: {
    id: 'stageSize',
    header: 'Stage Size',
    accessorFn: (originalRow) =>
      originalRow.stage_width &&
      `${originalRow.stage_width} x ${originalRow.stage_height}`,
  },
} as Record<
  ProblemStatsTableColumns,
  MRT_ColumnDef<{ id: string } & ProblemStats>
>

export default function Problems() {
  const { isLoading, data: stats } = useAPIData({
    fetch: () => API.getProblemsStats(),
    deps: [],
  })
  const data = _.map(stats?.problems, (data, id) => ({
    id,
    ...data,
    stageSize: `${data.stage_width} x ${data.stage_height}`,
  }))
  return (
    <Container size="xl">
      <Table
        mantinePaperProps={{
          radius: 0,
        }}
        columns={[
          columns.id,
          columns.instruments,
          columns.musicians,
          columns.attendees,
          columns.stageSize,
          columns.score,
        ]}
        enableStickyHeader
        enablePagination={false}
        enableDensityToggle={false}
        enableColumnActions={false}
        enableHiding={false}
        state={{ isLoading, density: 'xs' }}
        data={data}
      />
    </Container>
  )
}
