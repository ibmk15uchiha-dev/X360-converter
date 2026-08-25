#include "App.xaml.h"
#include "MainPage.xaml.h"

using namespace XBConverterUWP;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

App::App()
{
    InitializeComponent();
    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
}

void App::OnLaunched(LaunchActivatedEventArgs^ e)
{
    Frame^ rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

    if (rootFrame == nullptr)
    {
        rootFrame = ref new Frame();
        rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

        if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
        {
            // Restore saved session state
        }

        Window::Current->Content = rootFrame;
    }

    if (e->PrelaunchActivated == false)
    {
        if (rootFrame->Content == nullptr)
        {
            rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
        }
        Window::Current->Activate();
    }
}

void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
    auto deferral = e->SuspendingOperation->GetDeferral();
    // Save application state and stop background activity
    deferral->Complete();
}

void App::OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e)
{
    throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}
