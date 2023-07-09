import _ from 'lodash'
import { Helmet } from 'react-helmet'
import { useEffect, useState } from 'react'
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
  SegmentedControl,
  NativeSelect,
  Slider,
  Switch,
} from '@mantine/core'
import { $problem, setProblemId } from '../../state/problem'
import API, { useAPIData } from '../../api'
import TrafficLight from '../../components/TrafficLight'
import Visualizer from '../../components/visualizer/Visualizer'
import ProblemPreview from '../../components/ProblemPreview'
import {
  $previewInstruments,
  $previewInstrumentsMode,
  $zoomMode,
  togglePreviewInstruments,
} from '../../state/renderMode'
import config from '../../config'
import { $userboardZoomMode } from '../../state/userboardDisplayMode'
import { useHotkeys } from '@mantine/hooks'
import { formatNumber } from '../../utils/numbers'

export default function ProblemInspector() {
  const { problemId: problemIdStr } = useParams()

  // State
  const previewsZoomMode = useStore($userboardZoomMode)
  const problemId = parseInt(problemIdStr as any)
  const [solutionId, setSolutionId] = useState<string | null>(null)
  const problem = useStore($problem)
  const zoomMode = useStore($zoomMode)
  const previewInstruments = useStore($previewInstruments)
  const previewInstrumentsMode = useStore($previewInstrumentsMode)
  const [selectedInstrumentId, setSelectedInstrumentId] = useState(0)
  const safeSetSelectedInstrumentId = (v: number) => {
    setSelectedInstrumentId(
      Math.min(Math.max(v, 0), stats ? stats.instruments - 1 : 0)
    )
  }
  const clearSelectedInstrument = () => setSelectedInstrumentId(0)

  // Data
  const { data: problemsData } = useAPIData({
    fetch: API.getProblems,
  })
  const { isLoading } = useAPIData({
    skip: !problemId,
    fetch: () => API.getProblem(problemId as any),
    onSuccess: (_problem) => $problem.set(_problem),
    deps: [problemId],
  })
  const { data: allProblemsStats } = useAPIData({
    fetch: () => API.getProblemsStats(),
    deps: [problemId],
  })
  const { data: stats } = useAPIData({
    fetch: () => API.getProblemStats(problemId),
    deps: [problemId],
  })
  const { data: solutionsResp, refetch: refetchSolutions } = useAPIData({
    fetch: () => API.getProblemSolutions(problemId),
    deps: [problemId],
  })
  const { data: solution, clearData: clearSolutionData } = useAPIData({
    fetch: () => API.getSolution(solutionId as string),
    skip: !solutionId,
    deps: [solutionId],
  })
  const totalInstruments = stats?.instruments ?? 1

  // Cleanup on page change
  const onPageChange = (newProblemId: number) => {
    clearSolutionData()
    clearSelectedInstrument()
    setSolutionId(`loks_best/${newProblemId}.json`)
  }
  useEffect(() => {
    onPageChange(problemId)
  }, [problemId])
  const solutionsSelectOpts = solutionsResp?.solutions?.length
    ? solutionsResp.solutions
    : [{ value: 'none', label: 'No solutions', disabled: true }]

  // Controls
  const toggleZoomMode = () => {
    $zoomMode.set(zoomMode === 'Zoom' ? 'Full' : 'Zoom')
  }
  useHotkeys([
    ['z', () => toggleZoomMode()],
    ['i', () => togglePreviewInstruments()],
    [
      'ArrowLeft',
      () => {
        if (previewInstruments) {
          safeSetSelectedInstrumentId(selectedInstrumentId - 1)
        }
      },
    ],
    [
      'ArrowRight',
      () => {
        if (previewInstruments) {
          safeSetSelectedInstrumentId(selectedInstrumentId + 1)
        }
      },
    ],
    ['n', () => $previewInstrumentsMode.set('linear')],
    ['l', () => $previewInstrumentsMode.set('log')],
  ])

  return (
    <Group h="100%" pos="relative">
      <Helmet>
        <title>
          Problem {problemId.toString()} - {config.HTML_TITLE}
        </title>
      </Helmet>
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
                      problemStats={allProblemsStats?.problems[_problemId]}
                      zoomMode={previewsZoomMode}
                    />
                    # {_problemId}
                  </Group>
                }
              />
            </Link>
          ))}
      </Stack>
      <Box sx={{ flex: 1 }}>
        <Stack
          pos="absolute"
          sx={{ zIndex: 10000 }}
          top={0}
          right={0}
          align="center"
        >
          <TrafficLight size="10rem" red={isLoading} green={!isLoading} />
          <SegmentedControl
            color="orange.4"
            data={[
              {
                value: 'Zoom',
                label: '(Z)oom',
              },
              {
                value: 'Full',
                label: 'Full',
              },
            ]}
            value={zoomMode}
            onChange={(v) => $zoomMode.set(v)}
          />
        </Stack>
        <Center>
          <Stack align="center" spacing={0}>
            <Title order={2}>Problem {problemId}</Title>
            <NativeSelect
              placeholder="Select solution"
              data={solutionsSelectOpts}
              value={solutionId || ''}
              onChange={(ev) => setSolutionId(ev.currentTarget.value)}
              onFocus={refetchSolutions}
            />
            <Space h="xl" />
            <Group position="center">
              <Stack>
                <Box w={'70vmin'} h={'70vmin'} pos="relative">
                  <LoadingOverlay visible={isLoading} overlayBlur={2} />
                  {problem && (
                    <Visualizer
                      bg="black"
                      size="70vmin"
                      problemId={problemId}
                      problem={problem}
                      previewInstrumentId={
                        previewInstruments ? selectedInstrumentId : -1
                      }
                      problemStats={stats}
                      solution={solution}
                      zoomMode={zoomMode}
                    />
                  )}
                </Box>
                <Stack w="100%" spacing={'xs'}>
                  <Group spacing={'xs'}>
                    <Switch
                      color="orange.4"
                      checked={previewInstruments}
                      onChange={togglePreviewInstruments}
                      label={
                        <Text w="11rem">
                          Preview (i)nstrument: {selectedInstrumentId}
                        </Text>
                      }
                    />
                    <Text size="sm">Mode:</Text>
                    <SegmentedControl
                      color={previewInstruments ? 'orange.4' : 'gray.0'}
                      size="xs"
                      disabled={!previewInstruments}
                      value={previewInstrumentsMode}
                      data={[
                        {
                          value: 'linear',
                          label: '(N)ormal',
                        },
                        {
                          value: 'log',
                          label: '(L)og10',
                        },
                      ]}
                      onChange={$previewInstrumentsMode.set}
                    />
                  </Group>
                  <Slider
                    disabled={!previewInstruments}
                    min={0}
                    max={totalInstruments - 1}
                    value={selectedInstrumentId}
                    onChange={setSelectedInstrumentId}
                    labelTransition="fade"
                    labelTransitionDuration={150}
                  />
                </Stack>
              </Stack>
              <Space h="xl" />
              <Stack spacing={0}>
                <Text size="sm">Best score: {formatNumber(stats?.score)}</Text>
                <Text size="sm">
                  Estimated max: {formatNumber(stats?.estimated_max)}
                </Text>
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
            </Group>
          </Stack>
        </Center>
      </Box>
    </Group>
  )
}
