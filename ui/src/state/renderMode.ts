import _ from 'lodash'
import { persistentAtom } from '@nanostores/persistent'

export const $renderMode = persistentAtom<string>(
  'icfpc-2023:render_mode',
  'svg'
)

export const $zoomMode = persistentAtom<string>('icfpc-2023:zoom_mode', 'full')
