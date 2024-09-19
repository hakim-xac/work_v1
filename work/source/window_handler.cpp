#include "../include/window_handler.h"
#include "../include/resource.h"
#include "../include/HdcScoped.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <memory>

namespace LAB
{
	//--------------------------
	
    std::atomic_bool WindowHandler::m_is_running = true;

    //--------------------------
	
    WindowHandler::WindowHandler(HWND hwnd) :
        m_hwnd{ hwnd },
        m_hdc { GetDC(m_hwnd) }
    {
        CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(vehicleThread), 0, 0, &m_thread_ids[0]);
        CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(fuelProcurementPointThread), 0, 0, &m_thread_ids[1]);
        CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(powerUnitsThread), reinterpret_cast<LPVOID>(0), 0, &m_thread_ids[2]);
        CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(powerUnitsThread), reinterpret_cast<LPVOID>(1), 0, &m_thread_ids[3]);
        CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(powerUnitsThread), reinterpret_cast<LPVOID>(2), 0, &m_thread_ids[4]);
        CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(powerUnitsThread), reinterpret_cast<LPVOID>(3), 0, &m_thread_ids[5]);
    }

	//--------------------------
	
    WindowHandler::~WindowHandler()
    {
        ReleaseDC(m_hwnd, m_hdc);
    }
    
    //--------------------------
	
    LRESULT CALLBACK
        WindowHandler::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_CREATE:
            SetTimer(hWnd, 1, 20, nullptr);
            break;
        case WM_TIMER:
            InvalidateRect(hWnd, nullptr, false);
            break;
        case WM_COMMAND:
        {
            int wmId{ LOWORD(wParam) };
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        case WM_PAINT:
        {
            LAB::HdcScoped hdc_scoped{ hWnd };
            HDC hdc{ hdc_scoped.getHDC() };
            g_drawer.draw(hdc);
        }
        break;
        case WM_DESTROY:
            m_is_running = false;
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }

    //--------------------------

    void WindowHandler::vehicleThread()
    {
        int32_t capacity{};
        int32_t delay{};
        g_drawer.drawRectangle(480, 40, 499, 60, Color{ 200, 200, 200 });
        MSG msg{};
        PeekMessageW(&msg, 0, 0, 0, PM_REMOVE);
        GetMessageW(&msg, 0, 0, 0);
        for (int32_t i{ 480 }; i >= 60; i -= 10)
        {
            g_drawer.drawRectangle(i, 40, i + 19, 60, Color{ 200, 200, 200 });

            std::this_thread::sleep_for(std::chrono::milliseconds{ 20 });

            if (i > 60)
                g_drawer.drawRectangle(i, 40, i + 19, 60, Color{ 255, 255, 255 });
        }

        while (m_is_running)
        {
            PostThreadMessageW(m_thread_ids[1], WM_USER + 4, 0, 0);
            GetMessageW(&msg, 0, 0, 0);
            capacity = static_cast<int32_t>(msg.wParam);

            int32_t boiler_number{ static_cast<int32_t>(msg.lParam) };
            delay = capacity * 7;

            for (int32_t i{ 60 }; i <= 200 + 80 * boiler_number; i += 10)
            {
                g_drawer.drawRectangle(i, 40, i + 19, 60, Color{ 200, 240, 200 });

                std::this_thread::sleep_for(std::chrono::milliseconds{ delay });

                if (i < 200 + 80 * boiler_number)
                {
                    g_drawer.drawRectangle(i, 40, i + 19, 60, Color{ 255, 255, 255 });
                }
            }

            PostThreadMessageW(m_thread_ids[2 + boiler_number], WM_USER, capacity, 0);
            GetMessageW(&msg, 0, 0, 0);

            for (int32_t i{ 200 + 80 * boiler_number }; i >= 60; i -= 10)
            {
                g_drawer.drawRectangle(i, 40, i + 19, 60, Color{ 200, 200, 200 });
                std::this_thread::sleep_for(std::chrono::milliseconds{ 20 });

                if (i > 60)
                    g_drawer.drawRectangle(i, 40, i + 19, 60, Color{ 255, 255, 255 });
            }

        }

    }

	//--------------------------
     
    void WindowHandler::fuelProcurementPointThread()
    {
        auto random{ [](int32_t left_range, int32_t right_range)
        {
            std::default_random_engine re{ std::invoke(std::random_device{}) };
            std::uniform_int_distribution<int32_t> uniform_dist{ left_range, right_range };
            return uniform_dist(re);
        } };

        int32_t capacity{};
        MSG msg{};
        PeekMessageW(&msg, 0, 0, 0, PM_REMOVE);

        std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });

        PostThreadMessageW(m_thread_ids[0], WM_USER, 0, 0);

        int32_t prev_boiler{};

        while (m_is_running)
        {
            capacity = std::invoke(random, 0, 7) + 5;

            for (int32_t i{}; i < capacity; ++i)
            {
                g_drawer.drawRectangle(40, 85 + i * 20, 99, 85 + i * 20 + 19, Color{ 200, 240, 200 });

                std::this_thread::sleep_for(std::chrono::milliseconds{ 250 });
            }

            GetMessageW(&msg, 0, WM_USER + 4, WM_USER + 4);
            GetMessageW(&msg, 0, WM_USER, WM_USER + 3);

            int32_t boiler_number{ std::invoke(random, 0, 3) };
            for (; boiler_number == prev_boiler; boiler_number = std::invoke(random, 0, 3));
            prev_boiler = boiler_number;

            for (int32_t i{ capacity - 1 }; i >= 0; --i)
            {
                g_drawer.drawRectangle(40, 85 + i * 20, 99, 85 + i * 20 + 19, Color{ 255, 255, 255 });
                std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
            }

            PostThreadMessage(m_thread_ids[0], WM_USER, capacity, boiler_number);
        }
    }

	//--------------------------
     
    void WindowHandler::powerUnitsThread(int32_t n)
    {
        int32_t capacity{};
        int32_t pos{ 180 + n * 80 };
        MSG msg{};
        PeekMessageW(&msg, 0, 0, 0, PM_REMOVE);

        PostThreadMessageW(m_thread_ids[1], WM_USER + n, n, 0);

        while (m_is_running)
        {
            GetMessageW(&msg, 0, 0, 0);
            capacity = static_cast<int32_t>(msg.wParam);
            for (int32_t i{}; i < capacity; ++i)
            {
                g_drawer.drawRectangle(pos, 85 + i * 20, pos + 59, 85 + i * 20 + 19, Color{ 255, 250, 125 });
                std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
            }
            PostThreadMessageW(m_thread_ids[0], WM_USER, 0, 0);
            for (int32_t i{ capacity - 1 }; i >= 0; --i)
            {
                g_drawer.drawRectangle(pos, 85 + i * 20, pos + 59, 85 + i * 20 + 19, Color{ 255, 255, 255 });
                std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
            }
            PostThreadMessageW(m_thread_ids[1], WM_USER + n, n, 0);
        }
    }

	//--------------------------
     
    void WindowHandler::moveSquare2() const
    {

        //std:/*:shared_ptr<SharedData> shared_data_shptr{ g_settings.getSharedData() };
        //Square& squarte_data{ shared_data_shptr.get()->square2 };

        //while (m_is_running)
        //{
        //    squarte_data.pos_x -= 4;
        //    std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
        //}*/
    }

	//--------------------------
}