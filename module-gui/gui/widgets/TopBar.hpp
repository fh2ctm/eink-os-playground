// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>

#include <PhoneModes/Common.hpp>

#include <vector>
#include <map>

namespace gui
{
    class Label;
    class Image;

    namespace top_bar
    {
        class SignalStrengthBase;
        class PhoneMode;
        class BatteryBase;
        class NetworkAccessTechnology;
        class SIM;
        class Time;
        class Lock;
    } // namespace top_bar
} // namespace gui

class UTF8;

namespace gui::top_bar
{

    enum class Indicator
    {
        Signal,                  /// signal strenght
        Time,                    /// digital clock
        Lock,                    /// is phone locked
        Battery,                 /// battery status
        SimCard,                 /// sim card info
        NetworkAccessTechnology, /// NAT (eg 3G, 4G, LTE)
        PhoneMode,               /// phone mode
    };

    using Indicators        = std::vector<Indicator>;
    using IndicatorStatuses = std::map<Indicator, bool>;

    enum class TimeMode
    {
        Time12h, /// 12h time format
        Time24h  /// 24h time format
    };

    /// Carries the top bar configuration.
    class Configuration
    {
      public:
        /// Enable specified indicator
        /// @param indicator indicator type to be enabled
        void enable(Indicator indicator);

        /// Enable number of specified indicators
        /// @param indicators vectior of indicators to enable
        void enable(const Indicators &indicators);

        /// Disable specified indicator
        /// @param indicator indicator type to disable
        void disable(Indicator indicator);

        /// Set the state of specified indicator
        /// @param indicator indicator type to set state
        /// @param enabled desired status to be set (true=enabled, false=disabled)
        void setIndicator(Indicator indicator, bool enabled);

        /// Set time mode (12h/24h)
        /// @param timeMode desired time mode configuration
        void setTimeMode(TimeMode timeMode);

        /// Set phone mode (connected/dnd/offline)
        /// @param phoneMode desired phone mode configuration
        void setPhoneMode(sys::phone_modes::PhoneMode phoneMode);

        /// Return the time mode configuration
        /// @return cuurent time mode configuration
        [[nodiscard]] auto getTimeMode() const noexcept -> TimeMode;

        /// Get the phone mode configuration
        /// @return phone mode
        [[nodiscard]] auto getPhoneMode() const noexcept -> sys::phone_modes::PhoneMode;

        /// Check if the specified indicator is enabled
        /// @param indicator indicator to be checked
        /// @return indicator status
        [[nodiscard]] auto isEnabled(Indicator indicator) const -> bool;

        /// Return the indicator statuses
        /// @return indicator statuses
        [[nodiscard]] auto getIndicatorsConfiguration() const noexcept -> const IndicatorStatuses &;

      private:
        /// Current indicator statuses
        IndicatorStatuses indicatorStatuses = {{Indicator::Signal, false},
                                               {Indicator::PhoneMode, false},
                                               {Indicator::Time, false},
                                               {Indicator::Lock, false},
                                               {Indicator::Battery, false},
                                               {Indicator::SimCard, false},
                                               {Indicator::NetworkAccessTechnology, false}};

        /// Time mode
        TimeMode mTimeMode = TimeMode::Time12h;

        /// Phone mode
        sys::phone_modes::PhoneMode mPhoneMode = sys::phone_modes::PhoneMode::Connected;
    };

    /// Top bar widget class.
    /// This is horizontal box with three sections
    /// * left showing signal strenght, NAT info, and/or phone mode
    /// * center showing lock info or digital clock
    /// * right showing sim card and battery status
    class TopBar : public HBox
    {
      public:
        /// Constructor
        /// @param parent parent item pointer
        /// @param x widget x position
        /// @param y widget y position
        /// @param w widget width
        /// @param h widget height
        TopBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

        /// Set the configuration basing on the specialized gui::top_bar::Configuration class
        /// @param configuration desired configuration
        void configure(Configuration &&config);

        /// Returns the current configuration
        /// @return configuration stored in Configuration class
        [[nodiscard]] auto getConfiguration() const noexcept -> const Configuration &;

        /// Update sim card status widget state depending on the current configuration
        bool updateSim();

        /// Update clock widget state depending on the current configuration
        bool updateTime();

        /// Update battery status widget state depending on the current configuration
        bool updateBattery();

        /// Update signal widget state depending on the current configuration
        bool updateSignalStrength();

        /// Update phone mode widget state depending on the current configuration
        bool updatePhoneMode();

        /// Update NAT widget state depending on the current configuration
        bool updateNetworkAccessTechnology();

        /// Accepts GuiVisitor to update the top bar
        void accept(GuiVisitor &visitor) override;

      protected:
        /// Set up and add all the widgets to the top bar
        void prepareWidget();

        /// Show/hide sim card status widget
        /// @param enabled true to show false to hide the widget
        void showSim(bool enabled);

        /// Show/hide clock widget
        /// @param enabled true to show false to hide the widget
        void showTime(bool enabled);

        /// Show/hide lock status widget
        /// @param enabled true to show false to hide the widget
        void showLock(bool enabled);

        /// Show/hide battery status widget
        /// @param enabled true to show false to hide the widget
        void showBattery(bool enabled);

        /// Show/hide signal strenght widget
        /// @param enabled true to show false to hide the widget
        void showSignalStrength(bool enabled);

        /// Show/hide phone mode widget
        /// @param enabled true to show false to hide the widget
        void showPhoneMode(bool enabled);

        /// Show/hide NAT widget
        /// @param enabled true to show false to hide the widget
        void showNetworkAccessTechnology(bool enabled);

        /// Sets the status of the specified indicator on the top bar
        /// @param indicator indicator id
        /// @param enabled enable or disable the specified indicator
        void setIndicatorStatus(Indicator indicator, bool enabled);

        /// Pointer to widget showing digital clock
        Time *time = nullptr;

        /// Pointer to widget showing NAT (eg 3G, 4G, LTE)
        NetworkAccessTechnology *networkAccessTechnology = nullptr;

        /// Pointer to widget showing current signal strenght
        SignalStrengthBase *signal = nullptr;

        /// Pointer to widget with current phone mode
        PhoneMode *phoneMode = nullptr;

        /// Pointer to widget showing lock status
        Lock *lock = nullptr;

        /// Pointer to widget with sim card status
        SIM *sim = nullptr;

        /// Pointer to widget with battery status
        BatteryBase *battery = nullptr;

        /// Pointer to the left horizontal box
        HBox *leftBox = nullptr;

        /// Pointer to the central horizontal box
        HBox *centralBox = nullptr;

        /// Pointer to the right horizontal box
        HBox *rightBox = nullptr;

        /// Current configuration of the TopBar
        Configuration configuration;
    };

} // namespace gui::top_bar
