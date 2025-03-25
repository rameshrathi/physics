import React, { useState } from 'react';

function Contact() {
  const [formData, setFormData] = useState({
    email: '',
    phone: '',
    name: '',
    location: '',
    requirements: '',
  });
  const [errors, setErrors] = useState({});
  const [submitted, setSubmitted] = useState(false);

  const validateForm = () => {
    const newErrors = {};
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    const phoneRegex = /^\+?[1-9]\d{1,14}$/; // Basic international phone format

    if (!formData.email || !emailRegex.test(formData.email)) {
      newErrors.email = 'Please enter a valid email address';
    }
    if (!formData.phone || !phoneRegex.test(formData.phone)) {
      newErrors.phone = 'Please enter a valid phone number (e.g., +919954567890)';
    }
    if (!formData.name) newErrors.name = 'Name is required';
    if (!formData.location) newErrors.location = 'Location is required';
    if (!formData.requirements || formData.requirements.length < 10 || formData.requirements.length > 1000) {
      newErrors.requirements = 'Requirements must be 10-1000 characters';
    }

    setErrors(newErrors);
    return Object.keys(newErrors).length === 0;
  };

  const handleChange = (e) => {
    const { name, value } = e.target;
    setFormData({ ...formData, [name]: value });
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    if (validateForm()) {
      try {
        const response = await fetch('https://jsonplaceholder.typicode.com/posts', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify(formData),
        });
        if (response.ok) {
          setSubmitted(true);
          setFormData({ email: '', phone: '', name: '', location: '', requirements: '' });
          setErrors({});
        } else {
          alert('Failed to submit. Please try again.');
        }
      } catch (error) {
        alert('An error occurred: ' + error.message);
      }
    }
  };

  return (
    <main className="contact-page">
      <section className="contact">
        <h1>Contact Us</h1>
        <p>Let us know how we can help your business with open-source SaaS solutions.</p>
        {submitted && <p className="success">Thank you! We’ll get back to you soon.</p>}
        <form onSubmit={handleSubmit}>
          <div className="form-group">
            <label htmlFor="name">Name</label>
            <input
              type="text"
              id="name"
              name="name"
              value={formData.name}
              onChange={handleChange}
              placeholder="Your Name"
            />
            {errors.name && <span className="error">{errors.name}</span>}
          </div>
          <div className="form-group">
            <label htmlFor="email">Email</label>
            <input
              type="email"
              id="email"
              name="email"
              value={formData.email}
              onChange={handleChange}
              placeholder="your@email.com"
            />
            {errors.email && <span className="error">{errors.email}</span>}
          </div>
          <div className="form-group">
            <label htmlFor="phone">Phone</label>
            <input
              type="tel"
              id="phone"
              name="phone"
              value={formData.phone}
              onChange={handleChange}
              placeholder="+1234567890"
            />
            {errors.phone && <span className="error">{errors.phone}</span>}
          </div>
          <div className="form-group">
            <label htmlFor="location">Location</label>
            <input
              type="text"
              id="location"
              name="location"
              value={formData.location}
              onChange={handleChange}
              placeholder="City, Country"
            />
            {errors.location && <span className="error">{errors.location}</span>}
          </div>
          <div className="form-group">
            <label htmlFor="requirements">What You Need (10-1000 characters)</label>
            <textarea
              id="requirements"
              name="requirements"
              value={formData.requirements}
              onChange={handleChange}
              placeholder="Describe your requirements..."
              rows="10"
            />
            <small>{formData.requirements.length}/1000 characters</small>
            {errors.requirements && <span className="error">{errors.requirements}</span>}
          </div>
          <button type="submit">Send Message</button>
        </form>
      </section>
    </main>
  );
}

export default Contact;