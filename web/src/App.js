import React, { useState } from 'react';
import './styles.css';
import Header from './components/Header';
import Footer from './components/Footer';
import Home from './pages/Home';
import Solutions from './pages/Solutions';
import Pricing from './pages/Pricing';
import Contact from './pages/Contact';

const App = () => {
  const [currentPage, setCurrentPage] = useState('Home');

  const renderPage = () => {
    switch (currentPage) {
      case 'solutions':
        return <Solutions />;
      case 'pricing':
        return <Pricing />;
      case 'contact':
        return <Contact />;
      default:
        return <Home setCurrentPage={setCurrentPage} />;
    }
  };

  return (
    <div>
      <Header setCurrentPage={setCurrentPage} currentPage={currentPage} />
      <div className="app">
        {renderPage()}
      </div>
      <Footer />
    </div>
  );
};

export default App;
