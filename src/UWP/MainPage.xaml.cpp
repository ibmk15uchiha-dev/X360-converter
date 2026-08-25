#ifndef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <filesystem>
#include <thread>
#include <ppltasks.h>

#include "MainPage.xaml.h"

// XGDTool backend includes
#pragma warning(push)
#pragma warning(disable: 4996)
#include "../InputHelper/Types.h"
#include "../InputHelper/InputHelper.h"
#include "../XGDLog.h"
#include "../XGD.h"
#pragma warning(pop)

using namespace XBConverterUWP;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage;
using namespace concurrency;
using namespace Windows::UI::Core;
using namespace Windows::Gaming::Input;
using namespace Windows::System;

static std::string PlatformStringToStdString(Platform::String^ ps) {
    if (ps == nullptr || ps->IsEmpty()) return "";
    int len = WideCharToMultiByte(CP_UTF8, 0, ps->Data(), -1, nullptr, 0, nullptr, nullptr);
    if (len <= 0) return "";
    std::string str(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, ps->Data(), -1, &str[0], len, nullptr, nullptr);
    return str;
}

static Platform::String^ StdStringToPlatformString(const std::string& str) {
    if (str.empty()) return ref new Platform::String();
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    if (len <= 0) return ref new Platform::String();
    std::wstring wstr(len - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
    return ref new Platform::String(wstr.c_str());
}

MainPage::MainPage()
{
    InitializeComponent();
    m_inputFilePath = "";
    m_outputDirPath = "";
    m_isProcessing = false;
    m_isPaused = false;
    m_wasYPressed = false;
    m_wasXPressed = false;

    // Start Gamepad polling timer for smooth thumbstick scrolling & Y/X button triggers
    m_gamepadTimer = ref new DispatcherTimer();
    TimeSpan interval;
    interval.Duration = 30 * 10000; // 30 milliseconds (in 100ns units)
    m_gamepadTimer->Interval = interval;
    m_gamepadTimer->Tick += ref new EventHandler<Object^>(this, &MainPage::OnGamepadTimerTick);
    m_gamepadTimer->Start();
}

void MainPage::OnGamepadTimerTick(Platform::Object^ sender, Platform::Object^ e)
{
    auto gamepads = Gamepad::Gamepads;
    if (gamepads->Size > 0)
    {
        auto pad = gamepads->GetAt(0);
        auto reading = pad->GetCurrentReading();

        // Right thumbstick vertical scrolling: reading.RightThumbstickY is in [-1.0, 1.0]
        if (std::abs(reading.RightThumbstickY) > 0.15f)
        {
            double scrollAmount = -reading.RightThumbstickY * 25.0;
            MainScrollViewer->ChangeView(nullptr, MainScrollViewer->VerticalOffset + scrollAmount, nullptr, true);
        }

        // Y button: Start process
        bool yPressed = (reading.Buttons & GamepadButtons::Y) == GamepadButtons::Y;
        if (yPressed && !m_wasYPressed && !m_isProcessing)
        {
            StartProcessing();
        }
        m_wasYPressed = yPressed;

        // X button: Refresh / Reset
        bool xPressed = (reading.Buttons & GamepadButtons::X) == GamepadButtons::X;
        if (xPressed && !m_wasXPressed && !m_isProcessing)
        {
            ResetUI();
        }
        m_wasXPressed = xPressed;
    }
}

void MainPage::OnPageKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
    auto key = e->Key;
    if (key == VirtualKey::GamepadY || key == VirtualKey::Y)
    {
        if (!m_isProcessing)
        {
            StartProcessing();
            e->Handled = true;
        }
    }
    else if (key == VirtualKey::GamepadX || key == VirtualKey::X)
    {
        if (!m_isProcessing)
        {
            ResetUI();
            e->Handled = true;
        }
    }
    else if (key == VirtualKey::GamepadRightThumbstickUp)
    {
        MainScrollViewer->ChangeView(nullptr, (std::max)(0.0, MainScrollViewer->VerticalOffset - 60.0), nullptr);
        e->Handled = true;
    }
    else if (key == VirtualKey::GamepadRightThumbstickDown)
    {
        MainScrollViewer->ChangeView(nullptr, MainScrollViewer->VerticalOffset + 60.0, nullptr);
        e->Handled = true;
    }
}

void MainPage::BtnSelectInput_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FileOpenPicker^ openPicker = ref new FileOpenPicker();
    openPicker->ViewMode = PickerViewMode::List;
    openPicker->SuggestedStartLocation = PickerLocationId::ComputerFolder;
    openPicker->FileTypeFilter->Append(".iso");
    openPicker->FileTypeFilter->Append(".zar");
    openPicker->FileTypeFilter->Append(".cci");
    openPicker->FileTypeFilter->Append(".cso");
    openPicker->FileTypeFilter->Append(".xbe");
    openPicker->FileTypeFilter->Append("*");

    create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file)
        {
            m_inputFilePath = file->Path;
            TxtInputFile->Text = file->Path;
            LogMessage("Selected input: " + file->Path);
        }
    });
}

void MainPage::BtnSelectOutput_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    FolderPicker^ folderPicker = ref new FolderPicker();
    folderPicker->SuggestedStartLocation = PickerLocationId::ComputerFolder;
    folderPicker->FileTypeFilter->Append("*");

    create_task(folderPicker->PickSingleFolderAsync()).then([this](StorageFolder^ folder)
    {
        if (folder)
        {
            m_outputDirPath = folder->Path;
            TxtOutputDir->Text = folder->Path;
            LogMessage("Selected output directory: " + folder->Path);
        }
    });
}

void MainPage::BtnConvert_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    StartProcessing();
}

void MainPage::BtnReset_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ResetUI();
}

void MainPage::BtnPause_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    m_isPaused = !m_isPaused;
    BtnPause->Content = m_isPaused ? "Resume" : "Pause";
    TxtStatus->Text = m_isPaused ? "Paused" : "Processing...";
    LogMessage(m_isPaused ? "Processing paused." : "Processing resumed.");
}

void MainPage::BtnCancel_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    TxtStatus->Text = "Cancelled";
    LogMessage("Process cancelled by user.");
}

void MainPage::ResetUI()
{
    m_inputFilePath = "";
    m_outputDirPath = "";
    TxtInputFile->Text = "";
    TxtOutputDir->Text = "";
    CmbOutputFormat->SelectedIndex = 0;
    RadScrubNone->IsChecked = true;
    ChkSplitXiso->IsChecked = false;
    ChkAttachXbe->IsChecked = false;
    ChkAllowedMedia->IsChecked = false;
    ChkRenameXbe->IsChecked = false;
    TxtStatus->Text = "Idle";
    ProgBar->IsIndeterminate = false;
    ProgBar->Value = 0;
    TxtLog->Text = "Ready for new conversion.\n";
    BtnConvert->IsEnabled = true;
    BtnReset->IsEnabled = true;
    BtnPause->IsEnabled = false;
    BtnCancel->IsEnabled = false;
}

void MainPage::LogMessage(Platform::String^ msg)
{
    TxtLog->Text += msg + "\n";
}

void MainPage::StartProcessing()
{
    if (m_isProcessing) return;

    if (m_inputFilePath == nullptr || m_inputFilePath->IsEmpty())
    {
        TxtStatus->Text = "Error: No input selected";
        LogMessage("Please select an input image/file first.");
        return;
    }

    m_isProcessing = true;
    m_isPaused = false;
    BtnConvert->IsEnabled = false;
    BtnReset->IsEnabled = false;
    BtnPause->IsEnabled = true;
    BtnCancel->IsEnabled = true;
    TxtStatus->Text = "Processing input files...";
    ProgBar->IsIndeterminate = true;

    std::string inPath = PlatformStringToStdString(m_inputFilePath);
    std::string outPath = PlatformStringToStdString(m_outputDirPath);

    int formatIdx = CmbOutputFormat->SelectedIndex;
    bool bScrubFull = RadScrubFull->IsChecked->Value;
    bool bScrubPartial = RadScrubPartial->IsChecked->Value;
    bool bSplit = ChkSplitXiso->IsChecked->Value;
    bool bAttach = ChkAttachXbe->IsChecked->Value;
    bool bAllowedMedia = ChkAllowedMedia->IsChecked->Value;
    bool bRename = ChkRenameXbe->IsChecked->Value;

    LogMessage("Starting conversion for: " + m_inputFilePath);

    std::thread([this, inPath, outPath, formatIdx, bScrubFull, bScrubPartial, bSplit, bAttach, bAllowedMedia, bRename]() {
        OutputSettings settings;

        switch (formatIdx)
        {
            case 0: settings.file_type = FileType::DIR; break;
            case 1: settings.file_type = FileType::ISO; break;
            case 2: settings.file_type = FileType::GoD; break;
            case 3: settings.file_type = FileType::CCI; break;
            case 4: settings.file_type = FileType::CSO; break;
            case 5: settings.file_type = FileType::ZAR; break;
            case 6: settings.auto_format = AutoFormat::OGXBOX; break;
            case 7: settings.auto_format = AutoFormat::XBOX360; break;
            case 8: settings.auto_format = AutoFormat::XEMU; break;
            case 9: settings.auto_format = AutoFormat::XENIA; break;
            default: settings.file_type = FileType::ISO; break;
        }

        if (bScrubFull) settings.scrub_type = ScrubType::FULL;
        else if (bScrubPartial) settings.scrub_type = ScrubType::PARTIAL;
        else settings.scrub_type = ScrubType::NONE;

        settings.split = bSplit;
        settings.attach_xbe = bAttach;
        settings.allowed_media_patch = bAllowedMedia;
        settings.rename_xbe = bRename;
        settings.offline_mode = true; // Offline mode for UWP / Xbox DevMode

        std::filesystem::path pIn(inPath);
        std::filesystem::path pOut;
        if (!outPath.empty()) pOut = outPath;

        try {
            InputHelper helper(std::filesystem::absolute(pIn), pOut, settings);
            helper.process_all();

            auto failed = helper.failed_inputs();
            bool hasErrors = !failed.empty();

            Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, hasErrors]() {
                if (hasErrors) {
                    TxtStatus->Text = "Completed with errors";
                    LogMessage("Process completed with some errors.");
                } else {
                    TxtStatus->Text = "Processing complete";
                    ProgBar->IsIndeterminate = false;
                    ProgBar->Value = 100;
                    LogMessage("Conversion completed successfully!");
                }
                m_isProcessing = false;
                BtnConvert->IsEnabled = true;
                BtnReset->IsEnabled = true;
                BtnPause->IsEnabled = false;
                BtnCancel->IsEnabled = false;
            }));

        } catch (const std::exception& ex) {
            std::string errMsg = ex.what();
            Platform::String^ pMsg = StdStringToPlatformString(errMsg);
            Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, pMsg]() {
                TxtStatus->Text = "Error occurred";
                LogMessage("Error: " + pMsg);
                m_isProcessing = false;
                ProgBar->IsIndeterminate = false;
                BtnConvert->IsEnabled = true;
                BtnReset->IsEnabled = true;
                BtnPause->IsEnabled = false;
                BtnCancel->IsEnabled = false;
            }));
        }
    }).detach();
}
