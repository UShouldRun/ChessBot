# Chessbot

Chessbot is a modular chess engine that provides an efficient chess API and serves as a foundation for advanced search techniques. Designed with speed and memory efficiency in mind, the engine uses compact data structures and specialized move encodings. Future enhancements will integrate Monte Carlo Tree Search (MCTS) and neural network evaluation to improve move selection and overall play.

## Features

- **Efficient Chess API:**  
  - Uses an 8-element array of `uint32_t` values for board representation (4 bits per square).
  - 16‑bit move encoding with distinct layouts for normal moves and pawn moves.
  - Functions for move generation, board updates, FEN encoding, and board printing.

- **Future Enhancements:**  
  - **MCTS Integration:** A module based on Monte Carlo Tree Search will be added for advanced move search and decision making.
  - **Neural Network Evaluation:** A neural network component is planned to evaluate board positions, guiding move selection and improving performance.

## Build Instructions

The project uses a Makefile for compilation. To build the project, run:

```bash
make
```

This will compile the chess library and the main application, producing an executable named chessbot. To clean build artifacts, run:

```bash
make clean
```

## Future Plans
    MCTS Integration:
    Incorporate a Monte Carlo Tree Search module to enhance move search and decision making.

    Neural Network Evaluation:
    Add a neural network component to evaluate board positions and improve move selection.

## Author
Henrique Teixeira

## License
This project is licensed under the MIT License.
