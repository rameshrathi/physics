import React from 'react';

function BenefitTile({ icon, title, description }) {
  return (
    <div className="benefit-tile">
      <span className="icon">{icon}</span>
      <h3>{title}</h3>
      <p>{description}</p>
    </div>
  );
}

export default BenefitTile;
