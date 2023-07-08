import { useEffect, useState } from 'react'
import {
  Problem,
  ProblemStats,
  Problems,
  ProblemsStats,
  Scoreboard,
  Userboard,
} from './types'

const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:8000'

const fetchAPI = async (
  path: string,
  requestOpts: Omit<RequestInfo, 'url'> = {}
) =>
  fetch(`${API_URL}${path}`, {
    ...requestOpts,
  })
    .then((r) => r.json())
    .catch((err) => {
      console.error(err)
      throw err
    })

const API = {
  getScoreboard: async () => fetchAPI(`/scoreboard`) as Promise<Scoreboard>,
  getUserboard: async () => fetchAPI(`/userboard`) as Promise<Userboard>,
  getProblems: async () => fetchAPI(`/problems`) as Promise<Problems>,
  getProblemsStats: async () =>
    fetchAPI(`/problems/stats`) as Promise<ProblemsStats>,
  getProblemStats: async (problemId: string | number) =>
    fetchAPI(`/problems/${problemId}/stats`) as Promise<ProblemStats>,
  getProblem: async (problemId: number | string) =>
    fetchAPI(`/problems/${problemId}`) as Promise<Problem>,
  uploadPreview: async (problemId: number | string, imageBlob: Blob) =>
    fetchAPI(`/problems/${problemId}/preview`, {
      method: 'POST',
      body: imageBlob,
    }) as Promise<Problem>,
}

window.API = API

export function useAPIData<T extends Record<string, any>>({
  fetch,
  beforeFetch,
  onSuccess,
  skip = false,
  deps = [],
}: {
  fetch: () => Promise<T>
  beforeFetch?: () => any
  onSuccess?: (data: T) => any
  skip?: boolean | (() => boolean)
  deps?: any[]
}) {
  const [isLoading, setIsLoading] = useState(false)
  const [error, setError] = useState<Error | null>(null)
  const [data, setData] = useState<T | null>(null)
  useEffect(() => {
    if ((skip as any)?.call ? (skip as () => boolean)() : skip) {
      return
    }
    setIsLoading(true)
    setError(null)
    beforeFetch?.()
    fetch()
      .then((result) => {
        setData(result)
        onSuccess?.(result)
      })
      .catch((err) => {
        setError(err)
      })
      .finally(() => {
        setIsLoading(false)
      })
  }, [...deps])
  return { isLoading, error, data }
}

export default API
