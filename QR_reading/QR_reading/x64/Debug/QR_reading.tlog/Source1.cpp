#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <locale>
#include <codecvt>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace cv;
using namespace std;

void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    setlocale(LC_ALL, "ru_RU.UTF-8");
}

int main() {

    setupConsole();

    // Параметры камеры (нужно откалибровать)
    Mat cameraMatrix = (Mat1d(3, 3) <<
        700.0, 0.0, 320.0,
        0.0, 700.0, 240.0,
        0.0, 0.0, 1.0);

    Mat distCoeffs = (Mat1d(1, 5) << 0.1, -0.05, 0.0, 0.0, 0.0);

    QRCodeDetector qrDecoder;

    VideoCapture cap(1);
    if (!cap.isOpened()) {
        cerr << "Ошибка открытия камеры!" << endl;
        return -1;
    }

    /*cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CAP_PROP_FRAME_HEIGHT, 720);//*/

    namedWindow("QR Detection", WINDOW_NORMAL);

    while (true) {
        Mat frame, gray, undistorted;
        cap >> frame;
        if (frame.empty()) break;

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        undistort(gray, undistorted, cameraMatrix, distCoeffs);

                // Конвертация из UTF-8
                wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
                wstring wideData = c
        vector<Point> points;
        string data = qrDecoder.detectAndDecode(undistorted, points);

        if (!data.empty()) {
            try {onverter.from_bytes(data);

                // Вывод в консоль
                wcout << L"Найден QR-код: " << wideData << endl;

                // Рисование рамки
                if (points.size() == 4) {
                    for (int i = 0; i < 4; ++i) {
                        line(undistorted, points[i], points[(i + 1) % 4], Scalar(0, 255, 0), 3);
                    }
                }

                // Вывод текста на изображение
                putText(undistorted, data, Point(20, 40),
                    FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0, 0, 255), 2);
            }
            catch (const exception& e) {
                cerr << "Ошибка декодирования: " << e.what() << endl;
            }
        }//*/

        imshow("QR Detection", undistorted/*frame */);
        if (waitKey(30) == 27) break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}