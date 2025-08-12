/** @type {import('tailwindcss').Config} */
export default {
  content: ['./src/**/*.{html,js,svelte,ts}'],
  theme: {
    extend: {
      colors: {
        'game-board': '#f8f9fa',
        'game-border': '#dee2e6',
        'sun-color': '#ffd43b',
        'moon-color': '#6c757d',
        'constraint-same': '#28a745',
        'constraint-different': '#dc3545',
        'locked-tile': '#e9ecef'
      },
      spacing: {
        '18': '4.5rem',
        '88': '22rem'
      }
    },
  },
  plugins: [],
}
