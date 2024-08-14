# Chess AI

This is an ongoing chess AI project designed to play standard chess against a human player. In it's current iteration, it includes a basic engine capable of evaluating positions, generating legal moves, and selecting optimal moves using a minimax algorithm.

## Features

- **Move Generation**: Generates all possible legal moves for a given position.
- **Evaluation Function**: Evaluates the board position to assess which player has the advantage.
- **Minimax Algorithm**: Searches the game tree to find the best move.

## Building and Running

To download the project, use the following command to ensure the submodules are downloaded as well.

```
git clone https://github.com/siofra-mc/chess_ai.git --recursive
```

The project includes a copy of Premake 5, which is used for establishing the build environment. To use with Visual Studio, run Premake with the appropriate action for your version of visual studio. The example below is for Visual Studio 2022. Once premake completes, open the .sln file that generates and run the program.

```
premake5 vs2022
```

## License

This project is licensed under the GNU General Public License. See the [LICENSE](LICENSE) file for details.
