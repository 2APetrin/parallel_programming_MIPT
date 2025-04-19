import argparse
import random

def generate_sequence(length, min_val, max_val, seed=None):
    """
    Генерирует случайную последовательность заданной длины и диапазона значений.

    Параметры:
    length (int): Длина последовательности.
    min_val (float): Минимальное значение элемента.
    max_val (float): Максимальное значение элемента.
    seed (int, опционально): Зерно для воспроизводимости результатов.

    Возвращает:
    list: Сгенерированная последовательность.
    """
    if seed is not None:
        random.seed(seed)

    # Определяем тип данных элементов
    if min_val.is_integer() and max_val.is_integer():
        return [random.randint(int(min_val), int(max_val)) for _ in range(length)]
    else:
        return [random.uniform(min_val, max_val) for _ in range(length)]

def main():
    parser = argparse.ArgumentParser(description='Генератор случайных последовательностей')
    parser.add_argument('-l', '--length', type=int, required=True,
                        help='Длина последовательности (целое число > 0)')
    parser.add_argument('-m', '--min', type=float, required=True,
                        help='Минимальное значение элемента')
    parser.add_argument('-M', '--max', type=float, required=True,
                        help='Максимальное значение элемента')
    parser.add_argument('--seed', type=int,
                        help='Зерно для генератора случайных чисел')

    args = parser.parse_args()

    # Проверка корректности введённых данных
    if args.length < 1:
        parser.error("Длина последовательности должна быть положительным целым числом.")
    if args.min > args.max:
        parser.error("Минимальное значение не может превышать максимальное.")

    # Генерация и вывод последовательности
    sequence = generate_sequence(args.length, args.min, args.max, args.seed)
    print(' '.join(map(str, sequence)))

if __name__ == "__main__":
    main()