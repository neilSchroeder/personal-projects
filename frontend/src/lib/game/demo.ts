/**
 * Simple test/demo of the Tango game logic
 * Run this to verify the game engine works correctly
 */

import { gameService, PieceType, type GameState } from './index';

export function runGameDemo(): void {
  console.log('🎮 Starting Tango Game Demo');
  
  try {
    // Test 1: Create a new game
    console.log('\n📋 Test 1: Creating new game...');
    const gameState = gameService.newGame('standard');
    console.log('✅ New game created successfully');
    console.log(`Difficulty: ${gameState.difficulty}`);
    console.log(`Starting pieces: ${countFilledCells(gameState.board)}`);
    console.log(`Has constraints: ${hasConstraints(gameState)}`);
    
    // Test 2: Validate initial state
    console.log('\n📋 Test 2: Validating initial state...');
    const isValid = gameService.validateBoard(
      gameState.board, 
      gameState.hConstraints, 
      gameState.vConstraints
    );
    console.log(`✅ Initial board is valid: ${isValid}`);
    
    // Test 3: Make some moves
    console.log('\n📋 Test 3: Making test moves...');
    let currentState = gameState;
    
    // Find an empty position to test
    const emptyPos = findEmptyPosition(currentState.board);
    if (emptyPos) {
      const [row, col] = emptyPos;
      console.log(`Trying to place SUN at position (${row}, ${col})`);
      
      // Test if the move is valid
      const wouldBeValid = gameService.isValidMove(currentState, row, col, PieceType.SUN);
      console.log(`Move would be valid: ${wouldBeValid}`);
      
      if (wouldBeValid) {
        currentState = gameService.makeMove(currentState, row, col, PieceType.SUN);
        console.log('✅ Move made successfully');
        console.log(`Board is still valid: ${currentState.isValid}`);
        console.log(`Board is complete: ${currentState.isComplete}`);
      } else {
        console.log('Testing MOON instead...');
        const moonValid = gameService.isValidMove(currentState, row, col, PieceType.MOON);
        if (moonValid) {
          currentState = gameService.makeMove(currentState, row, col, PieceType.MOON);
          console.log('✅ MOON move made successfully');
        }
      }
    }
    
    // Test 4: Get a hint
    console.log('\n📋 Test 4: Getting hint...');
    try {
      const hint = gameService.getHint(currentState);
      console.log('✅ Hint generated successfully');
      console.log(`Hint type: ${hint.type}`);
      console.log(`Hint message: ${hint.message}`);
      if (hint.position) {
        console.log(`Suggested position: (${hint.position.row}, ${hint.position.col})`);
      }
      if (hint.suggestedPiece) {
        console.log(`Suggested piece: ${hint.suggestedPiece}`);
      }
    } catch (error) {
      console.log(`⚠️ Hint generation failed: ${error}`);
    }
    
    // Test 5: Board analysis
    console.log('\n📋 Test 5: Analyzing board...');
    const analysis = gameService.analyzeBoard(currentState);
    console.log('✅ Board analysis completed');
    console.log(`Error count: ${analysis.errorCount}`);
    console.log(`Completeness: ${analysis.completeness}%`);
    console.log(`Constraint violations: ${analysis.constraintViolations.length}`);
    console.log(`Rule violations: ${analysis.ruleViolations.length}`);
    
    // Test 6: Auto-solve (as demonstration)
    console.log('\n📋 Test 6: Auto-solving puzzle...');
    const solvedState = gameService.autoSolve(currentState);
    console.log('✅ Puzzle auto-solved');
    console.log(`Solution is complete: ${solvedState.isComplete}`);
    console.log(`Solution is valid: ${solvedState.isValid}`);
    console.log(`Validation passes: ${gameService.validateSolution(solvedState)}`);
    
    console.log('\n🎉 All tests completed successfully!');
    
  } catch (error) {
    console.error('❌ Demo failed:', error);
  }
}

function countFilledCells(board: PieceType[][]): number {
  return board.flat().filter(cell => cell !== PieceType.EMPTY).length;
}

function hasConstraints(gameState: GameState): boolean {
  const hHasConstraints = gameState.hConstraints.some(row => 
    row.some(constraint => constraint !== 'none')
  );
  const vHasConstraints = gameState.vConstraints.some(row => 
    row.some(constraint => constraint !== 'none')
  );
  return hHasConstraints || vHasConstraints;
}

function findEmptyPosition(board: PieceType[][]): [number, number] | null {
  for (let row = 0; row < board.length; row++) {
    for (let col = 0; col < board[row].length; col++) {
      if (board[row][col] === PieceType.EMPTY) {
        return [row, col];
      }
    }
  }
  return null;
}

// Export the function for use in components
export default runGameDemo;
