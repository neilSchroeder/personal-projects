/**
 * Puzzle configuration for different difficulty levels
 * Ported from backend/app/config/puzzle_config.py
 */

import type { PuzzleConfig } from './types';

const PUZZLE_CONFIGS: Record<string, PuzzleConfig> = {
  easy: {
    name: 'Easy',
    startingPiecesMin: 8,
    startingPiecesMax: 12,
    constraintProbability: 0.15,
    maxAttempts: 20
  },
  medium: {
    name: 'Medium', 
    startingPiecesMin: 6,
    startingPiecesMax: 10,
    constraintProbability: 0.25,
    maxAttempts: 15
  },
  hard: {
    name: 'Hard',
    startingPiecesMin: 4,
    startingPiecesMax: 8,
    constraintProbability: 0.35,
    maxAttempts: 10
  }
};

export function getPuzzleConfig(difficulty?: string): PuzzleConfig {
  const diff = difficulty?.toLowerCase() || 'medium';
  return PUZZLE_CONFIGS[diff] || PUZZLE_CONFIGS.medium;
}
