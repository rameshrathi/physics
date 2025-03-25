import React from 'react';
import Hero from '../components/Hero';

function Solutions() {
  return (
    <main>
      <Hero
        headline="Powerful Tools, Your Way"
        subheadline="We bring open-source SaaS to your business, hassle-free."
        ctaText="Request a Demo"
        ctaAction={() => alert('Contact us for a demo!')}
      />
      <section className="solutions-content">
        <h2>What We Offer</h2>
        <ul>
          <li><strong>Nextcloud</strong>: Secure file storage and sharing.</li>
          <li><strong>SuiteCRM (Soon)</strong>: Customer relationship management.</li>
          <li><strong>Rocket.Chat (Soon)</strong>: Team communication.</li>
        </ul>
        <h2>How It Works</h2>
        <ol>
          <li>We deploy your tool.</li>
          <li>Manage it with our dashboard.</li>
          <li>Get support when you need it.</li>
        </ol>
        <h2>Future Vision</h2>
        <p>AI enhancements like predictive analytics and smart workflows.</p>
      </section>
    </main>
  );
}

export default Solutions;
