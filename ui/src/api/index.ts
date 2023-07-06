import { useEffect, useState } from 'react'
import { Scoreboard } from './types'

const API_URL = 'http://localhost:8000'

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
}

export function useAPIData<T extends Record<string, any>>({
  fetch,
  skip = false,
  deps = [],
}: {
  fetch: () => Promise<any>
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
    fetch()
      .then((result) => {
        setData(result)
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
