import _ from 'lodash'
import { Link } from 'react-router-dom'
import { Helmet } from 'react-helmet'
import { Container, Group, Space } from '@mantine/core'
import { MantineReactTable as Table, MRT_ColumnDef } from 'mantine-react-table'
import API, { useAPIData } from '../../api'
import { ProblemStats } from '../../api/types'
import { formatNumber, formatNumberExp } from '../../utils/numbers'
import ProblemPreview from '../../components/ProblemPreview'
import config from '../../config'

const columns = {
  id: {
    id: 'id',
    accessorFn: (row) =>
      row.id && (
        <Link to={`/problems/${row.id}`}>
          <Group p={0} spacing={0}>
            <ProblemPreview problemId={row.id} size="8rem" />
            <Space w="xs" /># {row.id}
          </Group>
        </Link>
      ),
    header: 'Problem',
    size: 140,
  },
  ...Object.fromEntries(
    [
      ['instruments', 'Instruments'],
      ['musicians', 'Musicians'],
      ['attendees', 'Attendees'],
      ['pillars', 'Pillars'],
    ].map(([accessorKey, header]) => [
      accessorKey,
      {
        accessorKey,
        header,
        size: 90,
      },
    ]) as any
  ),
  score: {
    id: 'score',
    header: 'Score',
    size: 120,
    accessorKey: "score",
    Cell: ({ cell }) => (<span> {formatNumber(cell.getValue<number>())} </span>),
  },
  estimatedMax: {
    id: 'estimatedMax',
    header: 'Estimated Max',
    size: 120,
    accessorKey: "estimated_max",
    Cell: ({ cell }) => (<span> {formatNumber(cell.getValue<number>())} </span>),
  },
  percentOfMax: {
    id: 'percentOfMax',
    header: 'Percent of Max',
    size: 90,
    accessorFn: (originalRow) => `${(100 * (originalRow.score ?? 0) / (originalRow.estimated_max || 1)).toFixed(1)}%`,
  },
  deltaWithMax: {
    id: 'deltaWithMax',
    header: 'Delta',
    size: 90,
    accessorFn: (originalRow) => `${formatNumberExp((originalRow.estimated_max ?? 0) - (originalRow.score ?? 0))}`,
    sortingFn: (rowA, rowB, columnId) => (parseFloat(rowA.getValue<string>(columnId)) < parseFloat(rowB.getValue<string>(columnId)) ? -1 : 1),
  },
  stageSize: {
    id: 'stageSize',
    header: 'Stage Size',
    size: 90,
    accessorFn: (originalRow) =>
      originalRow.stage_width &&
      `${originalRow.stage_width} x ${originalRow.stage_height}`,
  },
} as Record<string, MRT_ColumnDef<{ id: string } & ProblemStats>>

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
    <Container maw={1200} h="100%" p={0}>
      <Helmet>
        <title>Problems - {config.HTML_TITLE}</title>
      </Helmet>
      <Table
        mantinePaperProps={{
          radius: 0,
          mah: '100%',
        }}
        columns={[
          columns.id,
          columns.instruments,
          columns.musicians,
          columns.attendees,
          columns.pillars,
          columns.stageSize,
          columns.score,
          columns.estimatedMax,
          columns.percentOfMax,
          columns.deltaWithMax,
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
