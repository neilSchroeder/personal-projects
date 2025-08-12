# Deploying Tango Puzzle Game to GitHub Pages

## Overview

This guide explains how to deploy your Tango puzzle game to GitHub Pages. Since GitHub Pages only supports static sites, we'll need to adapt your current backend+frontend architecture.

## Current Architecture Challenge

Your current setup:
- **Backend**: FastAPI server with game logic, puzzle generation, and hints
- **Frontend**: SvelteKit application that communicates with the backend API
- **Problem**: GitHub Pages cannot host server-side APIs

## Deployment Options

### Option 1: Static-Only Version (Recommended)

Convert the game to run entirely in the frontend by porting the game logic to JavaScript.

#### Steps:

1. **Install SvelteKit Static Adapter**
```bash
cd frontend
npm install -D @sveltejs/adapter-static
```

2. **Update SvelteKit Configuration**
Create/update `frontend/svelte.config.js`:
```javascript
import adapter from '@sveltejs/adapter-static';
import { vitePreprocess } from '@sveltejs/vite-plugin-svelte';

const dev = process.argv.includes('dev');

/** @type {import('@sveltejs/kit').Config} */
const config = {
	preprocess: vitePreprocess(),
	kit: {
		adapter: adapter({
			pages: 'build',
			assets: 'build',
			fallback: '404.html',
			precompress: false,
			strict: true
		}),
		paths: {
			base: dev ? '' : process.env.BASE_PATH || ''
		}
	}
};

export default config;
```

3. **Enable Prerendering**
Create `frontend/src/routes/+layout.js`:
```javascript
// Enable static site generation
export const prerender = true;
export const ssr = false; // Use SPA mode for dynamic game logic
```

4. **Port Game Logic to Frontend**
Create `frontend/src/lib/game/` directory and port:
- `TangoBoardSolver` class
- `GameService` logic
- Puzzle generation algorithms
- Hint system

5. **Update Package.json Scripts**
Add build script for GitHub Pages:
```json
{
  "scripts": {
    "build:gh-pages": "BASE_PATH=/personal-projects npm run build"
  }
}
```

6. **Create GitHub Actions Workflow**
Create `.github/workflows/deploy.yml`:
```yaml
name: Deploy to GitHub Pages

on:
  push:
    branches: ['main', 'master']
  workflow_dispatch:

jobs:
  build_site:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout
        uses: actions/checkout@v4

      - name: Install Node.js
        uses: actions/setup-node@v4
        with:
          node-version: 20
          cache: npm
          cache-dependency-path: frontend/package-lock.json

      - name: Install dependencies
        working-directory: ./frontend
        run: npm ci

      - name: Build
        working-directory: ./frontend
        env:
          BASE_PATH: '/personal-projects'
        run: npm run build

      - name: Upload Artifacts
        uses: actions/upload-pages-artifact@v3
        with:
          path: 'frontend/build/'

  deploy:
    needs: build_site
    runs-on: ubuntu-latest
    permissions:
      pages: write
      id-token: write
    environment:
      name: github-pages
      url: ${{ steps.deployment.outputs.page_url }}
    steps:
      - name: Deploy
        id: deployment
        uses: actions/deploy-pages@v4
```

7. **Add .nojekyll File**
Create `frontend/static/.nojekyll` (empty file) to prevent Jekyll interference.

8. **Configure GitHub Repository**
- Go to your repository settings
- Navigate to Pages section
- Set Source to "GitHub Actions"

### Option 2: Hybrid Approach with External Backend

Keep the backend separate and deploy it to a service like Railway, Render, or Vercel, then configure CORS.

#### Steps:

1. **Deploy Backend Separately**
   - Use Railway, Render, or Vercel for the FastAPI backend
   - Configure CORS to allow requests from your GitHub Pages domain

2. **Update Frontend Configuration**
   - Configure API base URL to point to your deployed backend
   - Use environment variables for different environments

3. **Follow Static Deployment Steps**
   - Use the same SvelteKit static adapter setup as Option 1
   - Update API calls to use the external backend URL

## Recommended Implementation (Option 1)

Since your game logic is well-structured, I recommend Option 1 - porting the game logic to the frontend. This provides:

### Benefits:
- ✅ Completely free hosting on GitHub Pages
- ✅ No external dependencies or API limits
- ✅ Faster gameplay (no network requests)
- ✅ Works offline once loaded
- ✅ No CORS issues

### Game Logic Porting Strategy:

1. **Convert Python Classes to TypeScript/JavaScript**
   - `TangoBoardSolver` → `TangoBoardSolver.ts`
   - `GameService` → `GameService.ts`
   - Puzzle generation → `PuzzleGenerator.ts`

2. **Preserve Core Algorithms**
   - Keep the constraint optimization logic
   - Maintain the inference-based solving
   - Port the educational hint system

3. **Use Local Storage**
   - Store leaderboard data in browser localStorage
   - Save game state for resuming later

## File Structure After Conversion

```
frontend/
├── src/
│   ├── lib/
│   │   ├── game/
│   │   │   ├── TangoBoardSolver.ts
│   │   │   ├── GameService.ts
│   │   │   ├── PuzzleGenerator.ts
│   │   │   └── types.ts
│   │   └── components/
│   ├── routes/
│   │   ├── +layout.js          # Enable prerendering
│   │   └── +page.svelte        # Main game page
│   └── static/
│       └── .nojekyll           # Prevent Jekyll
├── svelte.config.js            # Static adapter config
└── package.json
```

## Expected Results

After deployment, your game will be available at:
`https://neilSchroeder.github.io/personal-projects/`

The static version will maintain all the intelligent puzzle generation and educational features while being completely self-contained in the browser.

## Next Steps

1. Choose your preferred deployment option
2. If going with Option 1, start by setting up the static adapter
3. Port the game logic gradually, testing each component
4. Set up the GitHub Actions workflow
5. Configure GitHub Pages in your repository settings

Would you like me to help implement any of these steps?
