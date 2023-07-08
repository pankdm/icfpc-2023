import _ from 'lodash'
import { Link, useParams } from 'react-router-dom'
import { useStore } from '@nanostores/react'
import {
  Box,
  Center,
  Group,
  NavLink,
  Space,
  Stack,
  Text,
  Title,
  LoadingOverlay,
} from '@mantine/core'
import { $problem, setProblemId } from '../../state/problem'
import API, { useAPIData } from '../../api'
import TrafficLight from '../../components/TrafficLight'
import Visualizer3D from '../../components/visualizer/Visualizer.3d'
import ProblemPreview from '../../components/ProblemPreview'

export default function ProblemInspector() {
  const { problemId: problemIdStr } = useParams()
  const problemId = parseInt(problemIdStr as any)
  const problem = useStore($problem)
  const { data: problemsData } = useAPIData({
    fetch: API.getProblems,
  })
  const { isLoading } = useAPIData({
    skip: !problemId,
    fetch: () => API.getProblem(problemId as any),
    onSuccess: (_problem) => $problem.set(_problem),
    deps: [problemId],
  })
  const { data: stats } = useAPIData({
    fetch: () => API.getProblemStats(problemId),
    deps: [problemId],
  })
  return (
    <Group h="100%" pos="relative">
      <Stack
        h="100%"
        bg="gray.3"
        w={200}
        spacing={0}
        sx={{ overflow: 'auto', flexShrink: 0 }}
      >
        {problemsData?.problems &&
          _.map(problemsData.problems, (_problemId) => (
            <Link key={_problemId} to={`/problems/${_problemId}`}>
              <NavLink
                key={_problemId}
                onClick={() => setProblemId(_problemId)}
                active={_problemId === problemId}
                p={4}
                label={
                  <Group>
                    <ProblemPreview
                      size="4rem"
                      problemId={_problemId}
                      radius={0}
                    />
                    # {_problemId}
                  </Group>
                }
              />
            </Link>
          ))}
      </Stack>
      <Box sx={{ flex: 1 }}>
        <TrafficLight
          sx={{ zIndex: 10000 }}
          pos="absolute"
          top={0}
          right={0}
          size="10rem"
          red={isLoading}
          green={!isLoading}
        />
        <Center>
          <Stack align="center" spacing={0}>
            <Title order={2}>Problem {problemId}</Title>
            <Space h="xl" />
            <Box w={'70vmin'} h={'70vmin'} pos="relative">
              <LoadingOverlay visible={isLoading} overlayBlur={2} />
              {problem && (
                <Visualizer3D
                  bg="black"
                  size="70vmin"
                  problemId={problemId}
                  problem={problem}
                />
              )}
            </Box>
            <Space h="xl" />
            <Text size="sm">
              Room: {problem?.room_width} x {problem?.room_height}
            </Text>
            <Text size="sm">
              Scene: {problem?.stage_width} x {problem?.stage_height}
            </Text>
            <Text size="sm">Instruments: {stats?.instruments}</Text>
            <Text size="sm">Musicians: {stats?.musicians}</Text>
            <Text size="sm">Attendees: {stats?.attendees}</Text>
          </Stack>
        </Center>
      </Box>
    </Group>
  )
}
