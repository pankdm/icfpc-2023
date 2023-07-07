export type Scoreboard = {
  frozen: boolean
  scoreboard: {
    username: string
    score: number
  }[]
  updated_at: string
}

export type Problems = {
  problems: number[]
}

export type Problem = Record<string, any>
