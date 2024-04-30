// VotingList.js

import React, { useState, useEffect } from 'react';
import { Link } from 'react-router-dom';
import axios from 'axios';
function VotingList() {
  const [votingList, setVotingList] = useState([]);

  useEffect(() => {
    const fetchVotingList = async () => {
      try {
        const response = await axios.get('/voting');
        setVotingList(response.data);
      } catch (error) {
        console.error('Error fetching voting list:', error);
      }
    };

    fetchVotingList();
  }, []);

  // return (
  //   <div>
  //     <h1>Voting List</h1>
  //     <ul>
  //       {votingList.map(voting => (
  //         <li key={voting.id}>
  //           <Link to={`/voting/${voting.id}`}>{voting.title}</Link>
  //         </li>
  //       ))}
  //     </ul>
  //   </div>
  // );
     return (
    <div className="voting-list">
      <h2>Voting List</h2>
      <div className="voting-cards">
        {votingList.map(voting => (
          <div key={voting.id} className="voting-card">
            <h3>{voting.title}</h3>
            <p>{voting.description}</p>
            <Link to={`/voting/${voting.id}`} className="btn">
              View Details
            </Link>
          </div>
        ))}
      </div>
    </div>
  );
}

export default VotingList;

