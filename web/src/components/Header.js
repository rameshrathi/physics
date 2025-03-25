import React from 'react';
import './Header.css'; // Ensure you have a CSS file for styling
import logo from '../assets/logo.png'; // Import the logo image

const Header = ({ setCurrentPage, currentPage }) => {
  return (
    <header className="header">
      <div className="logo-container">
        <img src={logo} alt="RKLabs Logo" className="logo" />
        <span className="logo-text">RKLabs</span>
      </div>
      <nav>
        <button onClick={() => setCurrentPage('home')} className={currentPage === 'home' ? 'active' : ''}>
          Home
        </button>
        <button onClick={() => setCurrentPage('solutions')} className={currentPage === 'solutions' ? 'active' : ''}>
          Solutions
        </button>
        <button onClick={() => setCurrentPage('pricing')} className={currentPage === 'pricing' ? 'active' : ''}>
          Pricing
        </button>
        <button onClick={() => setCurrentPage('contact')} className={currentPage === 'contact' ? 'active' : ''}>
          Contact
        </button>
      </nav>
    </header>
  );
};

export default Header;