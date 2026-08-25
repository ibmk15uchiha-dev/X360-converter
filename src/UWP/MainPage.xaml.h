#pragma once

#include "MainPage.g.h"

namespace XBConverterUWP
{
    public ref class MainPage sealed
    {
    public:
        MainPage();

    private:
        void BtnSelectInput_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void BtnSelectOutput_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void BtnConvert_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void BtnReset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void BtnPause_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void BtnCancel_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnPageKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
        void OnGamepadTimerTick(Platform::Object^ sender, Platform::Object^ e);

        void StartProcessing();
        void ResetUI();
        void LogMessage(Platform::String^ msg);

        Platform::String^ m_inputFilePath;
        Platform::String^ m_outputDirPath;
        Windows::UI::Xaml::DispatcherTimer^ m_gamepadTimer;
        bool m_isProcessing;
        bool m_isPaused;
        bool m_wasYPressed;
        bool m_wasXPressed;
    };
}
