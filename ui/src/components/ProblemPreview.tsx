import { Center, MantineSize, Paper, PaperProps } from '@mantine/core'
import API, { useAPIData } from '../api'
import Preview from './visualizer/Preview'

const ProblemPreview = ({
  size,
  problemId,
  ...props
}: { size: MantineSize; problemId: number } & PaperProps) => {
  const { data: problem } = useAPIData({
    fetch: () => API.getProblem(problemId),
  })
  return (
    <Paper w={size} h={size} bg="gray.8" {...props}>
      <Center h="100%">
        {false && problem && (
          <Preview
            sx={{ flexShrink: 0 }}
            problem={problem as any}
            withBorder={false}
            size={size}
          />
        )}
      </Center>
    </Paper>
  )
}

export default ProblemPreview
