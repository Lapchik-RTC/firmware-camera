//#include <opencv2/opencv.hpp>
//#include <opencv2/objdetect.hpp>
//#include <iostream>
//#include <locale>
//#include <codecvt>
//
//#ifdef _WIN32
//#include <windows.h>
//#endif
//
//using namespace cv;
//using namespace std;
//
//void setupConsole() {
//#ifdef _WIN32
//    SetConsoleOutputCP(CP_UTF8);
//    SetConsoleCP(CP_UTF8);
//#endif
//    setlocale(LC_ALL, "ru_RU.UTF-8");
//}
//
//// Функция для оценки резкости изображения
//double evaluateSharpness(const Mat& gray) {
//    Mat laplacian;
//    Laplacian(gray, laplacian, CV_64F);
//    Scalar mean, stddev;
//    meanStdDev(laplacian, mean, stddev);
//    return stddev.val[0] * stddev.val[0]; // Чем выше значение, тем резче изображение
//}
//
//// Функция автоматической настройки параметров камеры
//void autoFocus(VideoCapture& cap, Mat& frame) {
//    static double lastSharpness = 0;
//    static int focusStep = 5;
//    static double currentFocus = 50;
//    static int direction = 1;
//
//    // Проверяем поддержку автофокуса
//    bool autoFocusSupported = cap.get(CAP_PROP_AUTOFOCUS) != 0;
//
//    if (!autoFocusSupported) {
//        // Если автофокус не поддерживается, используем программную эмуляцию
//        Mat gray;
//        cvtColor(frame, gray, COLOR_BGR2GRAY);
//        double sharpness = evaluateSharpness(gray);
//
//        // Если резкость ухудшилась, меняем направление фокусировки
//        if (sharpness < lastSharpness) {
//            direction *= -1;
//            focusStep = max(1, focusStep / 2); // Уменьшаем шаг фокусировки
//        }
//
//        lastSharpness = sharpness;
//        currentFocus += direction * focusStep;
//
//        // Ограничиваем диапазон фокусировки
//        currentFocus = max(0.0, min(255.0, currentFocus));
//        cap.set(CAP_PROP_FOCUS, currentFocus);
//    }
//}
//
//int main() {
//    setupConsole();
//
//    // Ручной выбор камеры
//    int cameraIndex = 1; // ← ИЗМЕНИТЕ НА НУЖНЫЙ ИНДЕКС КАМЕРЫ
//
//    QRCodeDetector qrDecoder;
//    VideoCapture cap(cameraIndex);
//
//    if (!cap.isOpened()) {
//        cerr << "Ошибка: не удалось открыть камеру #" << cameraIndex << "!" << endl;
//        return -1;
//    }
//
//    cout << "Используется камера #" << cameraIndex << endl;
//
//    // Настройки камеры
//    cap.set(CAP_PROP_FRAME_WIDTH, 720);
//    cap.set(CAP_PROP_FRAME_HEIGHT, 480);
//    cap.set(CAP_PROP_FPS, 29);
//
//    // Проверяем поддержку автофокуса
//    bool autoFocusSupported = cap.get(CAP_PROP_AUTOFOCUS) != 0;
//    if (autoFocusSupported) {
//        cap.set(CAP_PROP_AUTOFOCUS, 1);
//        cout << "Аппаратный автофокус включен" << endl;
//    }
//    else {
//        cout << "Аппаратный автофокус не поддерживается, используется программная эмуляция" << endl;
//    }
//
//    namedWindow("QR Detection", WINDOW_NORMAL);
//    resizeWindow("QR Detection", 720, 480);
//
//    // Переменные для отслеживания состояния QR-кода
//    bool qrDetected = false;
//    int undetectedCount = 0;
//    const int maxUndetected = 10;
//
//    while (true) {
//        Mat frame, gray;
//        cap >> frame;
//        if (frame.empty()) {
//            cerr << "Ошибка: пустой кадр!" << endl;
//            waitKey(50);
//            continue;
//        }
//
//        // Автоматическая фокусировка
//        autoFocus(cap, frame);
//
//        // Конвертация в серый
//        cvtColor(frame, gray, COLOR_BGR2GRAY);
//
//        // Обнаружение QR-кода
//        vector<Point> points;
//        string data;
//        bool detected = qrDecoder.detect(gray, points) && !points.empty();
//
//        if (detected) {
//            data = qrDecoder.decode(gray, points);
//            if (data.empty()) detected = false;
//        }
//
//        // Улучшенная обработка результатов
//        if (detected && !data.empty()) {
//            qrDetected = true;
//            undetectedCount = 0;
//
//            try {
//                // Конвертация из UTF-8
//                wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
//                wstring wideData = converter.from_bytes(data);
//
//                // Вывод в консоль
//                wcout << L"Найден QR-код: " << wideData << endl;
//
//                // Рисование рамки
//                if (points.size() >= 4) {
//                    for (size_t i = 0; i < points.size(); ++i) {
//                        line(frame, points[i], points[(i + 1) % points.size()],
//                            Scalar(0, 255, 0), 3);
//                    }
//                }
//
//                // Вывод текста на изображение
//                putText(frame, "QR: " + data, Point(20, 40),
//                    FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2);
//            }
//            catch (const exception& e) {
//                cerr << "Ошибка декодирования: " << e.what() << endl;
//            }
//        }
//        else {
//            if (qrDetected) {
//                undetectedCount++;
//                if (undetectedCount >= maxUndetected) {
//                    qrDetected = false;
//                    wcout << L"QR-код потерян" << endl;
//                }
//            }
//        }
//
//        // Отображение информации о камере
//        putText(frame, "Camera #" + to_string(cameraIndex), Point(20, 80),
//            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 0), 2);
//
//        // Отображение состояния фокуса
//        string focusStatus = autoFocusSupported ? "AutoFocus: ON" : "SoftFocus: ON";
//        putText(frame, focusStatus, Point(20, 120),
//            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 255), 2);
//
//        // Отображение резкости
//        double sharpness = evaluateSharpness(gray);
//        putText(frame, "Sharpness: " + to_string(int(sharpness)), Point(20, 160),
//            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(200, 200, 255), 2);
//
//        imshow("QR Detection", frame);
//
//        // Управление клавишами
//        int key = waitKey(1);
//        if (key == 27) break; // ESC - выход
//    }
//
//    cap.release();
//    destroyAllWindows();
//    return 0;
//}







#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <locale>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace cv;
using namespace std;

// Конфигурационные параметры
const int MULTIFRAME_CONFIRMATION = 3;  // Количество последовательных обнаружений для подтверждения
const int MAX_UNDETECTED = 10;          // Максимум кадров без обнаружения перед сбросом состояния
const int CLAHE_CLIP_LIMIT = 3;         // Предел обрезки для CLAHE (управляет контрастом)
const Size MORPH_KERNEL_SIZE(3, 3);     // Размер ядра для морфологических операций
const double RESIZE_FACTOR = 1.9;       // Коэффициент увеличения для мелких QR-кодов
const double MIN_QR_AREA = 200;         // Минимальная площадь QR-кода в пикселях

void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // Установка кодировки UTF-8 для Windows
    SetConsoleCP(CP_UTF8);        // Поддержка русских символов
#endif
    setlocale(LC_ALL, "ru_RU.UTF-8");  // Установка локали
}

// Функция оценки резкости изображения
double evaluateSharpness(const Mat& gray) {
    Mat laplacian;
    Laplacian(gray, laplacian, CV_64F);  // Применение оператора Лапласа
    Scalar mean, stddev;
    meanStdDev(laplacian, mean, stddev);  // Расчет стандартного отклонения
    return stddev.val[0] * stddev.val[0];  // Мера резкости (чем выше, тем резче)
}

// Функция программной фокусировки
void autoFocus(VideoCapture& cap, Mat& frame) {
    static double lastSharpness = 0;
    static int focusStep = 5;
    static double currentFocus = 50;
    static int direction = 1;

    bool autoFocusSupported = cap.get(CAP_PROP_AUTOFOCUS) != 0;

    if (!autoFocusSupported) {
        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);  // Конвертация в градации серого
        double sharpness = evaluateSharpness(gray);  // Оценка резкости

        // Логика "горного поиска" для максимизации резкости
        if (sharpness < lastSharpness) {
            direction *= -1;  // Смена направления
            focusStep = max(1, focusStep / 2);  // Уменьшение шага
        }

        lastSharpness = sharpness;
        currentFocus += direction * focusStep;
        currentFocus = max(0.0, min(255.0, currentFocus));  // Ограничение диапазона
        cap.set(CAP_PROP_FOCUS, currentFocus);  // Установка фокуса
    }
}

// Функция улучшения изображения для детекции QR-кодов
Mat enhanceQRImage(Mat& gray) {
    // 1. CLAHE (Contrast Limited Adaptive Histogram Equalization)
    Ptr<CLAHE> clahe = createCLAHE();  // Создание CLAHE-фильтра
    clahe->setClipLimit(CLAHE_CLIP_LIMIT);  // Установка параметра контраста
    Mat equalized;
    clahe->apply(gray, equalized);  // Применение к изображению

    // 2. Морфологическое закрытие (удаление мелких черных пятен)
    Mat morph;
    Mat kernel = getStructuringElement(MORPH_RECT, MORPH_KERNEL_SIZE);  // Создание ядра
    morphologyEx(equalized, morph, MORPH_CLOSE, kernel);  // Применение операции

    // 3. Фильтр повышения резкости
    Mat sharpened;
    // Ядро для усиления краев
    Mat sharpeningKernel = (Mat_<float>(3, 3) <<
        -1, -1, -1,
        -1, 9, -1,
        -1, -1, -1);
    filter2D(morph, sharpened, CV_8U, sharpeningKernel);  // Свертка с ядром

    return sharpened;  // Возврат улучшенного изображения
}

// Проверка валидности размера QR-кода
bool isQRSizeValid(const vector<Point>& points, const Mat& frame) {
    if (points.size() < 4) return false;  // Недостаточно точек для QR-кода

    double area = contourArea(points);  // Вычисление площади контура
    if (area < MIN_QR_AREA) return false;  // Слишком маленькая площадь

    Rect bbox = boundingRect(points);  // Ограничивающий прямоугольник
    double aspectRatio = static_cast<double>(bbox.width) / bbox.height;
    // Проверка соотношения сторон (должно быть близко к 1:1)
    return (aspectRatio > 0.7 && aspectRatio < 1.3);
}

int main() {
    setupConsole();  // Настройка консоли

    // Настройка окружения OpenCV
#ifdef _WIN32
    _putenv_s("OPENCV_VIDEOIO_PRIORITY_LIST", "MSMF;DSHOW;CV_IMAGES;CV_MJPEG");  // Приоритет бэкендов
    _putenv_s("OPENCV_LOG_LEVEL", "ERROR");  // Отключение информационных сообщений
#endif
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);  // Установка уровня логов

    int cameraIndex = 1;  // Индекс камеры
    cout << "Используется камера #" << cameraIndex << endl;

    QRCodeDetector qrDecoder;  // Инициализация детектора QR-кодов
    cout << "Используется стандартный детектор QR-кодов OpenCV" << endl;

    VideoCapture cap(cameraIndex);  // Открытие видеопотока
    if (!cap.isOpened()) {
        cerr << "Ошибка: не удалось открыть камеру #" << cameraIndex << "!" << endl;
        return -1;
    }

    // Настройка параметров камеры
    cap.set(CAP_PROP_FRAME_WIDTH, 720);   // Ширина кадра
    cap.set(CAP_PROP_FRAME_HEIGHT, 480);  // Высота кадра
    cap.set(CAP_PROP_FPS, 25);            // Частота кадров

    // Дополнительные настройки для улучшения качества
    cap.set(CAP_PROP_AUTO_EXPOSURE, 0.25);  // Отключение автоэкспозиции
    cap.set(CAP_PROP_EXPOSURE, -4);         // Ручная настройка экспозиции
    cap.set(CAP_PROP_CONTRAST, 50);         // Увеличение контраста

    // Проверка поддержки автофокуса
    bool autoFocusSupported = cap.get(CAP_PROP_AUTOFOCUS) != 0;
    if (autoFocusSupported) {
        cap.set(CAP_PROP_AUTOFOCUS, 1);  // Включение аппаратного автофокуса
        cout << "Аппаратный автофокус включен" << endl;
    }
    else {
        cout << "Аппаратный автофокус не поддерживается, используется программная эмуляция" << endl;
    }

    // Создание окон
    namedWindow("QR Detection", WINDOW_GUI_NORMAL);  // Основное окно
    resizeWindow("QR Detection", 720, 480);
    namedWindow("Enhanced Image", WINDOW_GUI_NORMAL);  // Окно улучшенного изображения
    resizeWindow("Enhanced Image", 360, 240);

    // Переменные для отслеживания состояния
    bool qrDetected = false;  // Флаг обнаружения
    int undetectedCount = 0;  // Счетчик пропущенных кадров
    vector<string> detectionHistory(MULTIFRAME_CONFIRMATION, "");  // История распознаваний
    int frameCount = 0;       // Счетчик кадров
    string confirmedData = "";  // Подтвержденные данные

    while (true) {
        Mat frame, gray;
        cap >> frame;  // Захват кадра
        if (frame.empty()) {
            cerr << "Ошибка: пустой кадр!" << endl;
            waitKey(50);
            continue;
        }

        autoFocus(cap, frame);  // Автофокусировка
        cvtColor(frame, gray, COLOR_BGR2GRAY);  // Конвертация в серый

        // Улучшение изображения для детекции
        Mat enhanced = enhanceQRImage(gray);

        vector<Point> points;  // Точки углов QR-кода
        string data;           // Декодированные данные
        bool detected = false; // Флаг успешного распознавания

        // Основная попытка распознавания
        detected = qrDecoder.detect(enhanced, points) && !points.empty();

        if (detected) {
            data = qrDecoder.decode(enhanced, points);  // Декодирование

            // Fallback на оригинальное изображение
            if (data.empty()) {
                data = qrDecoder.decode(gray, points);
            }

            // Фильтрация по размеру
            if (!data.empty() && !isQRSizeValid(points, frame)) {
                detected = false;
                data.clear();
            }
        }

        // Попытка распознавания на увеличенном изображении
        if (!detected) {
            Mat resized;
            // Увеличение изображения (бикубическая интерполяция)
            resize(enhanced, resized, Size(), RESIZE_FACTOR, RESIZE_FACTOR, INTER_CUBIC);
            vector<Point> resizedPoints;  // Точки на увеличенном изображении

            if (qrDecoder.detect(resized, resizedPoints) && !resizedPoints.empty()) {
                // Масштабирование точек обратно
                for (size_t i = 0; i < resizedPoints.size(); i++) {
                    resizedPoints[i].x = static_cast<int>(resizedPoints[i].x / RESIZE_FACTOR);
                    resizedPoints[i].y = static_cast<int>(resizedPoints[i].y / RESIZE_FACTOR);
                }

                points = resizedPoints;  // Сохранение точек
                data = qrDecoder.decode(enhanced, points);  // Декодирование

                // Fallback на оригинал
                if (data.empty()) {
                    data = qrDecoder.decode(gray, points);
                }

                // Проверка размера
                if (!data.empty() && isQRSizeValid(points, frame)) {
                    detected = true;  // Успешное распознавание
                }
            }
        }

        // Мультикадровая проверка
        if (detected && !data.empty()) {
            // Сохранение в историю
            detectionHistory[frameCount % MULTIFRAME_CONFIRMATION] = data;

            // Подсчет одинаковых результатов
            int sameCount = 0;
            for (const string& item : detectionHistory) {
                if (item == data) sameCount++;
            }

            // Подтверждение при устойчивом распознавании
            if (sameCount >= MULTIFRAME_CONFIRMATION) {
                if (!qrDetected || confirmedData != data) {
                    confirmedData = data;  // Сохранение подтвержденных данных
                    qrDetected = true;     // Установка флага
                    undetectedCount = 0;   // Сброс счетчика пропусков

                    // Вывод в консоль
                    try {
                        cout << "Найден QR-код: " << data << endl;
                    }
                    catch (const exception& e) {
                        cerr << "Ошибка вывода: " << e.what() << endl;
                    }
                }
            }
        }
        else {
            if (qrDetected) {
                undetectedCount++;
                // Сброс после серии пропусков
                if (undetectedCount >= MAX_UNDETECTED) {
                    qrDetected = false;
                    confirmedData.clear();
                    cout << "QR-код потерян" << endl;
                }
            }
        }

        // Визуализация результатов
        if (qrDetected && !points.empty() && points.size() >= 4) {
            // Рисование границ QR-кода
            for (size_t i = 0; i < points.size(); i++) {
                line(frame, points[i], points[(i + 1) % points.size()],
                    Scalar(0, 255, 0), 3);  // Зеленая рамка
            }

            // Отображение данных
            putText(frame, "QR: " + confirmedData, Point(20, 40),
                FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255), 2);  // Красный текст
        }

        // Отображение системной информации
        putText(frame, "Camera #" + to_string(cameraIndex), Point(20, frame.rows - 80),
            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 0), 2);  // Желтый текст

        string focusStatus = autoFocusSupported ? "AutoFocus: ON" : "SoftFocus: ON";
        putText(frame, focusStatus, Point(20, frame.rows - 50),
            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 255), 2);  // Голубой текст

        double sharpness = evaluateSharpness(enhanced);  // Оценка резкости
        putText(frame, "Sharpness: " + to_string(int(sharpness)), Point(20, frame.rows - 20),
            FONT_HERSHEY_SIMPLEX, 0.7, Scalar(200, 200, 255), 2);  // Розовый текст

        // Отображение окон
        imshow("QR Detection", frame);        // Основное окно с визуализацией
        imshow("Enhanced Image", enhanced);   // Окно улучшенного изображения

        // Обработка клавиш
        int key = waitKey(1);
        if (key == 27) break;  // Выход по ESC
        else if (key == 's') {  // Сохранение кадров
            imwrite("debug_frame.png", frame);
            imwrite("debug_enhanced.png", enhanced);
            cout << "Кадры сохранены для отладки" << endl;
        }

        frameCount++;  // Увеличение счетчика кадров
    }

    // Освобождение ресурсов
    cap.release();
    destroyAllWindows();
    return 0;
}