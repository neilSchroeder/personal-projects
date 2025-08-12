/**
 * Main export file for the Tango game library
 * Provides a clean interface for importing game functionality
 */

export * from './types';
export { TangoBoardSolver } from './TangoBoardSolver';
export { PuzzleGenerator } from './PuzzleGenerator';
export { GameService } from './GameService';

// Re-export common types for convenience
export { 
  PieceType, 
  ConstraintType
} from './types';

export type { 
  GameState, 
  GameResult, 
  Hint, 
  Stats,
  PuzzleConfig 
} from './types';

// Import GameService for the instance
import { GameService } from './GameService';

// Main game service instance for easy usage
export const gameService = new GameService();
