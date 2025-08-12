/**
 * Core types for the Tango game logic
 * Ported from backend/app/models/game_models.py
 */

export enum PieceType {
  EMPTY = 'empty',
  SUN = 'sun',
  MOON = 'moon'
}

export enum ConstraintType {
  NONE = 'none',
  SAME = 'same',
  DIFFERENT = 'different'
}

export interface GameState {
  gameId?: string;
  board: PieceType[][];
  hConstraints: ConstraintType[][];
  vConstraints: ConstraintType[][];
  lockedTiles: boolean[][];
  isComplete: boolean;
  isValid: boolean;
  difficulty: string;
  startTime: Date;
  completionTime?: Date;
  moveCount: number;
}

export interface GameValidationResult {
  isValid: boolean;
  errors: string[];
  isComplete: boolean;
}

export interface LeaderboardEntry {
  time: number;
  date: Date;
  formattedTime: string;
}

export interface GameResult {
  score: number;
  duration: number;
  moves: number;
  difficulty: string;
  completed: boolean;
}

export interface Hint {
  type: 'constraint' | 'rule' | 'logical' | 'none';
  message: string;
  position?: {
    row: number;
    col: number;
  };
  suggestedPiece?: PieceType;
  reasoning?: string;
}

export interface Stats {
  gamesPlayed: number;
  gamesWon: number;
  averageScore: number;
  bestScore: number;
  averageTime: number;
  bestTime: number;
  difficultyStats: { [key: string]: any };
}

export interface HintResult {
  found: boolean;
  row?: number;
  col?: number;
  pieceType?: PieceType;
  reasoning: string;
  confidence?: number;
  hintType?: 'logical_deduction' | 'strategic_guidance' | 'strategic_guess' | 'none';
  educationalValue?: 'high' | 'medium' | 'low';
}

export interface PuzzleConfig {
  name: string;
  startingPiecesMin: number;
  startingPiecesMax: number;
  constraintProbability: number;
  maxAttempts: number;
  baseScore: number;
  parTime: number;
  parMoves: number;
  timeWeight: number;
  moveWeight: number;
}

// Board size constant
export const BOARD_SIZE = 6;

// Utility functions for type conversions
export function pieceTypeFromString(str: string): PieceType {
  switch (str) {
    case 'sun':
      return PieceType.SUN;
    case 'moon':
      return PieceType.MOON;
    default:
      return PieceType.EMPTY;
  }
}

export function constraintTypeFromString(str: string): ConstraintType {
  switch (str) {
    case 'same':
      return ConstraintType.SAME;
    case 'different':
      return ConstraintType.DIFFERENT;
    default:
      return ConstraintType.NONE;
  }
}

// Helper function to create empty board
export function createEmptyBoard(): PieceType[][] {
  return Array(BOARD_SIZE).fill(null).map(() => 
    Array(BOARD_SIZE).fill(PieceType.EMPTY)
  );
}

// Helper function to create empty constraints
export function createEmptyHConstraints(): ConstraintType[][] {
  return Array(BOARD_SIZE).fill(null).map(() => 
    Array(BOARD_SIZE - 1).fill(ConstraintType.NONE)
  );
}

export function createEmptyVConstraints(): ConstraintType[][] {
  return Array(BOARD_SIZE - 1).fill(null).map(() => 
    Array(BOARD_SIZE).fill(ConstraintType.NONE)
  );
}

// Helper function to create empty locked tiles
export function createEmptyLockedTiles(): boolean[][] {
  return Array(BOARD_SIZE).fill(null).map(() => 
    Array(BOARD_SIZE).fill(false)
  );
}

// Puzzle configurations for different difficulty levels
export const PUZZLE_CONFIGS: { [key: string]: PuzzleConfig } = {
  easy: {
    name: 'Easy',
    startingPiecesMin: 8,
    startingPiecesMax: 12,
    constraintProbability: 0.3,
    maxAttempts: 10,
    baseScore: 100,
    parTime: 300, // 5 minutes
    parMoves: 30,
    timeWeight: 2,
    moveWeight: 3
  },
  standard: {
    name: 'Standard',
    startingPiecesMin: 6,
    startingPiecesMax: 10,
    constraintProbability: 0.25,
    maxAttempts: 10,
    baseScore: 200,
    parTime: 240, // 4 minutes
    parMoves: 25,
    timeWeight: 3,
    moveWeight: 4
  },
  challenging: {
    name: 'Challenging',
    startingPiecesMin: 4,
    startingPiecesMax: 8,
    constraintProbability: 0.2,
    maxAttempts: 10,
    baseScore: 300,
    parTime: 180, // 3 minutes
    parMoves: 20,
    timeWeight: 4,
    moveWeight: 5
  },
  expert: {
    name: 'Expert',
    startingPiecesMin: 2,
    startingPiecesMax: 6,
    constraintProbability: 0.15,
    maxAttempts: 10,
    baseScore: 500,
    parTime: 120, // 2 minutes
    parMoves: 15,
    timeWeight: 5,
    moveWeight: 6
  }
};
