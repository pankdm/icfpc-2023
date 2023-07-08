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
import Visualizer from '../../components/visualizer/Visualizer'
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
            <Link to={`/problems/${_problemId}`}>
              <NavLink
                key={_problemId}
                onClick={() => setProblemId(_problemId)}
                active={_problemId === problemId}
                label={
                  <Group>
                    <ProblemPreview size="2rem" problemId={_problemId} />
                    Problem {_problemId}
                  </Group>
                }
              />
            </Link>
          ))}
      </Stack>
      <Box sx={{ flex: 1 }}>
        <TrafficLight
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
            <Center w={'70vmin'} h={'70vmin'} pos="relative">
              <LoadingOverlay visible={isLoading} overlayBlur={2} />
              <Box sx={{ flexGrow: 0, flexShrink: 0 }}>
                {problem && (
                  <Visualizer size="70vmin" mah="100%" problem={problem} />
                )}
              </Box>
            </Center>
            <Space h="xl" />
            <Text size="sm">
              Room: {problem?.room_width} x {problem?.room_height}
            </Text>
            <Text size="sm">
              Scene: {problem?.stage_width} x {problem?.stage_height}
            </Text>
            <Text size="sm">Attendees: {problem?.attendees.length}</Text>
            <Text size="sm">Musicians: {problem?.musicians.length}</Text>
            <Text size="sm">Tastes: {problem?.attendees[0].tastes.length}</Text>
          </Stack>
        </Center>
      </Box>
    </Group>
  )
}
