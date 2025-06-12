# Sofia - A Simple Chess Engine

**Sofia** (meaning *wisdom*) is a lightweight, educational chess engine written in **C++17**, designed for learning and experimentation rather than competitive play. It is a clean and simple implementation of a UCI-based engine that lays a strong foundation for further development.

---

## Features

- **Alpha-beta pruning**
- **Iterative deepening**
- **Quiescence search**
- **MVV/LVA move ordering**
- **Principal Variation Table**
- **Basic static evaluation**

---

## Next Steps

Planned improvements for future versions of Sofia:

- Improve the evaluation function
- Add support for opening book moves
- Use **bitboards** to improve move generation speed
- Add support for the **WinBoard** protocol (currently only **UCI** is supported)

---

## Compilation (Windows Only)

Make sure you have `g++` that supports **C++17** (e.g., version 8 or higher). You can compile Sofia using the following command in your project directory:


```
git clone https_repo_link
cd projectDir/src
g++ -std=c++17 -O3 -o sofia *.cpp
```

*NOTE*: sometimes you may encounter the error: ``` "The code execution cannot proceed because libewinpthread-1.dll" was not found. Reinstalling the program may fix this problem." ```
then use this command instead

```
g++ -std=c++17 -O3 -static -o sofia *.cpp
```


### Contributions are welcome! Feel free to fork the project, submit pull requests, or raise issues.
---
# Acknowledgments
### Thanks to the chess programming community for the resources and open-source engines that inspired this project.

