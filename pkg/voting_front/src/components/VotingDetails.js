// VotingDetails.js

import React, { useState, useEffect } from 'react';
import { useParams } from 'react-router-dom';
import axios from 'axios';

function VotingDetails() {
  const { voting_id } = useParams();
  const [votingDetails, setVotingDetails] = useState(null);

  useEffect(() => {
    const fetchVotingDetails = async () => {
      try {
        const response = await axios.get(`/voting/${voting_id}`);
        setVotingDetails(response.data);
      } catch (error) {
        console.error('Error fetching voting details:', error);
      }
    };

    fetchVotingDetails();
  }, [voting_id]);

  if (!votingDetails) return <div>Loading...</div>;

  return (
    <div>
      <h2>{votingDetails.title}</h2>
      <p>{votingDetails.description}</p>
      {/* Render options or any other details */}
    </div>
  );
}

export default VotingDetails;

