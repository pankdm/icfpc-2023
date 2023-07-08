import { Image, MantineSize, Paper, PaperProps } from '@mantine/core'
import { API_URL } from '../api'

const ProblemPreview = ({
  size,
  problemId,
  ...props
}: { size: MantineSize; problemId: number } & PaperProps) => {
  return (
    <Paper w={size} h={size} bg="gray.8" {...props}>
      <Image src={`${API_URL}/problems/${problemId}/preview`} />
    </Paper>
  )
}

export default ProblemPreview
