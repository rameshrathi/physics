import React, { useState } from 'react';
import './Home.css';
import toolsData from './Data';

const Home = ({ setCurrentPage }) => { // Assuming setCurrentPage is passed as a prop
  const [selectedDomains, setSelectedDomains] = useState([]);
  const [selectedTools, setSelectedTools] = useState([]);

  const domains = [
    'E-commerce', 'Healthcare', 'Education', 'Finance', 'Marketing',
    'Logistics', 'Hospitality', 'Manufacturing', 'Real Estate', 'Media',
    'Technology', 'Retail'
  ];

  const handleDomainSelect = (domain) => {
    setSelectedDomains((prev) =>
      prev.includes(domain) ? prev.filter((d) => d !== domain) : [...prev, domain]
    );
  };

  const handleToolSelect = (tool) => {
    setSelectedTools((prev) =>
      prev.some((t) => t.name === tool.name) ? prev.filter((t) => t.name !== tool.name) : [...prev, tool]
    );
  };

  const handleContinue = () => {
    if (typeof setCurrentPage === 'function') {
      setCurrentPage('contact'); // Navigate to the "Contact" page
      setTimeout(() => {
        window.scrollTo({ top: 0, behavior: 'smooth' }); // Scroll to the top with animation
      }, 0); // Ensure it happens after the page navigation
    } else {
      console.error('setCurrentPage is not a function');
    }
  };

  return (
    <div className="home">
      {/* Hero Section */}
      <section className="hero">
        <h2>Empower Your Business with Open-Source Solutions</h2>
        <p>
          Discover the best open-source tools tailored for your industry. Streamline operations, reduce costs, and drive innovation with trusted software solutions.
        </p>
      </section>

      {/* Main Content Section */}
      <section className="main-content">
        <h2>Select Your Business type</h2>
        <div className="domain-grid">
          {domains.map((domain) => (
            <div
              key={domain}
              className={`tool-card ${selectedDomains.includes(domain) ? 'selected' : ''}`}
              onClick={() => handleDomainSelect(domain)}
            >
              <h3>{domain}</h3>
            </div>
          ))}
        </div>

        {/* Tools Showcase */}
        {selectedDomains.length > 0 && (
          <>
            <h2>Recommended Open-Source Tools</h2>
            <div className="tools-list">
              {selectedDomains.flatMap((domain) =>
                (toolsData[domain]?.primary || []).slice(0, 10).map((tool, index) => (
                  <div
                    key={`${domain}-${index}`}
                    className={`tool-card ${selectedTools.some((t) => t.name === tool.name) ? 'selected' : ''}`}
                    onClick={() => handleToolSelect(tool)}
                  >
                    <h3 className="tool-title">{tool.name}</h3>
                    <p className="tool-description">{tool.description}</p>
                  </div>
                ))
              )}
            </div>

            {/* Continue Button */}
            {selectedTools.length > 0 && (
              <button className="continue-button" onClick={handleContinue}>
                Continue
              </button>
            )}
          </>
        )}
      </section>
    </div>
  );
};

export default Home;