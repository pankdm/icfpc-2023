export type Scoreboard = {
  frozen: boolean
  scoreboard: {
    username: string
    score: number
  }[]
  updated_at: string
}

export type Userboard = {
  problems: {
    id: number
    score: number | null
  }
}

export type Problems = {
  problems: number[]
}

type InstrumentId = number
type InstrumentPreference = number
type Attendee = {
  x: number
  y: number
  tastes: InstrumentPreference[]
}
type Pillar = {
  x: number
  y: number
  radius?: number
}

export type Problem = {
  room_width: number
  room_height: number
  stage_width: number
  stage_height: number
  stage_bottom_left: [number, number]
  musicians: InstrumentId[]
  attendees: Attendee[]
  pillars: Pillar[]
}
