import React from 'react';

function Pricing() {
  const plans = [
    { name: 'Basic', price: '$300 one-time', description: 'Setup + 1 tool (e.g., Nextcloud).' },
    { name: 'Standard', price: '$500 one-time + $30/month', description: 'Setup, maintenance, support.' },
    { name: 'Premium (Soon)', price: '$700 one-time + $50/month', description: 'AI features included.' },
  ];

  return (
    <main>
      <section className="pricing">
        <h1>Simple, Transparent Pricing</h1>
        <div className="pricing-grid">
          {plans.map((plan, index) => (
            <div key={index} className="pricing-card">
              <h2>{plan.name}</h2>
              <p className="price">{plan.price}</p>
              <p>{plan.description}</p>
              <button onClick={() => alert('Get started today! Contact us.')}>Get Started</button>
            </div>
          ))}
        </div>
        <p className="note">Custom plans available for multiple tools.</p>
      </section>
    </main>
  );
}

export default Pricing;
