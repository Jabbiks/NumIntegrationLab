# NumIntegrationLab (Лабораторна робота №7, Варіант 1)

Статична бібліотека чисельного інтегрування `NumIntegrationLib` мовою C++20 з обробкою виняткових ситуацій, контролем інваріантів, сильною гарантією безпеки (`strong guarantee`) та модульними тестами Google Test. Розроблено у **Visual Studio Code** з використанням **CMake Tools** та **vcpkg**.

## Структура проєкту
- `include/` — заголовкові файли бібліотеки (`NumIntegration.h`, `IntegrationTask.h`, `IntegrationExceptions.h`, `ScopeLogger.h`);
- `src/` — реалізація алгоритмів інтегрування та класу `IntegrationTask`;
- `demo/` — демонстраційна програма (`IntegrationDemo`);
- `antipattern/` — демонстрація антипатернів обробки винятків (`AntipatternDemo`);
- `tests/` — модульні тести Google Test (`IntegrationTests`).

## Збирання та запуск у Visual Studio Code

### Через інтерфейс VS Code (CMake Tools)
1. Відкрийте папку проєкту `NumIntegrationLab` через **File → Open Folder...**.
2. Натисніть `Ctrl + Shift + P` → **CMake: Select Configure Preset** → виберіть **`x64 Debug (vcpkg)`**.
3. Натисніть `Ctrl + Shift + P` → **CMake: Select a Kit** → виберіть компілятор **Visual Studio Build Tools ... - amd64**.
4. Натисніть **Build** на нижній панелі стану (або клавішу `F7`).
5. Для запуску демонстраційних програм виберіть ціль **`IntegrationDemo`** або **`AntipatternDemo`** на нижній панелі та натисніть **`▷`** (`Shift + F5`).
6. Для запуску модульних тестів відкрийте вкладку **Testing** (іконка колби на лівій панелі) та натисніть **Run Tests** (`▷`).

### Через термінал VS Code
```powershell
cmake --preset x64-debug
cmake --build --preset x64-debug
ctest --test-dir out/build/x64-debug --output-on-failure