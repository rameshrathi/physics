import React from 'react';

function Hero({ headline, subheadline, ctaText, ctaAction }) {
  return (
    <section className="hero">
      <h1>{headline}</h1>
      <p>{subheadline}</p>
      <button onClick={ctaAction}>{ctaText}</button>
    </section>
  );
}

export default Hero;
