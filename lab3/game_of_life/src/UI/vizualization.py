import pygame
import json
import subprocess
import time
import select

WIDTH, HEIGHT = 800, 600
FPS = 60
BACKGROUND_COLOR = (0, 0, 0)
CELL_COLOR = (255, 255, 255)
GRID_COLOR = (50, 50, 50)
EDIT_BG_COLOR = (30, 30, 30)

class GameOfLifeVisualizer:
    def __init__(self):
        pygame.init()
        pygame.display.set_caption("Game of Life - Edit Mode")
        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont(None, 24)

        # Параметры камеры
        self.scale = 20.0
        self.offset_x = 0
        self.offset_y = 0
        self.dragging = False
        self.last_mouse_pos = (0, 0)

        # Режимы работы
        self.edit_mode = True
        self.cells = set()

    def world_to_screen(self, x, y):
        """Конвертирует мировые координаты в экранные"""
        screen_x = int((x - self.offset_x) * self.scale + WIDTH // 2)
        screen_y = int((y - self.offset_y) * self.scale + HEIGHT // 2)
        return screen_x, screen_y

    def screen_to_world(self, screen_x, screen_y):
        """Конвертирует экранные координаты в мировые"""
        x = (screen_x - WIDTH // 2) / self.scale + self.offset_x
        y = (screen_y - HEIGHT // 2) / self.scale + self.offset_y
        return x, y

    def screen_to_grid(self, screen_x, screen_y):
        """Конвертирует экранные координаты в координаты сетки"""
        x, y = self.screen_to_world(screen_x, screen_y)
        return round(x), round(y)

    def save_initial_state(self, filename="initial_state.txt"):
        """Сохраняет начальное состояние в файл"""
        with open(filename, "w") as f:
            for x, y in self.cells:
                f.write(f"{x} {y}\n")
        print(f"Initial state saved to {filename} with {len(self.cells)} cells")

    def load_state(self, filename="./tmp.json"):
        """Загружает состояние из JSON-файла"""
        try:
            with open(filename, 'r') as f:
                data = json.load(f)
                self.cells = set((cell['x'], cell['y']) for cell in data['alive_cells'])
        except FileNotFoundError:
            print("Файл состояния не найден")
            self.cells = set()

    def handle_edit_events(self):
        """Обрабатывает события ввода в режиме редактирования"""
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return False

            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN:
                    # Завершаем редактирование и начинаем симуляцию
                    self.save_initial_state()
                    self.edit_mode = False
                    pygame.display.set_caption("Game of Life - Simulation Mode")
                    return True
                elif event.key == pygame.K_c:
                    # Очищаем поле
                    self.cells.clear()
                elif event.key == pygame.K_s:
                    # Сохраняем текущее состояние
                    self.save_initial_state()

            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:  # Левая кнопка мыши - добавление/удаление клетки
                    grid_x, grid_y = self.screen_to_grid(*event.pos)
                    cell = (grid_x, grid_y)
                    if cell in self.cells:
                        self.cells.remove(cell)
                    else:
                        self.cells.add(cell)
                elif event.button == 2 or event.button == 3:  # Средняя или правая кнопка - перемещение
                    self.dragging = True
                    self.last_mouse_pos = event.pos
                elif event.button == 4:  # Колесико вверх
                    self.scale *= 1.1
                elif event.button == 5:  # Колесико вниз
                    self.scale /= 1.1

            elif event.type == pygame.MOUSEBUTTONUP:
                if event.button == 2 or event.button == 3:
                    self.dragging = False

            elif event.type == pygame.MOUSEMOTION:
                if self.dragging:
                    dx = (event.pos[0] - self.last_mouse_pos[0]) / self.scale
                    dy = (event.pos[1] - self.last_mouse_pos[1]) / self.scale
                    self.offset_x -= dx
                    self.offset_y -= dy
                    self.last_mouse_pos = event.pos

        return True

    def handle_simulate_events(self):
        """Обрабатывает события ввода в режиме симуляции"""
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return False

            elif event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:  # Левая кнопка мыши
                    self.dragging = True
                    self.last_mouse_pos = event.pos
                elif event.button == 4:  # Колесико вверх
                    self.scale *= 1.1
                elif event.button == 5:  # Колесико вниз
                    self.scale /= 1.1

            elif event.type == pygame.MOUSEBUTTONUP:
                if event.button == 1:
                    self.dragging = False

            elif event.type == pygame.MOUSEMOTION:
                if self.dragging:
                    dx = (event.pos[0] - self.last_mouse_pos[0]) / self.scale
                    dy = (event.pos[1] - self.last_mouse_pos[1]) / self.scale
                    self.offset_x -= dx
                    self.offset_y -= dy
                    self.last_mouse_pos = event.pos

        return True

    def draw_grid(self):
        """Отрисовывает сетку"""
        # Вычисляем видимую область в мировых координатах
        left, top = self.screen_to_world(0, 0)
        right, bottom = self.screen_to_world(WIDTH, HEIGHT)

        # Рисуем сетку
        for x in range(int(left), int(right) + 1):
            screen_x, _ = self.world_to_screen(x + 0.5, 0)
            pygame.draw.line(self.screen, GRID_COLOR, (screen_x, 0), (screen_x, HEIGHT))

        for y in range(int(top), int(bottom) + 1):
            _, screen_y = self.world_to_screen(0, y + 0.5)
            pygame.draw.line(self.screen, GRID_COLOR, (0, screen_y), (WIDTH, screen_y))

    def draw_cells(self):
        """Отрисовывает живые клетки"""
        for x, y in self.cells:
            screen_x, screen_y = self.world_to_screen(x, y)
            cell_size = max(1, int(self.scale))
            pygame.draw.rect(self.screen, CELL_COLOR, 
                           (screen_x - cell_size//2, screen_y - cell_size//2, 
                            cell_size, cell_size))

    def draw_ui(self):
        """Отрисовывает UI элементы"""
        if self.edit_mode:
            # Отображаем инструкции для режима редактирования
            instructions = [
                "Edit Mode: Click to add/remove cells",
                "Enter: Start simulation",
                "C: Clear all cells",
                "S: Save initial state",
                f"Cells: {len(self.cells)}"
            ]

            for i, text in enumerate(instructions):
                text_surface = self.font.render(text, True, (255, 255, 255))
                self.screen.blit(text_surface, (10, 10 + i * 25))
        else:
            # Отображаем информацию о симуляции
            text = f"Simulation Mode - Cells: {len(self.cells)}"
            text_surface = self.font.render(text, True, (255, 255, 255))
            self.screen.blit(text_surface, (10, 10))

    def send_step_command(self):
        """Отправляет команду шага в C++ программу"""
        try:
            self.cpp_process.stdin.write("step\n")
            self.cpp_process.stdin.flush()
        except (IOError, BrokenPipeError) as e:
            print(f"Ошибка отправки команды: {e}")

    def step_wait(self, timeout=1.0):
        """Отправляет команду шага в C++ программу и ждет подтверждения"""
        try:
            # Ждем ответ из stdout с таймаутом
            start_time = time.time()
            while time.time() - start_time < timeout:
                # Проверяем, есть ли данные для чтения
                if select.select([self.cpp_process.stdout], [], [], 0.1)[0]:
                    response = self.cpp_process.stdout.readline().strip()
                    if response == "done":
                        return True  # Шаг успешно выполнен
                    elif response:
                        print(f"Получен неожиданный ответ: {response}")
        except (IOError, BrokenPipeError, ValueError) as e:
            print(f"Ошибка отправки команды или чтения ответа: {e}")
            return False

    def run(self):
        running = True

        # Режим редактирования
        while running and self.edit_mode:
            running = self.handle_edit_events()

            self.screen.fill(EDIT_BG_COLOR)
            self.draw_grid()
            self.draw_cells()
            self.draw_ui()

            pygame.display.flip()
            self.clock.tick(FPS)

        if running:
            self.cpp_process = subprocess.Popen(
                ["./game_of_life_simulation"],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                bufsize=1
            )

            time.sleep(0.1)

            # Основной цикл симуляции
            while running:
                running = self.handle_simulate_events()

                self.load_state()
                self.send_step_command()
                self.step_wait()

                self.screen.fill(BACKGROUND_COLOR)
                self.draw_grid()
                self.draw_cells()
                self.draw_ui()

                pygame.display.flip()
                self.clock.tick(FPS)

            # Завершаем C++ процесс
            try:
                self.cpp_process.terminate()
            except:
                pass

if __name__ == "__main__":
    visualizer = GameOfLifeVisualizer()
    visualizer.run()
