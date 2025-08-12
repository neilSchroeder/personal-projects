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
  gameId: string;
  board: PieceType[][];
  hConstraints: ConstraintType[][];
  vConstraints: ConstraintType[][];
  lockedTiles: boolean[][];
  isComplete: boolean;
  startTime: Date;
  completionTime?: Date;
  movesCount: number;
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
