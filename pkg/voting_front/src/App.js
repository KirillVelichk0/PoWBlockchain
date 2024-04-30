// import logo from './logo.svg';
// import './App.css';
//
// function App() {
//   return (
//     <div className="App">
//       <header className="App-header">
//         <img src={logo} className="App-logo" alt="logo" />
//         <p>
//           Edit <code>src/App.js</code> and save to reload.
//         </p>
//         <a
//           className="App-link"
//           href="https://reactjs.org"
//           target="_blank"
//           rel="noopener noreferrer"
//         >
//           Learn React
//         </a>
//       </header>
//     </div>
//   );
// }
//
// export default App;
// App.js

import React, {useState} from 'react';
import { BrowserRouter as Router, Route, Routes, Link } from 'react-router-dom';
import VotingList from './components/VotingList';
import VotingDetails from './components/VotingDetails';
import Login from './components/Login';
import Vote from './components/Vote';
import VotersList from './components/VotersList';
import UserVoteDetails from './components/UserVoteDetails';

function App() {
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [isLogined, setIsLogined] = useState(false);
  const changeLogin = () =>{
    setIsLogined(!isLogined);
  }
  const openModal = () => {
    setIsModalOpen(true);
  };

  const closeModal = () => {
    setIsModalOpen(false);
  };
  return (
    <Router>
      <div className="App">
        <header>
          <button onClick={openModal} className="btn-login">{isLogined ? 'Logout' : 'Login'}</button>
        </header>
        <Routes>
          <Route path="/" element={<VotingList />} />
          <Route path="/voting/:voting_id" element={<VotingDetails></VotingDetails>} />
          <Route path="/voting/:voting_id/vote" element={<Vote></Vote>} />
          <Route path="/voting/:voting_id/voters" element={<VotersList></VotersList>} />
          <Route path="/voting/:voting_id/user_vote/:user_id" element={<UserVoteDetails></UserVoteDetails>} />
        </Routes>
        <Login isOpen={isModalOpen} onClose={closeModal} isLogined={isLogined} changeLoginState={changeLogin} />
      </div>
    </Router>
  );
}

export default App;

