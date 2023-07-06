import { Routes, Route } from 'react-router-dom'
import AppLayout from './components/AppLayout'
import Problems from './pages/problems'
import Leaderboard from './pages/leaderboard'
import Root from './pages/root'

function App() {
  return (
    <Routes>
      <Route path="/" element={<AppLayout />}>
        <Route index element={<Root/>} />
        <Route path="/problems" element={<Problems />} />
        <Route path="/leaderboard" element={<Leaderboard />} />
        <Route path="*" element={<Root />} />
      </Route>
    </Routes>
  )
}

export default App
