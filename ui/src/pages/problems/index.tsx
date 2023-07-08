import _ from 'lodash'
import { Link, useNavigate } from 'react-router-dom'
import {
  Box,
  Center,
  Container,
  Group,
  NavLink,
  Space,
  Stack,
  Text,
  Title,
} from '@mantine/core'
import { MantineReactTable as Table } from 'mantine-react-table'

import API, { useAPIData } from '../../api'
export default function Problems() {
  const navigate = useNavigate()
  const { isLoading, data: stats } = useAPIData({
    fetch: () => API.getProblemsStats(),
    deps: [],
  })
  const data = _.map(stats?.problems, (data, id) => ({
    id,
    ...data,
  }))
  return (
    <Container size="xl">
      <Table
        mantinePaperProps={{
          radius: 0,
        }}
        columns={[
          {
            id: 'id',
            accessorFn: (row) =>
              row.id && <Link to={`/problems/${row.id}`}># {row.id}</Link>,
            header: 'Problem',
          },
          ...([
            ['instruments', 'Instruments'],
            ['musicians', 'Musicians'],
            ['attendees', 'Attendees'],
          ].map(([accessorKey, header]) => ({
            accessorKey,
            header,
          })) as any),
          {
            id: 'stageSize',
            header: 'Stage Size',
            accessorFn: (originalRow) =>
              originalRow.stage_width &&
              `${originalRow.stage_width} x ${originalRow.stage_height}`,
          },
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
