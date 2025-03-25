## To start server
npm start


# File Structure
my-saas-website/
├── .devcontainer/
│   ├── devcontainer.json
│   └── Dockerfile.dev
├── public/
│   └── index.html
├── src/
│   ├── components/
│   │   ├── Header.js
│   │   ├── Footer.js
│   │   ├── Hero.js
│   │   └── BenefitTile.js
│   ├── pages/
│   │   ├── Home.js
│   │   ├── Solutions.js
│   │   ├── Pricing.js
│   │   └── Contact.js  ← New
│   ├── App.js
│   ├── index.js
│   └── styles.css
├── Dockerfile
├── docker-compose.yml
├── package.json
└── .gitignore