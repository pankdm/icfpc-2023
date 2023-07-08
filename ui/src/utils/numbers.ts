export function formatNumber(num: number | null) {
  return num?.toLocaleString('en-US')
}
export function formatNumberExp(num: number | null) {
  return num?.toExponential(3)
}
