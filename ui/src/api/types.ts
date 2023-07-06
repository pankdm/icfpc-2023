export type Scoreboard = {
  frozen: boolean
  scoreboard: {
    username: string
    score: number
  }[]
  updated_at: string
}
