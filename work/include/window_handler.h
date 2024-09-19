#pragma once

#include "common.h"
#include "Drawer.h"

//--------------------------
extern LAB::Drawer g_drawer;
//--------------------------
namespace LAB
{

	class WindowHandler final
	{
	public:

		//--------------------------
		
		explicit WindowHandler(HWND hwnd);
		
		//--------------------------
		
		~WindowHandler();

		//--------------------------
		
		static void fuelProcurementPointThread();

		//--------------------------
		
		static void vehicleThread();

		//--------------------------
		
		static void powerUnitsThread(int32_t n);

		//--------------------------
		
		void moveSquare2() const;

		//--------------------------
		
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		//--------------------------

	private:

		HWND m_hwnd;
		HDC m_hdc;
		mutable std::mutex m_draw_mx;
		std::unordered_map<size_t, std::jthread> m_threads;

		static inline std::array<unsigned long, 6> m_thread_ids { 1, 2, 3, 4, 5, 6 };
		static std::atomic_bool m_is_running;
	};
}
