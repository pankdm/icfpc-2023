import _ from 'lodash'
import { useStore } from '@nanostores/react'
import {
  Box,
  Center,
  Code,
  Group,
  NavLink,
  Stack,
  Text,
  Title,
} from '@mantine/core'
import { $problem, $problemId, setProblemId } from '../state/problem'
import API, { useAPIData } from '../api'
import TrafficLight from '../components/TrafficLight'

export default function Problems() {
  const problemId = useStore($problemId)
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
            <NavLink
              key={_problemId}
              onClick={() => setProblemId(_problemId)}
              active={_problemId === problemId}
              label={<Box>Problem {_problemId}</Box>}
            />
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
          <Stack align="center">
            <Title order={2}>Problem {problemId}</Title>
            <Box
              bg="gray.2"
              w={'60vmin'}
              h={'60vmin'}
              sx={{ overflow: 'auto', whiteSpace: 'pre' }}
            >
              <Code w="100%" h="100%">
                {problem &&
                  JSON.stringify(
                    {
                      ...problem,
                      musicians: problem.musicians.slice(0, 10),
                      attendees: problem.attendees
                        .slice(0, 10)
                        .map((a: any) => ({
                          ...a,
                          tastes: a.tastes.slice(0, 10),
                        })),
                    },
                    undefined,
                    2
                  )}
              </Code>
            </Box>
            <Text size="sm">
              Data preview: arrays trimmed down to 10 elements.
            </Text>
          </Stack>
        </Center>
      </Box>
    </Group>
  )
}
